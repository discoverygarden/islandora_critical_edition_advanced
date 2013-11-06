<?php

/**
 * @file
 * Files to update consolidated datastreams.
 */

/**
 * The form for associating a TEI-RDF object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */

/**
 * The form for associating a TEI-RDF object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   Object representing the Versionable Object
 * @param String $tei_rdf
 *   PID of the associated TEI_RDF object
 *
 * @return array
 *   Drupal form data
 */
function islandora_critical_edition_advanced_update_datastream_form($form, $form_state, $object, $tei_rdf) {
  $form['intro'] = array(
    '#type' => 'markup',
    '#prefix' => "<div><p>",
    '#suffix' => "</p></div>",
    '#markup' => t("This will consolidate the page level TEI from this TEI-RDF object into a single xml."),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $tei_rdf,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );
  return $form;
}

/**
 * Submit handler.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal From State
 */
function islandora_critical_edition_advanced_update_datastream_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/consolidate_tei');
  $object = islandora_object_load($form_state['values']['pid']);
  if ($object) {
    islandora_critical_edition_consolidate_tei($object);
  }
  drupal_set_message(t('TEI stream updated'));
}
