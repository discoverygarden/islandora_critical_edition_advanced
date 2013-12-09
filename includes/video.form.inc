<?php

/**
 * @file
 * Handles the association of an Video object.
 */

/**
 * Form to associate existing Video object.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form_state
 * @param AbstractObject $object
 *   Object representing the Versionable Object
 *
 * @return array
 *   Drupal Form
 */
function islandora_critical_edition_associate_video_form($form, $form_state, $object, $content_model = 'islandora:sp_videoCModel') {
  return islandora_critical_edition_associate_tei_rdf_base_form($form, $form_state, $object, $content_model);
}

/**
 * Form Submission handler.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal Form State
 */
function islandora_critical_edition_associate_video_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/versionable_object_creation.form');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $type = $form_state['values']['container_object_model'];

  if ($type == 'islandora:criticalEditionContainerCModel') {
    $video = $form_state['values']['table'];
    $video_object = islandora_object_load($video);
    $container_object = islandora_object_load($form_state['values']['container_object_pid']);
    $tn = $video_object['TN'];
    $label = $form_state['values']['vo_title'];
    $versionable_object = islandora_critical_edition_build_vo($container_object, $label, $tn);
    $tei_rdf = make_video_tei_rdf($versionable_object, $video_object);
    $transcription = make_video_transcription($versionable_object, $form_state['values']['table']);
  }
  else {
    $versionable_object_pid = $form_state['values']['container_object_pid'];
  }
  $video_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
}

/**
 * Unsure of what this does.
 * @todo Document
 */
function make_video_tei_rdf($versionable_object, $video_object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/create');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $namespace = islandora_get_namespace($video_object->id);
  $new_tei_rdf_object = islandora_critical_edition_create_object($namespace, $video_object->label);
  $new_tei_rdf_object->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModel');
  $new_tei_rdf_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
  $new_tei_rdf_object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_CRITICAL_EDITION_IS_CRITICAL_EDITION_OF, $video_object->id, RELS_TYPE_PLAIN_LITERAL);
  $tei_rdf_object = islandora_add_object($new_tei_rdf_object);

  $new_page_object = islandora_critical_edition_create_object($namespace, "Audio Transcription");

  // Add all relationships.
  $new_page_object->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModelPage');
  $new_page_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $tei_rdf_object->id);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageOf', $tei_rdf_object->id);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageNumber', 1, RELS_TYPE_PLAIN_LITERAL);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSequenceNumber', 1, RELS_TYPE_PLAIN_LITERAL);
  // Create object.
  $page_object = islandora_add_object($new_page_object);

  // Add a minimal set of datastreams from original page object.
  if ($video_object['TN']) {
    $page_object->ingestDatastream($video_object['TN']);
  }
  $ocr_datastream = $new_page_object->constructDatastream("OCR", 'M');
  $ocr_datastream->mimetype = 'text/plain';
  $ocr_datastream->label = "Default Transcription";
  $ocr_datastream->setContentFromString(t("Transcription"));
  $page_object->ingestDatastream($ocr_datastream);
  islandora_critical_edition_add_cwrc_datastream($page_object);
}

/**
 * Creates default blank transcription for new Versionable Object.
 *
 * @param AbstactObject $versionable_object
 *   Object representing versionable object
 * @param String $vudio_object_pid
 *   PID of ausio object being transcribed
 */
function make_video_transcription($versionable_object, $video_object_pid) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  $video_object = islandora_object_load($video_object_pid);
  $label = $video_object->label;
  $transcription_object = islandora_critical_edition_advanced_build_transcription($versionable_object, $label);
  $transcription_datastream = $transcription_object->constructDatastream("TRANSCRIPTION", 'M');
  $transcription_datastream->mimetype = 'text/plain';
  $transcription_datastream->label = "Default Transcription";
  $transcription_datastream->setContentFromString = "Default transcription";
  $transcription_object->ingestDatastream($transcription_datastream);
  islandora_calliope_write_file($transcription_object->id);
}
