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
    'Poem' => 'Poem'
  );
  
  $form = array();
  
  $form['mvd'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of new MVD'),
    '#size' => 40,
    '#required' => TRUE,
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

  $form['project'] = array(
    '#type' => 'textfield',
    '#title' => t('Project'),
    '#default_value' => 'islandora'
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
    '#value' => t('Create MVD'),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  return $form;
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
  $project = $form_state['values']['project'];
  $mvd_title = $form_state['values']['mvd'];
  $filter = $form_state['values']['filter'];
  $success = islandora_calliope_create_mvd($project, $mvd_title, $transcriptions, $filter);
  drupal_set_message($success);
}

/**
 * Builds MVD object in repository.
 *
 * @param String $critical_edition
 *   PID of critical edition
 * @param String $title
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
