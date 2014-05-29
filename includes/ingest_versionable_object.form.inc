<?php

/**
 * @file
 * Handles the ingestion of versionable objects.
 */

/**
 * Defines the allowed Content-Models for Source Objects.
 *
 * @return array
 *   An associative array where the keys are the Content Model PID and the
 *   values are label's for the Content Model.
 */
function islandora_critical_edition_advanced_valid_source_types() {
  return array(
    'islandora:bookCModel' => t('Text'),
    'islandora:sp-audioCModel' => t('Audio'),
    'islandora:sp_videoCModel' => t('Video'),
  );
}

/**
 * Local menu action to present an ingest versionable object form.
 *
 * @param AbstractObject $edition
 *   The critical edition to ingest into.
 *
 * @return string
 *   The HTML representation of the ingest page form.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_page(AbstractObject $edition) {
  global $user;
  module_load_include('inc', 'islandora', 'includes/breadcrumb');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($edition));
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  $version = $edition->repository->constructObject(islandora_get_namespace($edition->id));
  $version->owner = $user->name;
  $version->label = 'New Version';
  $version->models = 'islandora:versionableObjectCModel';
  $version->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $edition);
  drupal_set_title(t('Add Versionable Object to @edition', array('@edition' => $edition->label)));
  return drupal_get_form('islandora_ingest_form', array(
    'models' => array('islandora:versionableObjectCModel'),
    'objects' => array($version)));
}

/**
 * Autocomplete menu callback for source objects.
 */
function islandora_critical_edition_advanced_versionable_object_form_source_autocomplete($type = NULL, $search = NULL) {
  $type = trim(urldecode($type));
  // List of approved source content model types.
  $models = array_keys(islandora_critical_edition_advanced_valid_source_types());
  $models = in_array($type, $models) ? array($type) : $models;
  $where_clause = '';
  foreach ($models as $model) {
    $where_clause .= !empty($where_clause) ? 'UNION ' : '';
    $where_clause .= "{ ?object fedora-model:hasModel <info:fedora/$model> . }\n";
  }
  $where_clause = substr($where_clause, 0, -1);
  $query = <<<EOT
    PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    PREFIX islandora: <http://islandora.ca/ontology/relsext#>
      SELECT DISTINCT ?object ?label FROM <#ri> WHERE {
        $where_clause
        OPTIONAL { ?object fedora-model:label  ?label. }
        FILTER (regex(?label, "{$search}", "i")) .
      }
EOT;
  $tuque = islandora_get_tuque_connection();
  // @todo If time permits swap this out for a more flexable solr serach.
  $results = $tuque->repository->ri->sparqlQuery($query);
  $output = array();
  foreach ($results as $result) {
    $key = $result['object']['value'];
    $output[$key] = $result['label']['value'] . " ($key)";
  }
  drupal_json_output($output);
}

/**
 * Defines the ingest Versionable Object form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_versionable_object_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $values = &$form_state['values'];
  $source = isset($values['source']) ? islandora_object_load($values['source']) : NULL;
  // Build the list rows for the TEI-RDF tableselect, and count the number.
  $page_count = 0;
  $tei_rdf_objects = array();
  if ($source) {
    // Not efficent we should create a seperate function in
    // islandora_paged_content to compute the number of pages.
    $page_count = count(islandora_paged_content_get_pages($source));
    foreach (islandora_critical_edition_get_critical_editions($source) as $pid => $label) {
      $tei_rdf_objects[$pid] = array($pid, $label);
    }
  }
  $tei_rdf_source = isset($values['tei_rdf_source']) ? $values['tei_rdf_source'] : 'new';
  $transcription_source = isset($values['transcription_source']) ? $values['transcription_source'] : 'tei-rdf';
  // Can only select page ranges for books, and if the islandora_paged_content
  // module is enabled.
  $can_limit_page_range = $source && in_array('islandora:bookCModel', $source->models);
  $limit_page_range = $can_limit_page_range && $values['limit_page_range'];
  // Upload Transcription.
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  $extensions = array('txt');
  $valid_source_selected = $source ? TRUE : FALSE;
  // Opted to use #ajax over #states in most cases as #states doesn't support
  // conditionally #required values.
  return array(
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'islandora_critical_edition_advanced') . '/js/ingest_versionable_object.js' => array(
          'type' => 'file',
          'weight' => 5,
        ),
      ),
    ),
    'source_ajax_wrapper' => array(
      '#type' => 'container',
      '#prefix' => '<div id="source-ajax-container-wrapper">',
      '#suffix' => '</div>',
      'source_type' => array(
        '#type' => 'select',
        '#title' => t('Limit Source Results to type'),
        '#options' => array('*' => t('Any')) + islandora_critical_edition_advanced_valid_source_types(),
      ),
      'source' => array(
        '#type' => 'textfield',
        '#title' => t('Select Source Object'),
        '#size' => 30,
        '#maxlength' => 60,
        '#required' => TRUE,
        '#autocomplete_path' => 'islandora/critical_edition_advanced_source/autocomplete',
        '#ajax' => array(
          'callback' => 'islandora_critical_edition_advanced_source_ajax_container',
          'wrapper' => 'source-ajax-container-wrapper',
          'method' => 'replace',
          'effect' => 'fade',
        ),
      ),
      'page_range' => array(
        '#access' => $can_limit_page_range,
        '#type' => 'fieldset',
        '#title' => t('Select page range'),
        '#description' => t('You may optionally limit the pages used in the TEI-RDF and transcription. Only valid for new TEI-RDF.'),
        '#prefix' => '<div id="page-ajax-container-wrapper">',
        '#suffix' => '</div>',
        'limit_page_range' => array(
          '#type' => 'checkbox',
          '#title' => t('Limit the page range'),
          '#default_value' => FALSE,
          '#ajax' => array(
            'callback' => 'islandora_critical_edition_advanced_page_ajax_container',
            'wrapper' => 'page-ajax-container-wrapper',
            'method' => 'replace',
            'effect' => 'fade',
          ),
        ),
        'first' => array(
          '#access' => $limit_page_range,
          '#type' => 'textfield',
          '#size' => 6,
          '#title' => t('First page'),
          '#default_value' => 1,
          '#required' => TRUE,
        ),
        'last' => array(
          '#access' => $limit_page_range,
          '#type' => 'textfield',
          '#size' => 6,
          '#title' => t('Last page'),
          '#default_value' => $page_count,
          '#required' => TRUE,
        ),
      ),
      'tei_rdf_source' => array(
        '#access' => $valid_source_selected,
        '#disabled' => empty($tei_rdf_objects),
        '#description' => check_plain(empty($tei_rdf_objects) ? t('No existing TEI-RDF found.') : NULL),
        '#type' => 'radios',
        '#title' => 'TEI-RDF',
        '#required' => TRUE,
        '#options' => array('new' => t('Create New'), 'existing' => t('Use Existing')),
        '#default_value' => 'new',
        '#ajax' => array(
          'callback' => 'islandora_critical_edition_advanced_tei_rdf_ajax_container',
          'wrapper' => 'tei-rdf-ajax-container-wrapper',
          'method' => 'replace',
          'effect' => 'fade',
        ),
      ),
      'tei_rdf_ajax_wrapper' => array(
        '#access' => $valid_source_selected,
        '#type' => 'container',
        '#prefix' => '<div id="tei-rdf-ajax-container-wrapper">',
        '#suffix' => '</div>',
        'tei_rdf' => array(
          '#access' => $tei_rdf_source == 'existing',
          '#type' => 'tableselect',
          '#multiple' => FALSE,
          '#header' => array('PID', 'Label'),
          '#options' => $tei_rdf_objects,
          '#empty' => t('No TEI-RDF objects available. Please select "Create New"'),
        ),
        'tei_rdf_label' => array(
          '#access' => $tei_rdf_source == 'new',
          '#required' => TRUE,
          '#type' => 'textfield',
          '#title' => t('TEI-RDF Label'),
        ),
      ),
      'transcription_label' => array(
        '#access' => $valid_source_selected,
        '#type' => 'textfield',
        '#required' => TRUE,
        '#title' => t('Transcription Label'),
      ),
      'transcription_source' => array(
        '#access' => $valid_source_selected,
        '#type' => 'radios',
        '#default_value' => 'tei-rdf',
        '#options' => array(
          'tei-rdf' => t('Create transcription from TEI-RDF Object'),
          'digital' => t('Create born-digital transcription'),
          'upload' => t('Upload transcription from file'),
        ),
      ),
      'transcription' => array(
        '#access' => $valid_source_selected,
        '#title' => t('Current Transcription'),
        '#type' => 'textarea',
        '#default_value' => t('Enter Transcription here.'),
        '#rows' => 40,
        '#cols' => 20,
        '#attributes' => array('class' => array('transcription_window')),
        '#states' => array(
          'visible' => array(
            ':input[name="transcription_source"]' => array('value' => 'digital'),
          ),
        ),
      ),
    ),
    // The pluploader doesn't respect #states, so we must wrap it in a
    // container.
    'transcription_upload' => array(
      '#title' => t('Upload Transcription'),
      '#type' => 'managed_file',
      '#description' => t('Select file to upload.<br/>Files must be less than <b>@size MB.</b><br/>Allowed file types: <b>@ext.</b>', array('@size' => $upload_size, '@ext' => $extensions[0])),
      '#upload_location' => 'temporary://',
      '#upload_validators' => array(
        'file_validate_extensions' => $extensions,
        'file_validate_size' => array($upload_size * 1024 * 1024),
      ),
      '#states' => array(
        'visible' => array(
          ':input[name="transcription_source"]' => array('value' => 'upload'),
        ),
      ),
    ),
  );
}

/**
 * Validate handler for the ingest Versionable Object form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_versionable_object_form_validate(array $form, array &$form_state) {
  $values = $form_state['values'];
  // Validate Source Object.
  $source = islandora_object_load($values['source']);
  // It must be a Fedora Object.
  if (!$source) {
    form_error($form['source_ajax_wrapper']['source'], t('Source Object: "@source" is not valid object.', array(
      '@source' => $values['source'])));
  }
  else {
    $valid_source_types = islandora_critical_edition_advanced_valid_source_types();
    $sources_valid_models = array_intersect(array_keys($valid_source_types), $source->models);
    // It must be of a valid source type.
    if (empty($sources_valid_models)) {
      form_error($form['source_ajax_wrapper']['source'], t('Source Object: "@source" must be one of the following types: @types.', array(
        '@source' => $values['source'],
        '@types' => implode(', ', $valid_source_types))));
    }
  }
  // Validate TEI-RDF object, if "use existing" was selected.
  if ($values['tei_rdf_source'] == 'existing') {
    // We can't limit pages on existing TEI-RDF.
    if ($values['limit_page_range']) {
      form_set_error('limit_page_range', t('You can not limit the page range on existing TEI-RDF objects, you must create a new one.'));
    }
    if (empty($values['tei_rdf'])) {
      // We don't put the form error on the tableselect as that is not supported
      // by the Drupal Form API.
      form_error($form, t('Select an existing TEI-RDF object or create a new one.'));
    }
    else {
      $tei_rdf = islandora_object_load($values['tei_rdf']);
      // Must exist and actually be an TEI-RDF object.
      if (!$tei_rdf || !in_array('islandora:criticalEditionCModel', $tei_rdf->models)) {
        // We don't put the form error on the tableselect as that is not
        // supported by the Drupal Form API.
        form_error($form, t('TEI-RDF Object: "@tei_rdf" is not valid object.', array(
          '@tei_rdf' => $values['tei_rdf'])));
      }
    }
  }
  // Validate Page Range.
  if ($values['limit_page_range']) {
    module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
    // Not efficent we should create a seperate function in
    // islandora_paged_content to compute the number of pages.
    $page_count = count(islandora_paged_content_get_pages($source));
    foreach (array('first', 'last') as $element) {
      $numeric = is_numeric($values[$element]);
      $out_of_range = $numeric ? $values[$element] < 1 || $values[$element] > $page_count : FALSE;
      if ($out_of_range) {
        $element = $form['source_ajax_wrapper']['page_range'][$element];
        form_error($element, t('@name must be a number between 1 - @max.', array('@name' => $element['#title'], '@max' => $page_count)));
      }
    }
    if ((int) $values['last'] < (int) $values['first']) {
      $last = $form['source_ajax_wrapper']['page_range']['last'];
      $first = $form['source_ajax_wrapper']['page_range']['first'];
      form_error($last, t('@last must be less than @first.', array('@last' => $last['#title'], '@first' => $first['#title'])));
    }
  }
  // Validate Upload.
  if ($values['transcription_source'] == 'upload') {
    // Handels pluploader custom behavoir.
    $file_uploaded = isset($form_state['input']['edit-transcription-upload_count']) && ((int) $form_state['input']['edit-transcription-upload_count'] !== 0);
    $file_uploaded = $file_uploaded || (file_load($values['transcription_upload']) !== FALSE);
    if (!$file_uploaded) {
      // We handle the logic for #required here, allowing us to use #states on
      // the conditional transcription form elements.
      $element = $form['transcription_upload'];
      form_error($element, t('@title field is required.', array('@title' => $element['#title'])));
    }
  }
}

/**
 * Submit form for ingest Versionable Object form.
 *
 * Creates a single Versionable Object (islandora:versionableObjectCModel) and
 * either creates or links an existing TEI-RDF Object
 * (islandora:criticalEditionCModel) with the Verisonable Object. Also generates
 * a Transcription Object (islandora:transcriptionCModel) from the TEI-RDF
 * Object and links it with the Versionable Object.
 *
 * It's assumed that the next form page will be the versionable objects
 * Metadata.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_versionable_object_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/transcriptions');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $values = $form_state['values'];
  $shared_storage = &islandora_ingest_form_get_shared_storage($form_state);
  // Store the Source PID in shared storage so it can be used in other forms
  // steps and alters.
  $shared_storage['source'] = $form_state['values']['source'];
  $source = islandora_object_load($shared_storage['source']);
  // Get the versionable object and it's critical edition.
  $versionable_object = islandora_ingest_form_get_object($form_state);
  $ingest_objects = &islandora_ingest_form_get_objects($form_state);
  $parents = $versionable_object->relationships->get(FEDORA_RELS_EXT_URI, 'isMemberOf');
  $critical_edition = $parents[0]['object']['value'];
  // Link to an existing TEI-RDF Object.
  if ($values['tei_rdf_source'] == 'existing') {
    $tei_rdf = islandora_object_load($values['tei_rdf']);
    $tei_rdf->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
  }
  // Create a new TEI-RDF object.
  else {
    $pages = islandora_paged_content_get_pages($source);
    $namespace = islandora_get_namespace($versionable_object->id);
    $tei_rdf = $versionable_object->repository->constructObject($namespace);
    $tei_rdf->models = 'islandora:criticalEditionCModel';
    $tei_rdf->label = $values['tei_rdf_label'];
    $tei_rdf->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
    // @todo We need to migrate the existing data at some point in the future
    // The following property should not be a literal value, but we must make
    // it so for backwards compatibility.
    $tei_rdf->relationships->add(ISLANDORA_RELS_EXT_URI, 'isCriticalEditionOf', $source->id, RELS_TYPE_PLAIN_LITERAL);
    $ingest_objects['tei-rdf'] = $tei_rdf;
    // TEI-RDF objects associated with books require a batch process to be
    // created, all the files created in the batch will be added to the
    // $form_state, to be ingested when islandora_ingest_form is submited.
    $pages = $values['limit_page_range'] ? array_splice($pages, $values['first'] - 1, $values['last']) : $pages;
    if (!empty($pages)) {
      $batch = array(
        'title' => t('Creating TEI-RDF Object from @source', array('@source' => $source->label)),
        'file' => drupal_get_path('module', 'islandora_critical_edition_advanced') . '/includes/ingest_versionable_object.form.inc',
        'progress_message' => t('@current of @total pages processed.', array('@total' => count($pages))),
        'finished' => 'islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_batch_finished',
        'operations' => array(),
      );
      foreach ($pages as $page) {
        $batch['operations'][] = array(
          'islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_page_batch_operation',
          array($page, $tei_rdf->id),
        );
      }
      $batch['operations'][] = array(
        'islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_tei_datastream_batch_operation',
        array(),
      );
      batch_set($batch);
    }
    // TEI-RDF objects that are not associated with books require that we have
    // at least one child islandora:criticalEditionCModelPage so that the users
    // can edit TEI with the cwrc.
    else {
      $namespace = islandora_get_namespace($tei_rdf->id);
      $page = $tei_rdf->repository->constructObject($namespace);
      $page->label = $tei_rdf->label;
      // Add all relationships.
      $page->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModelPage');
      $page->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $tei_rdf->id);
      $page->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageOf', $tei_rdf->id);
      $page->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSequenceNumber', 1, RELS_TYPE_PLAIN_LITERAL);
      $page->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageNumber', 1, RELS_TYPE_PLAIN_LITERAL);
      $page->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSection', 1, RELS_TYPE_PLAIN_LITERAL);
      // Add default datastreams.
      $datastream = $page->constructDatastream("OCR", 'M');
      $datastream->mimetype = 'text/plain';
      $datastream->label = "Default Transcription";
      $datastream->setContentFromString(t("Transcription"));
      $page->ingestDatastream($datastream);
      islandora_critical_edition_add_cwrc_datastream($page);
      $ingest_objects['page'] = $page;
    }
  }
  // Create a new Transcription Object.
  $transcription = islandora_critical_edition_advanced_construct_transcription($versionable_object, $values['transcription_label']);
  $ingest_objects['transcription'] = $transcription;
  // Populate the Transcription Object.
  switch ($values['transcription_source']) {
    case 'tei-rdf':
      $pages = ($values['tei_rdf_source'] == 'existing') ? islandora_paged_content_get_pages($tei_rdf) : islandora_paged_content_get_pages($source);
      $pages = $values['limit_page_range'] ? array_splice($pages, $values['first'] - 1, $values['last']) : $pages;
      if (!empty($pages)) {
        // If there is already a batch in place we need to append to it,
        // otherwise run from the existing object.
        $batch = array(
          'title' => t('Creating Transcription from TEI-RDF Object: @tei_rdf', array('@tei_rdf' => $tei_rdf->label)),
          'file' => drupal_get_path('module', 'islandora_critical_edition_advanced') . '/includes/ingest_versionable_object.form.inc',
          'progress_message' => t('@current of @total pages processed.', array('@total' => count($pages))),
          'finished' => 'islandora_critical_edition_advanced_ingest_versionable_object_transcription_batch_finished',
          'operations' => array(),
        );
        // If we are using a existing TEI-RDF use it's objects CWRC datastreams.
        // Otherwise generate the CWRC datastream.
        foreach (array_keys($pages) as $page) {
          $batch['operations'][] = array(
            'islandora_critical_edition_advanced_ingest_versionable_object_transcription_batch_operation',
            array($page),
          );
        }
        batch_set($batch);
      }
      else {
        $datastream = $transcription->constructDatastream('TRANSCRIPTION', 'M');
        $datastream->setContentFromString(t('Transcription'));
        $transcription->ingestDatastream($datastream);
      }
      break;

    case 'digital':
      $datastream = $transcription->constructDatastream('TRANSCRIPTION', 'M');
      $datastream->setContentFromString($values['transcription']);
      $transcription->ingestDatastream($datastream);
      break;

    case 'upload':
      $file = file_load($values['transcription_upload']);
      $datastream = $transcription->constructDatastream('TRANSCRIPTION', 'M');
      $datastream->setContentFromFile(drupal_realpath($file->uri), FALSE);
      $transcription->ingestDatastream($datastream);
      break;
  }
}

/**
 * Undoes the submit, purging any objects created by this step.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_versionable_object_form_undo_submit(array $form, array &$form_state) {
  $versionable_object = islandora_ingest_form_get_object($form_state);
  $ingest_objects = &islandora_ingest_form_get_objects($form_state);
  // Forget all generated objects except the default object.
  $ingest_objects = array(0 => $versionable_object);
}

/**
 * Ajax callback for populating form elements from the selected source object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_source_ajax_container(array $form, array &$form_state) {
  global $user;
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $values = &$form_state['values'];
  $source = isset($values['source']) ? islandora_object_load($values['source']) : NULL;
  // Repopulate the create label when selecting a new Source.
  if ($source) {
    $element = &$form['source_ajax_wrapper'];
    $element['page_range']['first']['#value'] = 1;
    $element['page_range']['last']['#value'] = count(islandora_paged_content_get_pages($source));
    $element = &$form['source_ajax_wrapper']['tei_rdf_ajax_wrapper'];
    $element['tei_rdf_label']['#value'] = $source->label;
    $element = &$form['source_ajax_wrapper'];
    $element['transcription_label']['#value'] = $source->label;
  }
  else {
    $element = &$form['source_ajax_wrapper']['tei_rdf_ajax_wrapper'];
    $element['tei_rdf_label']['#value'] = "";
    $element = &$form['source_ajax_wrapper'];
    $element['transcription_label']['#value'] = "";
  }
  return $form['source_ajax_wrapper'];
}

/**
 * Ajax callback for populating form elements when limit page is checked.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_page_ajax_container(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $values = &$form_state['values'];
  $source = isset($values['source']) ? islandora_object_load($values['source']) : NULL;
  if ($source) {
    $element = &$form['source_ajax_wrapper'];
    $element['page_range']['first']['#value'] = 1;
    $element['page_range']['last']['#value'] = count(islandora_paged_content_get_pages($source));
  }
  return $form['source_ajax_wrapper']['page_range'];
}

/**
 * Ajax callback for populating form elements when TEI-RDF source changes.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_tei_rdf_ajax_container(array $form, array &$form_state) {
  global $user;
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $values = &$form_state['values'];
  $source = isset($values['source']) ? islandora_object_load($values['source']) : NULL;
  if ($source) {
    $args = array('@source' => $source->label, '@user' => $user->name);
    $element = &$form['source_ajax_wrapper']['tei_rdf_ajax_wrapper'];
    $element['tei_rdf_label']['#value'] = t('@source: TEI-RDF (@user)', $args);
  }
  return $form['source_ajax_wrapper']['tei_rdf_ajax_wrapper'];
}

/**
 * Create editions of the given page object.
 *
 * @param array $page
 *   A page from an array create by islandora_paged_content_get_pages.
 * @param string $tei_rdf
 *   The parent TEI-RDF objects PID.
 * @param array $context
 *   The Drupal Batch context.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_page_batch_operation(array $page, $tei_rdf, array &$context) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $namespace = islandora_get_namespace($tei_rdf);
  $edition = islandora_get_tuque_connection()->repository->constructObject($namespace);
  $edition->label = $page['label'];

  // Add all relationships.
  $edition->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModelPage');
  $edition->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $tei_rdf);
  $edition->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageOf', $tei_rdf);
  $edition->relationships->add(ISLANDORA_RELS_EXT_URI, 'isEditionOf', $page['pid']);
  $edition->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSequenceNumber', $page['page'], RELS_TYPE_PLAIN_LITERAL);
  $edition->relationships->add(ISLANDORA_RELS_EXT_URI, 'isPageNumber', $page['page'], RELS_TYPE_PLAIN_LITERAL);
  $edition->relationships->add(ISLANDORA_RELS_EXT_URI, 'isSection', '1', RELS_TYPE_PLAIN_LITERAL);
  // Add a minimal set of datastreams from original page object.
  $page = islandora_object_load($page['pid']);
  $dsids = array('OCR', 'TN', 'CWRC');
  foreach ($dsids as $dsid) {
    if (isset($page[$dsid])) {
      $datastream = $edition->constructDatastream($dsid, 'M');
      $unique_id = uniqid();
      $file = file_save_data('', "temporary://${unique_id}_datastream_$DSID");
      $filename = drupal_realpath($file->uri);
      // Since the file must persist across multiple requests, we must not
      // delete it, until it has successfully been ingested.
      $page[$dsid]->getContent($filename);
      $datastream->setContentFromFile($filename, FALSE);
      $edition->ingestDatastream($datastream);
    }
  }
  // Create new CWRC datastream if there is none.
  if (!isset($edition['CWRC']) && isset($edition['OCR'])) {
    islandora_critical_edition_add_cwrc_datastream($edition);
  }
  $context['results']['pages'][] = $edition;
  $context['results']['objects'][$edition->id] = $edition;
}

/**
 * Creates the TEI datastream on the TEI-RDF object.
 *
 * @param array $context
 *   The Drupal Batch context.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_tei_datastream_batch_operation(array &$context) {
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $batch = &batch_get();
  $ingest_objects = &islandora_ingest_form_get_objects($batch['form_state']);
  $tei_rdf = &$ingest_objects['tei-rdf'];
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['total'] = count($context['results']['pages']);
    // Create the template TEI to be populated.
    $context['sandbox']['content'] = islandora_critical_edition_advanced_get_blank_tei($tei_rdf->label);
  }
  $index = $context['sandbox']['progress'];
  $page = isset($context['results']['pages'][$index]) ? $context['results']['pages'][$index] : array();
  if (isset($page['CWRC'])) {
    $context['sandbox']['content'] = islandora_critical_edition_advanced_append_tei_rdf_text_to_tei($context['sandbox']['content'], $page['CWRC']->content);
  }
  $context['sandbox']['progress']++;
  $context['message'] = t('Converting page @page of @total into a TEI ...', array(
    '@page' => $context['sandbox']['progress'],
    '@total' => $context['sandbox']['total']));
  if ($context['sandbox']['progress'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['total'];
  }
  else {
    $datastream = $tei_rdf->constructDatastream('TEI', 'M');
    $datastream->mimetype = 'application/tei+xml';
    $datastream->setContentFromString($context['sandbox']['content']);
    $datastream->label = "Consolidated TEI";
    $tei_rdf->ingestDatastream($datastream);
  }
}

/**
 * Finished call-back for the create TEI-RDF batch process.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_tei_rdf_batch_finished($success, array $results, array $operations) {
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  $batch = &batch_get();
  // We have to unset the cached files here due to a bug in core. Basically when
  // we come back from the batch function drupal_build_form() gets the
  // $form_state back from the session rather than the database, thus skipping
  // $form_get_cache which loads all of the required files. We forget the
  // required files now, so that we can dynamically load the required files
  // during drupal_rebuild_form().
  $batch['form_state']['build_info']['files'] = array();
  if ($success) {
    drupal_set_message(t('Succesfully created TEI-RDF Object.'));
    $ingest_objects = &islandora_ingest_form_get_objects($batch['form_state']);
    $ingest_objects = array_merge($ingest_objects, $results['objects']);
  }
  else {
    drupal_set_message(t('Failed to create the TEI-RDF object.'), 'error');
    // Mock the form as it's not used, then return to the form step,
    // that launched this batch process.
    $form = array();
    islandora_ingest_form_previous_submit($form, $batch['form_state']);
  }
}

/**
 * Create editions of the given page object.
 *
 * @param string $page
 *   The page object's PID.
 * @param array $context
 *   The Drupal Batch context.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_transcription_batch_operation($page, array &$context) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  // Even though this include is not needed by this function it must be included
  // here the Batch API won't be able to Find the submit handler:
  // islandora_ingest_form_next_submit() as it doesn't respect form_load_include
  // and doesn't check load any files required by submit handles.
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  if (empty($context['sandbox'])) {
    $file = file_save_data('', 'temporary://transcription.txt');
    $context['results']['filename'] = drupal_realpath($file->uri);
  }
  $page = islandora_object_load($page);
  $filename = $context['results']['filename'];
  if (isset($page['CWRC'])) {
    $text = islandora_critical_edition_convert_tei_to_plain_text($page['CWRC']->content);
  }
  elseif (isset($page['OCR'])) {
    $text = $page['OCR']->content;
  }
  file_put_contents($filename, $text, FILE_APPEND | LOCK_EX);
}

/**
 * Finished call-back for the create TEI-RDF batch process.
 */
function islandora_critical_edition_advanced_ingest_versionable_object_transcription_batch_finished($success, array $results, array $operations) {
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  $batch = &batch_get();
  // We have to unset the cached files here due to a bug in core. Basically when
  // we come back from the batch function drupal_build_form() gets the
  // $form_state back from the session rather than the database, thus skipping
  // $form_get_cache which loads all of the required files. We forget the
  // required files now, so that we can dynamically load the required files
  // during drupal_rebuild_form().
  $batch['form_state']['build_info']['files'] = array();
  if ($success) {
    $ingest_objects = &islandora_ingest_form_get_objects($batch['form_state']);
    $transcription = $ingest_objects['transcription'];
    $datastream = $transcription->constructDatastream('TRANSCRIPTION', 'M');
    $datastream->setContentFromFile($results['filename'], FALSE);
    $transcription->ingestDatastream($datastream);
    drupal_set_message(t('Succesfully created Transcription Object.'));
  }
  else {
    drupal_set_message(t('Failed to create Transcription object.'), 'error');
    // Mock the form as it's not used, then return to the form step,
    // that launched this batch process.
    $form = array();
    islandora_ingest_form_previous_submit($form, $batch['form_state']);
  }
}
