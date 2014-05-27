<?php

/**
 * @file
 * Get cleaned text from TEI-RDF object.
 */

/**
 * Defines the update OCR form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $tei_rdf
 *   The PID of the TEI-RDF object.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_update_ocr_form(array $form, array $form_state, $tei_rdf) {
  $form_state['tei_rdf'] = $tei_rdf;
  return array(
    'intro' => array(
      '#type' => 'markup',
      '#prefix' => "<div><p>",
      '#suffix' => "</p></div>",
      '#markup' => t("Text will be extracted from TEI-RDF object and persisted back to the OCR datastream of the source object"),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Update'),
    ),
  );
}

/**
 * Extract text from edited pages and persit that data back to page level OCR.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_update_ocr_form_submit(array $form, array $form_state) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $tei_rdf = islandora_object_load($form_state['tei_rdf']);
  $pages = islandora_paged_content_get_pages($tei_rdf);
  $batch = array(
    'title' => t('Extracting text from TEI-RDF Page Objects'),
    'file' => drupal_get_path('module', 'islandora_critical_edition_advanced') . '/includes/update_ocr.form.inc',
    'progress_message' => t('@current of @total pages harvested.', array('@total' => count($pages))),
    'finished' => 'islandora_critical_edition_advanced_update_ocr_batch_finished',
    'operations' => array(),
  );
  foreach (array_keys($pages) as $page) {
    $batch['operations'][] = array(
      'islandora_critical_edition_advanced_update_ocr_batch_operation',
      array($page),
    );
  }
  batch_set($batch);
}

/**
 * Update OCR streams.
 *
 * @param string $page
 *   The page object's PID.
 */
function islandora_critical_edition_advanced_update_ocr_batch_operation($page) {
  $page = islandora_object_load($page);
  $rels = $page->relationships->get(ISLANDORA_RELS_EXT_URI, 'isEditionOf');
  $reference_object = islandora_object_load($rels[0]['object']['value']);
  $cwrc = $page['CWRC']->content;
  $sxml = simplexml_load_string($cwrc);
  $content = $sxml->text->body;
  if ($content) {
    $body = $content->asXML();
    $body = preg_replace("/<[lb|ab].*?\/>/", "\n", $body);
    $body = preg_replace("/<\/line>/", "\n", $body);
    $body = preg_replace("/<\/ab>/", "\n", $body);
    $body = preg_replace("/<\/p>/", "\n", $body);
    $reference_object['OCR']->content = strip_tags($body);
  }
}

/**
 * Report success.
 *
 * @param clear $success
 *   Success of batch job
 * @param array $results
 *   Drupal batch results
 * @param array $operations
 *   Drupal batch operations
 */
function islandora_critical_edition_advanced_update_ocr_batch_finished($success, array $results, array $operations) {
  if ($success) {
    drupal_set_message(t('Succesfully Updated OCR.'));
  }
  else {
    drupal_set_message(t('Failed to Updated OCR.'), 'error');
  }
}
