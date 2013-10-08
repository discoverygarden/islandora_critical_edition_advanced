<?php

/**
 * @file
 * Handles the management of transcription objects.
 */

/**
 * The form for creating a transcription object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_create_transcription_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('txt');
  $form = array();
  $input_types = array();
  $form['transcription'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of new transcription'),
    '#size' => 40,
    '#default_value' => t('Transcription of') . ' ' . $object->label,
  );

  $members = islandora_critical_edition_get_members($object->id);
  $critical_edition = array_search('islandora:criticalEditionCModel', $members);
  if ($critical_edition) {
    $input_types['cwrc'] = t('Create transcription from TEI-RDF Object');
    $form['cwrcwriter'] = array(
      '#type' => 'hidden',
      '#value' => $critical_edition,
    );
  }
  $input_types['none'] = t('Create born-digital transcription');
  $input_types['file'] = t('Upload transcription from file');

  $form['versionable_object_pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $form['content_source'] = array(
    '#type' => 'radios',
    '#options' => $input_types,
    '#default_value' => 'none',
  );

  $form['file'] = array(
    '#title' => t('Upload Optional Transcription.'),
    '#type' => 'managed_file',
    '#description' => t('Select file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b>', array('@size' => $upload_size, '@ext' => $extensions[0])),
    '#default_value' => isset($form_state['values']['files']) ? $form_state['values']['files'] : NULL,
    '#upload_location' => 'temporary://',
    '#upload_validators' => array(
      'file_validate_extensions' => $extensions,
      'file_validate_size' => array($upload_size * 1024 * 1024),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="content_source"]' => array('value' => 'file'),
      ),
    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create Transcription'),
  );
  return $form;
}

/**
 * Submit handler for Transcription creation.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form_state
 */
function islandora_critical_edition_create_transcription_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $versionable_object_pid = $form_state['values']['versionable_object_pid'];
  $versionable_object = islandora_object_load($versionable_object_pid);
  $label = $form_state['values']['transcription'];
  $content_source = $form_state['values']['content_source'];

  $tuque = new IslandoraTuque();
  $namespace = islandora_get_namespace($versionable_object->id);
  $transcription = $tuque->repository->constructObject($namespace);
  $transcription->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
  $transcription->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:transcriptionCModel');
  $transcription->label = $label;
  $transcription->owner = $versionable_object->owner;
  $datastream_id = "TRANSCRIPTION";
  $control_group = "M";
  $transcription_datastream = $transcription->constructDatastream($datastream_id, $control_group);
  $transcription_datastream->mimetype = 'text/plain';

  if ($content_source == 'cwrc') {
    $cwrcwriter = $form_state['values']['cwrcwriter'];
    add_cwrc_derived_transcription($cwrcwriter, $versionable_object_pid, $transcription, $transcription_datastream);
  }

  if ($content_source == 'file') {
    $file = file_load($form_state['values']['file']);
    $path = drupal_realpath($file->uri);
    $transcription_datastream->setContentFromFile($path);
    file_delete($file);
  }

  if ($content_source == 'none') {
    $transcription_datastream->setContentFromString(t('No transcription entered.'));
  }

  $transcription->ingestDatastream($transcription_datastream);
  $tuque->repository->ingestObject($transcription);
}

/**
 * The form for creating a transcription object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_manage_transcriptions_form(array $form, array &$form_state, AbstractObject $object) {
  $form = array();
  $rows = array();
  $versionable_object_pid = $object->id;
  $transcriptions = islandora_critical_get_related_transcriptions($versionable_object_pid);
  $members = islandora_critical_edition_get_members($object->id);
  $cwrcwriter = array_search('islandora:criticalEditionCModel', $members);
  foreach ($transcriptions as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  $header = array(
    'title' => array('data' => t('Transcription Title')),
    'pid' => array('data' => t('PID')),
  );
  // Build and return table element.
  $book_element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t("This versionable object has not transcription objects."),
    '#js' => FALSE,
    '#multiple' => FALSE,
  );
  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => t('Choose transcription.'),
  );
  $form['transcription'] = $book_element;
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete Transcription'),
    '#attributes' => array('source' => 'delete_transcription', 'class' => array('transcription_submit')),
  );
  $form['edit'] = array(
    '#type' => 'submit',
    '#value' => t('Edit Transcription'),
    '#attributes' => array('source' => 'edit_transcription', 'class' => array('transcription_submit')),
  );
  if ($cwrcwriter) {
    $form['cwrc_update'] = array(
      '#type' => 'submit',
      '#value' => t('Synchronize with TEI-RDF object'),
      '#attributes' => array('source' => 'cwrc_update', 'class' => array('transcription_submit')),
    );

    $form['cwrcwriter'] = array(
      '#type' => 'hidden',
      '#value' => $cwrcwriter,
    );
  }


  return $form;
}

/**
 * Submission handler handler for transcription management form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 */
function islandora_critical_edition_manage_transcriptions_form_submit(array $form, array &$form_state) {
  module_load_include('module', 'islandora');
  $action = $form_state['triggering_element']['#attributes']['source'];
  if (isset($form_state['values']['transcription'])) {
    $transcription_pid = $form_state['values']['transcription'];
  }
  else {
    return;
  }

  switch ($action) {
    case 'delete_transcription':
      islandora_delete_object(islandora_object_load($transcription_pid));
      break;

    case 'edit_transcription':
      drupal_goto("islandora/transcription/edit/$transcription_pid");
      break;

    case 'cwrc_update':
      $cwrcwriter = $form_state['values']['cwrcwriter'];
      islandora_update_from_tei_rdf_object($cwrcwriter, $transcription_pid);
      break;
  }
}

/**
 * Form to update transcription.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form_state
 * @param abstractObject $transcription
 *   transcription object
 */
function islandora_critical_transcription_edit_form($form, $form_state, $transcription) {
  module_load_include('inc', 'islandora', 'includes/breadcrumb');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($transcription));
  drupal_set_title($transcription->label);
  $form = array();
  $form['transcription'] = array(
    '#title' => t('Current Transcription'),
    '#type' => 'textarea',
    '#default_value' => $transcription['TRANSCRIPTION']->content,
    '#rows' => 40,
    '#cols' => 20,
    '#attributes' => array('class' => array('transcription_window')),
  );
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $transcription->id,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Transcription'),
  );
  return $form;
}

/**
 * Submit handler for transcription edit form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form
 */
function islandora_critical_transcription_edit_form_submit($form, $form_state) {
  $transcription_object = islandora_object_load($form_state['values']['pid']);
  try {
    $success = $transcription_object['TRANSCRIPTION']->content = $form_state['values']['transcription'];
  }
  catch (Exception $exc) {
    watchdog("islandora critical edition", $exc->getTraceAsString());
  }
  if ($success) {
    drupal_set_message(t("Transcription updated."));
  }
}

/**
 * Adds transcription from CWRC datastreams of CWRCWriter page objects.
 *
 * @param String $cwrcwriter
 *   PID of Critical Edition
 * @param String $versionable_object_pid
 *   PID of Versionable Object
 * @param AbstractObject $transcription
 *   Object representing Transcription Object
 * @param IslandoraFedoraDatastream $transcription_datastream
 *   Datastream object to be updated
 */
function add_cwrc_derived_transcription($cwrcwriter, $versionable_object_pid, $transcription, $transcription_datastream) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/utilities');
  $critical_edition_object = islandora_object_load($cwrcwriter);
  $pages = array_keys(islandora_paged_content_get_pages($critical_edition_object));
  $batch = array(
    'title' => t('Creating Transcription from CWRCWRiter'),
    'operations' => array(),
    'file' => drupal_get_path('module', 'islandora_critical_edition') . '/includes/transcriptions.inc',
    'progress_message' => t('@current of @total pages processed.'),
  );
  $flat_pid = islandora_escape_pid_for_function($versionable_object_pid);

  $filename = file_create_filename($flat_pid . "tempcwrc", 'temporary://');
  foreach ($pages as $page) {
    $batch['operations'][] = array('islandora_critical_edition_build_text_file', array(
        'page_object' => islandora_object_load($page),
        'filename' => $filename));
  }
  $batch['operations'][] = array('islandora_transcription_file_add',
    array(
      'transcription' => $transcription,
      'transcription_datastream' => $transcription_datastream,
      'filename' => $filename));
  batch_set($batch);
  batch_process();
}

/**
 * Builds temp file with CWRC data from page level objects.
 *
 * @param AbstractObject $page_object
 *   Object representing page
 * @param String $filename
 *   Temp file to hold data
 */
function islandora_critical_edition_build_text_file($page_object, $filename) {
  if ($page_object['CWRC']) {
    $raw_cwrc = $page_object['CWRC']->content;
    $cwrc = str_replace('<br />', "\n", $raw_cwrc);
    $cleaned_text = strip_tags($cwrc);
    file_put_contents($filename, $cleaned_text, FILE_APPEND | LOCK_EX);
  }
}

/**
 * Adds completed file to Transcription Object.
 *
 * @param AbstractObject $transcription
 *   Uningested object representing transcription
 * @param IslandoraFedoraDatastream $transcription_datastream
 *   Transcription datastream
 * @param String $filename
 *   file to be added
 */
function islandora_transcription_file_add($transcription, $transcription_datastream, $filename) {
  $tuque = new IslandoraTuque();
  $transcription_datastream->setContentFromFile($filename);
  unlink($filename);
  $transcription->ingestDatastream($transcription_datastream);
  $tuque->repository->ingestObject($transcription);
}

/**
 * Populates transcription with current text from TEI-RDF object.
 *
 * @param String $tei_rdf_object_pid
 *   PID of RDF-TEI Object
 * @param String $transcription_pid
 *   PID of Transcription Object being updated
 */
function islandora_update_from_tei_rdf_object($tei_rdf_object_pid, $transcription_pid) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/utilities');
  $tei_rdf_object = islandora_object_load($tei_rdf_object_pid);
  $transcription = islandora_object_load($transcription_pid);
  $pages = array_keys(islandora_paged_content_get_pages($tei_rdf_object));
  $batch = array(
    'title' => t('Creating Transcription from TEI-RDF Object'),
    'operations' => array(),
    'file' => drupal_get_path('module', 'islandora_critical_edition') . '/includes/transcriptions.inc',
    'progress_message' => t('@current of @total pages processed.'),
  );

  $flat_pid = islandora_escape_pid_for_function($transcription_pid);
  $filename = file_create_filename($flat_pid . "tempcwrc", 'temporary://');
  foreach ($pages as $page) {
    $batch['operations'][] = array('islandora_critical_edition_build_text_file', array(
        'page_object' => islandora_object_load($page),
        'filename' => $filename));
  }
  $batch['operations'][] = array('islandora_transcription_file_update',
    array(
      'transcription' => $transcription,
      'filename' => $filename));
  batch_set($batch);
  batch_process();
}

/**
 * Updates completed file to Transcription Object.
 *
 * @param AbstractObject $transcription
 *   Transcription Object
 * @param String $filename
 *   file to be added
 */
function islandora_transcription_file_update($transcription, $filename) {
  $transcription['TRANSCRIPTION']->setContentFromFile($filename);
  unlink($filename);
}
