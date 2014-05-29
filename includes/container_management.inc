<?php

/**
 * @file
 * Contains files to manage creation and population of full critical edition.
 */

/**
 * Attaches and detaches versionable objects from Critical Edition object.
 *
 * @param array $form
 *   The form
 * @param array $form_state
 *   The form state
 * @param AbstractObject $object
 *   Fedora Object representing Critical Edition Object
 *
 * @return array
 *   Drupal form.
 */
function islandora_critical_edition_attach_versionable(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $empty_message = t("There are no versionable objects in this Fedora Repository.");
  $objects = islandora_get_versionable_objects();
  $chosen = array_keys(islandora_critical_edition_get_members($object->id, 'islandora:versionableObjectCModel'));
  $comparator = function ($a, $b) use ($chosen) {
    $a_val = $b_val = 0;
    if (in_array($a, $chosen)) {
      $a_val = 1;
    }
    if (in_array($b, $chosen)) {
      $b_val = 1;
    }
    return $b_val - $a_val;
  };
  uksort($objects, $comparator);
  $rows = array();
  $defaults = array();
  foreach ($objects as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
    $defaults[$pid] = in_array($pid, $chosen);
  }
  $header = array(
    'title' => array('data' => t('Versionable Object Title')),
    'pid' => array('data' => t('PID')),
  );

  $table_element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#default_value' => $defaults,
    '#empty' => $empty_message,
    '#js' => FALSE,
  );

  $form['versionable_objects'] = $table_element;
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Associate these Objects'),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  return $form;
}

/**
 * Submit handler for islandora_critical_edition_attach_versionable().
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form_state
 *
 * @return NULL
 *   returns if nothing chosen.
 */
function islandora_critical_edition_attach_versionable_submit($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  if (!isset($form_state['values']['versionable_objects'])) {
    return;
  }
  $desired_objects = array_filter(array_values($form_state['values']['versionable_objects']));
  $attached = islandora_critical_edition_get_members($form_state['values']['pid'], "islandora:versionableObjectCModel");
  $existing_objects = array_keys($attached);
  $to_add = array_diff($desired_objects, $existing_objects);
  $to_remove = array_diff($existing_objects, $desired_objects);
  if ($to_add) {
    foreach ($to_add as $candidate) {
      $temp_object = islandora_object_load($candidate);
      $temp_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $form_state['values']['pid']);
    }
  }
  if ($to_remove) {
    foreach ($to_remove as $candidate) {
      $temp_object = islandora_object_load($candidate);
      $temp_object->relationships->remove(FEDORA_RELS_EXT_URI, 'isMemberOf', $form_state['values']['pid']);
    }
  }
}

/**
 * Returns all versionable objects in the Fedora Repository.
 *
 * @return array
 *   array of PID => Label pairs
 */
function islandora_get_versionable_objects() {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $tuque = islandora_get_tuque_connection();
  $query = "PREFIX fm: <info:fedora/fedora-system:def/model#>
                SELECT DISTINCT ?subject ?label FROM <#ri>
                WHERE {
                        {?subject fm:hasModel <info:fedora/islandora:versionableObjectCModel>;
                       }
                 OPTIONAL{
                           ?subject fm:label  ?label;
                          }
                       }";
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $objects[$result['subject']['value']] = $result['label']['value'];
  }
  return $objects;
}

/**
 * Gets all versionable objects assicated with critical edition.
 *
 * @param string $critical_edition_pid
 *   PID of critical edition
 *
 * @return array
 *   PIDs of currently associated versionable objects
 */
function islandora_get_associated_versionable_objects($critical_edition_pid) {
  $tuque = islandora_get_tuque_connection();
  $query = "PREFIX fm: <" . FEDORA_MODEL_URI . ">
            PREFIX fr: <" . FEDORA_RELS_EXT_URI . ">
            SELECT ?subject
            FROM <#ri>
            WHERE {
              {?subject fm:hasModel <info:fedora/islandora:versionableObjectCModel>;
                            fm:state fm:Active;
                            fr:isMemberOfCollection <info:fedora/$critical_edition_pid>;
              }
            UNION
              {?subject fm:hasModel <info:fedora/islandora:versionableObjectCModel>;
                        fm:state fm:Active;
                        fr:isMemberOf <info:fedora/$critical_edition_pid>;
              }


             }";
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $objects[] = $result['subject']['value'];
  }
  return $objects;
}

/**
 * Displays the container forms in a set of vertical tabs.
 *
 * @param AbstractObject $object
 *   The versionable object to manage.
 *
 * @return array
 *   A renderable array containing all the management forms related to
 *   versionable objects.
 */
function islandora_critical_edition_management_menu(AbstractObject $object) {
  module_load_include('inc', 'islandora', 'includes/breadcrumb');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/critical_apparatus');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/manage_rdf_tei_attachment.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/transcriptions');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/mvd');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/versionable_object_creation.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/audio.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/video.form');
  drupal_add_css(drupal_get_path('module', 'islandora_critical_edition_advanced') . "/css/fix_textfield_width.css");
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $members = islandora_critical_edition_get_members($object->id);
  $tei_rdf = array_search('islandora:criticalEditionCModel', $members);
  $versionable_objects = array_keys($members, 'islandora:versionableObjectCModel');
  $transcriptions = array();
  foreach ($versionable_objects as $versionable_object) {
    $vo_members = islandora_critical_edition_get_members($versionable_object);
    $transcriptions = array_merge($transcriptions, array_keys($vo_members, 'islandora:transcriptionCModel'));
  }
  $apparatus = array_keys($members, 'islandora:criticalApparatusCModel');
  $output = array();
  $output['manage_critical_edition'] = array('#type' => 'vertical_tabs');

  $output['manage_critical_edition']['replace_apparatus'] = array(
    '#id' => 'replace-apparatus',
    '#access' => user_access(ISLANDORA_ADD_DS) && empty($apparatus),
    '#title' => t('Create Apparatus'),
    '#description' => t('Creates blank Critical Apparatus'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_replace_apparatus', $object),
    '#group' => 'manage_critical_edition',
  );
  $output['manage_critical_edition']['attach_vo'] = array(
    '#id' => 'attach-vo',
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Associate existing Versionable Object'),
    '#description' => t('Manage Versionable Object associations'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_attach_versionable', $object),
    '#group' => 'manage_critical_edition',
  );

  $output['manage_critical_edition']['mvd'] = array(
    '#id' => 'mvd',
    '#access' => user_access(ISLANDORA_ADD_DS) && $transcriptions,
    '#title' => t('Create Collation'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_mvd_form', $object),
    '#group' => 'manage_critical_edition',
  );
  return $output;
}
