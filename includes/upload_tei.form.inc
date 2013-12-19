<?php

/**
 * @file
 * Upload and create / replace the TEI-RDF objects TEI Datastream.
 */

/**
 * Defines a file upload form for uploading the TEI Datastream.
 *
 * No Checking is done to confirm the given object is a TEI-RDF object.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 * @param string $tei_rdf
 *   The PID of the TEI-RDF object to update.
 *
 * @return array
 *   The drupal form definition.
 */
function islandora_critical_edition_advanced_upload_tei_form(array $form, array &$form_state, $tei_rdf) {
  $form_state['tei_rdf'] = $tei_rdf;
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('xml');
  return array(
    'tei_file' => array(
      '#title' => t('Upload TEI XML'),
      '#type' => 'managed_file',
      '#required' => TRUE,
      '#description' => t('Select TEI XML document to upload.'),
      '#upload_location' => 'temporary://',
      '#upload_validators' => array(
        'file_validate_extensions' => $extensions,
        // Assume its specified in MB.
        'file_validate_size' => array($upload_size * 1024 * 1024),
      ),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Upload'),
    ),
  );
}

/**
 * Submit handler, adds uploaded file to ingest object.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function islandora_critical_edition_advanced_upload_tei_form_submit(array $form, array &$form_state) {
  $file = file_load($form_state['values']['tei_file']);
  $path = drupal_realpath($file->uri);
  $object = islandora_object_load($form_state['tei_rdf']);
  if (empty($object['TEI'])) {
    $datastream = $object->constructDatastream('TEI', 'M');
    $datastream->setContentFromFile($path);
    $datastream->label = 'Consolidated TEI';
    $datastream->mimetype = 'application/tei+xml';
    $object->ingestDatastream($datastream);
  }
  else {
    $datastream = $object['TEI'];
    $datastream->setContentFromFile($path);
    $datastream->label = 'Consolidated TEI';
    $datastream->mimetype = 'application/tei+xml';
  }
}
