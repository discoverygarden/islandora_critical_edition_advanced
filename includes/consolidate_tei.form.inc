<?php

/**
 * @file
 * Consolidate TEI-RDF Page objects into a TEI Datastream on the TEI-RDF object.
 */

/**
 * Defines the consolidate TEI form.
 *
 * Consolidate TEI-RDF Page objects into a TEI Datastream on the TEI-RDF object.
 *
 * Be sure you pass in a TEI-RDF object (islandora:criticalEditionCModel) as
 * no checking is done.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param strin $tei_rdf
 *   The PID of the TEI-RDF object.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_consolidate_tei_form(array $form, array $form_state, $tei_rdf) {
  $form_state['tei_rdf'] = $tei_rdf;
  return array(
    'intro' => array(
      '#type' => 'markup',
      '#prefix' => "<div><p>",
      '#suffix' => "</p></div>",
      '#markup' => t("This will consolidate the page level TEI-RDF from this TEI-RDF object into a single TEI-XML document."),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Update'),
    ),
  );
}

/**
 * Consolidate TEI-RDF Page objects into a TEI Datastream on the TEI-RDF object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_consolidate_tei_form_submit(array $form, array $form_state) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $tei_rdf = islandora_object_load($form_state['tei_rdf']);
  $pages = islandora_paged_content_get_pages($tei_rdf);
  $batch = array(
    'title' => t('Consolidating TEI from TEI-RDF Page Objects'),
    'file' => drupal_get_path('module', 'islandora_critical_edition_advanced') . '/includes/consolidate_tei.form.inc',
    'progress_message' => t('@current of @total pages consolidated.', array('@total' => count($pages))),
    'finished' => 'islandora_critical_edition_advanced_consolidate_tei_batch_finished',
    'operations' => array(),
  );
  foreach (array_keys($pages) as $page) {
    $batch['operations'][] = array(
      'islandora_critical_edition_advanced_consolidate_tei_batch_operation',
      array($tei_rdf->id, $page),
    );
  }
  $batch['operations'][] = array('islandora_critical_edition_advanced_update_tei_datastream_batch_operation', array($tei_rdf));
  batch_set($batch);
}

/**
 * Append the given page to the TEI document.
 *
 * @param string $page
 *   The page object's PID.
 * @param array $context
 *   The Drupal Batch context.
 */
function islandora_critical_edition_advanced_consolidate_tei_batch_operation($tei_rdf, $page, array &$context) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  if (!isset($context['results']['tei'])) {
    $context['results']['tei'] = islandora_critical_edition_advanced_get_blank_tei($tei_rdf->label);
  }
  $page = islandora_object_load($page);
  if (isset($page['CWRC'])) {
    $context['results']['tei'] = islandora_critical_edition_advanced_append_tei_rdf_text_to_tei($context['results']['tei'], $page['CWRC']->content);
  }
}

/**
 * Update the TEI datastream on the TEI-RDF object.
 *
 * @param string $tei_rdf
 *   The TEI-RDF object's PID.
 * @param array $context
 *   The Drupal Batch context.
 */
function islandora_critical_edition_advanced_update_tei_datastream_batch_operation($tei_rdf, array &$context) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $tei_rdf = islandora_object_load($tei_rdf);
  if (!isset($tei_rdf['TEI'])) {
    $datastream = $tei_rdf->constructDatastream('TEI', 'M');
    $datastream->label = 'Consolidated TEI';
    $datastream->mimetype = 'application/tei+xml';
    $datastream->setContentFromString($context['results']['tei']);
    $tei_rdf->ingestDatastream($datastream);
  }
  else {
    $datastream = $tei_rdf['TEI'];
    $datastream->label = 'Consolidated TEI';
    $datastream->mimetype = 'application/tei+xml';
    $datastream->setContentFromString($context['results']['tei']);
  }
}

/**
 * Report on the success/failure of the consolidate batch process.
 *
 * Also update the datastream with the results.
 */
function islandora_critical_edition_advanced_consolidate_tei_batch_finished($success, array $results, array $operations) {
  if ($success) {
    drupal_set_message(t('Succesfully Consolidated TEI.'));
  }
  else {
    drupal_set_message(t('Failed to Consolidate TEI.'), 'error');
  }
}
