<?php

/**
 * @file
 * Handles the association of an Audio object.
 */

/**
 * Form to associate existing Audio object.
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
function islandora_critical_edition_associate_media_form($form, $form_state, $object, $content_model = 'islandora:sp-audioCModel') {
  $object_models = $object->models;
  $system = array_search("fedora-system:FedoraObject-3.0", $object_models);
  unset($object_models[$system]);
  $object_model = array_shift($object_models);
  $offset = islandora_critical_edition_prep_form($object, $content_model);
  $name_mappings = array(
    'islandora:criticalEditionContainerCModel' => t('Critical Edition'),
    'islandora:versionableObjectCModel' => t('Versionable Object'),
  );
  $type_mappings = array(
    'islandora:sp-audioCModel' => t('Audio'),
    'islandora:sp_videoCModel' => t('Video'),
  );
  $audios = islandora_critical_edition_get_related_objects($content_model, $offset);
  $rows = array();
  foreach ($audios as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  $header = array(
    'title' => array('data' => t('@type Object', array('@type' => $type_mappings[$content_model]))),
    'pid' => array('data' => t('PID')),
  );
  // Build and return table element.
  $element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t("There are no @type Objects in this Fedora Repository.", array('@type' => $type_mappings[$content_model])),
    '#js' => FALSE,
    '#multiple' => FALSE,
  );

  $form['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array('quantity', count($rows))),
  );
  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => t("Choose RDF-TEI object to associate with this @label.", array('@label' => $name_mappings[$object_model])),
  );
  $form['audio_objects'] = $element;
  $form['vo_title'] = array(
    '#type' => 'textfield',
    '#title' => t("Title for new Versionable Object"),
    '#required' => TRUE,
  );
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Associate TEI-RDF Object'),
  );
  $form['container_object_pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );
  $form['container_object_model'] = array(
    '#type' => 'hidden',
    '#value' => $object_model,
  );
  return $form;
  $form = array();
}

/**
 * Form Submission handler.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal Form State
 */
function islandora_critical_edition_associate_audio_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/versionable_object_creation.form');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $type = $form_state['values']['container_object_model'];

  if ($type == 'islandora:criticalEditionContainerCModel') {
    $audio = $form_state['values']['audio_objects'];
    $media_object = islandora_object_load($audio);
    $container_object = islandora_object_load($form_state['values']['container_object_pid']);
    $tn = $media_object['TN'];
    $label = $form_state['values']['vo_title'];
    $versionable_object = islandora_critical_edition_build_vo($container_object, $label, $tn);
    $tei_rdf = make_audio_tei_rdf($versionable_object, $media_object);
    $transcription = make_media_transcription($versionable_object, $form_state['values']['audio_objects']);
  }
  else {
    $versionable_object_pid = $form_state['values']['container_object_pid'];
  }
  $media_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
}

/**
 * Builds TEI_RDF from media resource.
 *
 * @param AbstractObject $versionable_object
 *   Object representing Versionable Object
 * @param AbstractObject $media_object
 *   Object representing audio or video object
 */
function make_audio_tei_rdf($versionable_object, $media_object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/create');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $namespace = islandora_get_namespace($media_object->id);
  $new_tei_rdf_object = islandora_critical_edition_create_object($namespace, $media_object->label);
  $new_tei_rdf_object->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModel');
  $new_tei_rdf_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
  $new_tei_rdf_object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_CRITICAL_EDITION_IS_CRITICAL_EDITION_OF, $versionable_object->id, RELS_TYPE_PLAIN_LITERAL);
  $tei_rdf_object = islandora_add_object($new_tei_rdf_object);
  $new_page_object = islandora_critical_edition_create_object($namespace, "Audio Transcription");
  $new_page_object->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModelPage');
  $new_page_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $tei_rdf_object->id);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageOf', $tei_rdf_object->id);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageNumber', 1, RELS_TYPE_PLAIN_LITERAL);
  $new_page_object->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSequenceNumber', 1, RELS_TYPE_PLAIN_LITERAL);
  $page_object = islandora_add_object($new_page_object);
  if ($media_object['TN']) {
    $page_object->ingestDatastream($media_object['TN']);
  }
  $ocr_datastream = $new_page_object->constructDatastream("OCR", 'M');
  $ocr_datastream->mimetype = 'text/plain';
  $ocr_datastream->label = "Default Transcription";
  $ocr_datastream->setContentFromString = "Default transcription";
  $page_object->ingestDatastream($ocr_datastream);
  islandora_critical_edition_add_cwrc_datastream($page_object);
}

/**
 * Builds transcription object for media object.
 *
 * @param AbstractObject $versionable_object
 *   Object representing Versionable Object
 * @param String $media_object_pid
 *   Pid of media object
 */
function make_media_transcription($versionable_object, $media_object_pid) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  $media_object = islandora_object_load($media_object_pid);
  $label = t('Transcription of') . " " . $media_object->label;
  $transcription_object = islandora_critical_edition_advanced_build_transcription($versionable_object, $label);
  $transcription_datastream = $transcription_object->constructDatastream("TRANSCRIPTION", 'M');
  $transcription_datastream->mimetype = 'text/plain';
  $transcription_datastream->label = "Default Transcription";
  $transcription_datastream->setContentFromString = "Default transcription";
  $transcription_object->ingestDatastream($transcription_datastream);
  islandora_calliope_write_file($transcription_object->id);
}
