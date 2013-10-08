<?php

/**
 * @file
 * Contains files to manage creation and population of full critical edition.
 */

/**
 * Creates form to manage Critical Edition.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   Object representing Critical Edition container
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_container_management(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $path = drupal_get_path('module', 'islandora_critical_edition');
  drupal_add_css("$path/css/critical_edition_management.css");
  $versionable_objects = array_keys(islandora_critical_edition_get_members($object->id, 'islandora:versionableObjectCModel'));
  $transcriptions = array();
  foreach ($versionable_objects as $versionable_object) {
    $transcriptions = array_merge($transcriptions, array_keys(islandora_critical_get_related_transcriptions($versionable_object)));
  }

  drupal_add_css("$path/css/critical_edition_management.css");
  $form = array();
  $form['create_new'] = array(
    '#type' => 'submit',
    '#value' => t('Create New Versionable Object'),
    '#attributes' => array('source' => 'new_versionable', 'class' => array('critical_edition_submit')),
  );
  $form['attach'] = array(
    '#type' => 'submit',
    '#value' => t('Attach Versionable Object to Critical Edition'),
    '#attributes' => array('source' => 'attach_versionable', 'class' => array('critical_edition_submit')),
  );

  $form['include_tei_rdf'] = array(
    '#type' => 'submit',
    '#value' => t('Include RDF-TEI Object'),
    '#attributes' => array('source' => 'include_rdf', 'class' => array('critical_edition_submit')),
  );

  if (!empty($transcriptions)) {
    $form['mvd'] = array(
      '#type' => 'submit',
      '#value' => t('Create/Update MVD'),
      '#attributes' => array('source' => 'make_mvd', 'class' => array('critical_edition_submit')),
    );
  }
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  return $form;
}

/**
 * Submit handler for critical edition management.
 *
 * @param array $form
 *   The form.
 * @param array $form_state
 *   The form state.
 */
function islandora_critical_edition_container_management_submit(array $form, array &$form_state) {
  $action = $form_state['triggering_element']['#attributes']['source'];
  $pid = $form_state['values']['pid'];

  switch ($action) {

    case 'new_versionable':
      drupal_goto("islandora/critical_edition/$pid/create/versionable_object");
      break;

    case 'attach_versionable':
      drupal_goto("islandora/critical_edition/$pid/associate");
      break;

    case 'make_mvd':
      drupal_goto("islandora/critical_edition/$pid/mvd");
      break;
  }
}

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
  // Build and return table element.
  $table_element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#default_value' => $defaults,
    '#empty' => t("There are no versionable objects in this Fedora Repository."),
    '#js' => FALSE,
  );
  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => t("Choose existing versionable object to associate with @label.", array('@label' => $object->label)),
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
      $temp_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $form_state['values']['pid']);
    }
  }
  if ($to_remove) {
    foreach ($to_remove as $candidate) {
      $temp_object = islandora_object_load($candidate);
      $temp_object->relationships->remove(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $form_state['values']['pid']);
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
 * @param String $critical_edition_pid
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
 * Displays the versionable object forms in a set of vertical tabs.
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
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/manage_rdf_tei_attachment.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/transcriptions');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/versionable_object_creation.form');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $members = islandora_critical_edition_get_members($object->id);
  $cwrcwriter = array_search('islandora:criticalEditionCModel', $members);
  $transcriptions = array_keys($members, 'islandora:transcriptionCModel');

  $output = array();
  $output['manage_critical_edition'] = array('#type' => 'vertical_tabs');
  $output['manage_critical_edition']['create_vo'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Create new Versionable Object'),
    '#description' => t('Create Versionable object and associate it with this Critical Edition'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_create_versionable_object', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['manage_critical_edition']['attach_vo'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Associate existing Versionable Object'),
    '#description' => t('Associate existing Versionable Object with this Critical Edition'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_attach_versionable', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['manage_critical_edition']['attach_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Associate TEI-RDF Object'),
    '#description' => t('Associate existing TEI-RDF Object from repository'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_associate_tei_rdf_form', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['manage_critical_edition']['create_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Create TEI-RDF Object from existing object'),
    '#description' => t('Create new RDF-TEI object as a full or partial subset on an existing object.'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_associate_tei_rdf_form', $object, 'islandora:bookCModel'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  if ($cwrcwriter) {
    $output['manage_critical_edition']['add_transcription'] = array(
      '#access' => user_access(ISLANDORA_ADD_DS),
      '#title' => t('Create Transcription'),
      '#type' => 'fieldset',
      'form' => drupal_get_form('islandora_critical_edition_create_transcription_form', $object),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
  }
  if ($transcriptions) {
    $output['manage_critical_edition']['manage_transcription'] = array(
      '#access' => user_access(ISLANDORA_ADD_DS) && $transcriptions,
      '#title' => t('Manage Transcriptions'),
      '#type' => 'fieldset',
      'form' => drupal_get_form('islandora_critical_edition_manage_transcriptions_form', $object),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
  }

  return $output;
}
