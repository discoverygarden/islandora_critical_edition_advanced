<?php

/**
 * @file
 * Handles the management of MVD.
 */

/**
 * The form for creating a MVD object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_mvd_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $filter_options = array(
    'Empty' => 'Empty',
    'CCE' => 'CCE',
    'Novel' => 'Novel',
    'Play' => 'Play',
    'Poem' => 'Poem',
  );

  $form = array();

  $form['mvd'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of new Collation'),
    '#size' => 40,
    '#required' => TRUE,
  );
  $form['mvd_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description of new Collation'),
  );

  $members = islandora_critical_edition_get_members($object->id);
  $versionable_objects = array_keys($members, 'islandora:versionableObjectCModel');
  $transcriptions = array();
  foreach ($versionable_objects as $versionable_object) {
    $vo_members = islandora_critical_edition_get_members($versionable_object);
    $transcriptions = array_merge($transcriptions, array_keys($vo_members, 'islandora:transcriptionCModel'));
  }
  $rows = array();

  foreach ($transcriptions as $transcription) {
    $transcription_object = islandora_object_load($transcription);
    $rows[$transcription] = array(
      'pid' => l($transcription, "islandora/object/$transcription"),
      'title' => l($transcription_object->label, "islandora/object/$transcription"),
    );
  }
  $header = array(
    'title' => array('data' => t('Transcription Title')),
    'pid' => array('data' => t('PID')),
  );
  $table_element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t("There are no transcriptions associated with this Critical Edition."),
  );

  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $form['filter'] = array(
    '#type' => 'select',
    '#title' => t('Filter'),
    '#options' => $filter_options,
  );

  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => t("Choose transcriptions of @label to compare.", array('@label' => $object->label)),
  );
  $form['transcriptions'] = $table_element;
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Create Collation'),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  return $form;
}

/**
 * Drupal Form Validation.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal Form State
 */
function islandora_critical_edition_mvd_form_validate($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  $critical_edition_object = islandora_object_load($form_state['values']['pid']);
  $mvds = islandora_calliope_find_mvds($critical_edition_object);
  $match = FALSE;
  foreach ($mvds as $mvd) {
    if ($mvd['name'] == $form_state['values']['mvd']) {
      form_set_error('mvd', t('A collation with this name is already associated with this critical edition.'));
    }
  }
}

/**
 * Sumit handler for MVD form.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal Form State
 */
function islandora_critical_edition_mvd_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  module_load_include('inc', 'islandora', 'includes/utilities');
  $transcriptions = array_filter($form_state['values']['transcriptions']);
  $mvd = islandora_critical_edition_advanced_build_mvd($form_state['values']['pid'], $form_state['values']['mvd'], $transcriptions);
  $critical_edition_pid = $form_state['values']['pid'];
  $mvd_title = $form_state['values']['mvd'];
  $mvd_description = $form_state['values']['mvd_description'];
  $filter = $form_state['values']['filter'];
  $success = islandora_calliope_create_mvd($critical_edition_pid, $mvd_title, $mvd_description, $transcriptions, $filter);
  drupal_set_message(check_plain($success));
}

/**
 * Builds MVD object in repository.
 *
 * @param string $critical_edition
 *   PID of critical edition
 * @param string $title
 *   Title for MVD
 * @param array $transcriptions
 *   Transcriptions to be included in MVD
 *
 * @return AbstractObject
 *   MVD object
 */
function islandora_critical_edition_advanced_build_mvd($critical_edition, $title, $transcriptions) {
  $fedora_object = islandora_object_load($critical_edition);
  $tuque = new IslandoraTuque();
  $namespace = islandora_get_namespace($fedora_object->id);
  $mvd = $tuque->repository->constructObject($namespace);
  $mvd->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $fedora_object->id);
  $mvd->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:mvdCModel');
  $mvd->label = $title;
  $mvd->owner = $fedora_object->owner;
  $tn_path = drupal_get_path('module', 'islandora_critical_edition_advanced') . '/images/calliope.jpg';
  $tn = $mvd->constructDatastream('TN');
  $tn->label = 'Thumbnail';
  $tn->mimetype = 'image/jpg';
  $tn->setContentFromFile($tn_path);
  $mvd->ingestDatastream($tn);
  $new_object = $tuque->repository->ingestObject($mvd);
  foreach ($transcriptions as $transcription) {
    $transcription_object = islandora_object_load($transcription);
    $transcription_object->relationships->add(FEDORA_RELS_EXT_URI, 'isPartOf', $new_object->id);
  }
  return $new_object;
}

/**
 * Handles all crud operations for MVDs.
 *
 * @param string $critical_edition_pid
 *   PID of Critical Edition
 * @param string $mvd_label
 *   Identifier for MVD
 * @param string $operation
 *   action to perform
 */
function islandora_critical_edition_advanced_mvd_crud($critical_edition_pid, $mvd_label, $operation) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  $query = array(
    'metadata.critical_edition' => $critical_edition_pid,
    'metadata.name' => $mvd_label,
  );
  $mvds = islandora_calliope_find_mvd($query);
  $mvd = $mvds[0];
  $mvd_description = isset($mvd['metadata']['description']) ? $mvd['metadata']['description'] : t("No description");

  switch ($operation) {
    case 'refresh':
      foreach ($mvd['metadata']['resources'] as $transcription) {
        $resource_ids[] = $transcription['id'];
      }
      $filter = $mvd['metadata']['filter'];
      islandora_calliope_create_mvd($critical_edition_pid, $mvd_label, $mvd_description, $resource_ids, $filter);
      drupal_goto("islandora/object/$critical_edition_pid/edit");

      break;

    case 'delete':
      islandora_calliope_delete_mvd($mvd['_id']);
      drupal_set_message(t("Collation successfully removed"));
      drupal_goto("islandora/object/$critical_edition_pid");
      break;

    case 'compare':
      drupal_goto("collationtools/compare", array('fragment' => "$mvd_label/$critical_edition_pid"));
      break;

    case 'apparatus':
      drupal_goto("collationtools/apparatus", array('fragment' => "$mvd_label/$critical_edition_pid"));
      break;

    case 'list':
      $cursor = islandora_calliope_compare_callback($critical_edition_pid);
      $results = array();
      foreach ($cursor as $obj) {
        $results['results'][] = $obj['metadata'];
      }
      echo json_encode($results);
      break;

    default:
      break;
  }
}
