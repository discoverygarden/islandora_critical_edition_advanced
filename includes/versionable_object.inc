<?php

/**
 * @file
 * Files to manage versionable objects.
 */

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
function islandora_versionable_object_management_menu(AbstractObject $object) {
  module_load_include('inc', 'islandora', 'includes/breadcrumb');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/manage_rdf_tei_attachment.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/transcriptions');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/update_datastreams.form');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $primary_object_types = array(
    'islandora:criticalEditionCModel',
    'islandora:sp-audioCModel',
  );
  $members = islandora_critical_edition_get_members($object->id);
  $primary_objects = array_intersect($primary_object_types, array_values($members));
  $tei_rdf = array_search('islandora:criticalEditionCModel', $members);
  $transcriptions = array_keys($members, 'islandora:transcriptionCModel');

  $output = array();
  $output['management_tabs'] = array('#type' => 'vertical_tabs');
  $output['management_tabs']['create_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && empty($primary_objects),
    '#title' => t('Create TEI-RDF Object'),
    '#description' => t('Create TEI-RDF Object from existing book object'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_associate_book_form', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['management_tabs']['attach_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && empty($primary_objects),
    '#title' => t('Associate TEI-RDF Object'),
    '#description' => t('Associate existing TEI-RDF Object from repository'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_associate_tei_rdf_form', $object, 'islandora:criticalEditionCModel'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['management_tabs']['attach_partial_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && empty($primary_objects),
    '#title' => t('Create TEI-RDF Object from existing object'),
    '#description' => t('Create new RDF-TEI object as a full or partial subset on an existing object.'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_associate_tei_rdf_form', $object, 'islandora:criticalEditionCModel'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $output['management_tabs']['add_transcription'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS),
    '#title' => t('Create Transcription'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_create_transcription_form', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['management_tabs']['manage_transcription'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && $transcriptions,
    '#title' => t('Manage Transcriptions'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_manage_transcriptions_form', $object),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $output['management_tabs']['delete_tei_rdf'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && $tei_rdf,
    '#title' => t('Disassociate TEI-RDF Object'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_delete_tei_rdf_form', $object, $tei_rdf),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $output['management_tabs']['update_datastreams'] = array(
    '#access' => user_access(ISLANDORA_ADD_DS) && $tei_rdf,
    '#title' => t('Update Consolidated TEI'),
    '#type' => 'fieldset',
    'form' => drupal_get_form('islandora_critical_edition_advanced_update_datastream_form', $object, $tei_rdf),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  return $output;
}
