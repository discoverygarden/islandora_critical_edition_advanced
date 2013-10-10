<?php

/**
 * @file
 * Handles the association of a TEI-RDF object.
 */

/**
 * The form for associating a TEI-RDF object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_associate_book_form(array $form, array &$form_state, AbstractObject $object) {
  $content_model = 'islandora:bookCModel';
  $offset = islandora_critical_edition_prep_form($object, $content_model);
  $books = islandora_critical_edition_get_related_objects($content_model, $offset);
  $rows = array();
  foreach ($books as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  $header = array(
    'title' => array('data' => t('Book Title')),
    'pid' => array('data' => t('PID')),
  );
  // Build and return table element.
  $book_element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t("There are no book objects in this Fedora Repository."),
    '#js' => FALSE,
    '#multiple' => FALSE,
  );
  $form['pager'] = array(
    '#type' => 'markup',
    '#markup' => theme('pager', array('quantity', count($rows))),
  );
  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => t('Choose source book object.'),
  );
  $form['book_objects'] = $book_element;
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter title for TEI-RDF object'),
    '#required' => TRUE,
  );
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Create TEI-RDF Object'),
  );
  $form['versionable_object_pid'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  return $form;
  $form = array();
}

/**
 * Submit handler, creates CWRCWriter objects.
 *
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
function islandora_critical_edition_associate_book_form_submit(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/create');
  if (!isset($form_state['values']['book_objects'])) {
    return;
  }
  module_load_include('inc', 'islandora', '/includes/ingest.form');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/utilities');

  $book_pid = $form_state['values']['book_objects'];
  $book_object = islandora_object_load($book_pid);
  // Create Critical edition Fedora Object.
  $versionable_object_pid = $form_state['values']['versionable_object_pid'];
  $versionable_object = islandora_object_load($versionable_object_pid);
  $tn = $book_object['TN'];
  if ($tn) {
    if ($versionable_object['TN']) {
      $versionable_object['TN']->content = $tn->content;
    }
    else {
      $versionable_object->ingestDatastream($tn);
    }
  }
  $fedora_object = islandora_critical_edition_create_object(islandora_get_namespace($book_pid), $form_state['values']['title']);

  $fedora_object->relationships->add(ISLANDORA_RELS_EXT_URI, ISLANDORA_CRITICAL_EDITION_IS_CRITICAL_EDITION_OF, $book_pid, RELS_TYPE_PLAIN_LITERAL);
  $fedora_object->relationships->add(FEDORA_MODEL_URI, 'hasModel', 'islandora:criticalEditionCModel');
  $fedora_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object_pid);
  $edition_object = islandora_add_object($fedora_object);

  $pages = islandora_paged_content_get_pages($book_object);

  $batch = array(
    'title' => t('Creating New TEI-RDF Object'),
    'operations' => array(),
    'file' => drupal_get_path('module', 'islandora_critical_edition') . '/includes/create.inc',
    'progress_message' => t('@current of @total pages created.'),
  );

  foreach ($pages as $page) {
    $batch['operations'][] = array('islandora_create_edition_page', array(
        'page' => $page,
        'parent_book' => $edition_object->id));
  }

  batch_set($batch);
  batch_process();
}

/**
 * The form for associating a CWRCWriter object, tied to a.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_delete_tei_rdf_form(array $form, array $form_state, AbstractObject $object, $tei_rdf) {
  $form['unassociate'] = array(
    '#type' => 'markup',
    '#prefix' => "<div><p>",
    '#suffix' => "</p></div>",
    '#markup' => t("Remove the TEI-RDF editor from this Versionable Object?"),
  );

  $form['tei_rdf'] = array(
    '#type' => 'hidden',
    '#value' => $tei_rdf,
  );

  $form['versionable_object'] = array(
    '#type' => 'hidden',
    '#value' => $object->id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove Association'),
  );
  return $form;
}

/**
 * Submit handler for CWRCWriter deletion form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 */
function islandora_critical_edition_delete_tei_rdf_form_submit($form, $form_state) {
  $tei_rdf_object = islandora_object_load($form_state['values']['tei_rdf']);
  $tei_rdf_object->relationships->remove(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $form_state['values']['versionable_object']);
  $tei_rdf_object->relationships->remove(FEDORA_RELS_EXT_URI, 'isMemberOf', $form_state['values']['versionable_object']);
  drupal_set_message(t("@label has been disassociated from the versionable object", array('@label' => $tei_rdf_object->label)));
}

/**
 * Form to associate existing TEI-RDF object.
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
function islandora_critical_edition_associate_tei_rdf_form($form, $form_state, $object, $content_model = 'islandora:criticalEditionCModel') {
  $object_models = $object->models;
  $system = array_search("fedora-system:FedoraObject-3.0", $object_models);
  unset($object_models[$system]);
  $object_model = array_shift($object_models);
  $offset = islandora_critical_edition_prep_form($object, $content_model);
  $name_mappings = array(
    'islandora:criticalEditionContainerCModel' => t('Critical Edition'),
    'islandora:versionableObjectCModel' => t('Versionable Object'),
  );
  $tei_rdfs = islandora_critical_edition_get_related_objects($content_model, $offset);
  $rows = array();
  foreach ($tei_rdfs as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  $header = array(
    'title' => array('data' => t('TEI-RDF Object')),
    'pid' => array('data' => t('PID')),
  );
  // Build and return table element.
  $element = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t("There are no RDF-TEI Objects in this Fedora Repository."),
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
  $form['tei_rdf_objects'] = $element;
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
 * Adds paging to form and determines offset.
 *
 * @param AbstrctObject $object
 *   Object representing containing object
 * @param String $content_model
 *   PID of content model to limit search results
 *
 * @return integer
 *   Offset for paging
 */
function islandora_critical_edition_prep_form($object, $content_model) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $offset = 0;
  $tuque = islandora_get_tuque_connection();
  $repository = $tuque->repository;
  $total = $repository->ri->countQuery(islandora_critical_edition_get_associated_objects_query($content_model, 0), 'sparql');
  $limit = 25;
  if ($total < 28) {
    $limit = $total;
  }
  if ($total) {
    $current_page = pager_default_initialize($total, $limit);
    $offset = $current_page * $limit;
  }

  return $offset;
}

/**
 * Form Submission handler.
 *
 * @param array $form
 *   Drupal Form
 * @param array $form_state
 *   Drupal Form State
 */
function islandora_critical_edition_associate_tei_rdf_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/versionable_object_creation.form');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  $type = $form_state['values']['container_object_model'];
  $tei_rdf = $form_state['values']['tei_rdf_objects'];
  $tei_rdf_object = islandora_object_load($tei_rdf);
  $relationships = $tei_rdf_object->relationships->get(ISLANDORA_RELS_EXT_URI, 'isCriticalEditionOf');
  $tn = NULL;
  if (!empty($relationships)) {
    $book_object = islandora_object_load($relationships[0]['object']['value']);
    $tn = $book_object['TN'];
    $label = $book_object->label;
  }
  
  if ($type == 'islandora:criticalEditionContainerCModel') {
    $container_object = islandora_object_load($form_state['values']['container_object_pid']);
    $versionable_object = islandora_critical_edition_build_vo($container_object, $label, $tn);
    // Build and attach basic transcription
    $transcription_label = t("Transcription of @label", array('@label' => $versionable_object->label));
    $transcription = islandora_critical_edition_advanced_build_transcription($versionable_object, $transcription_label);
    $datastream_id = "TRANSCRIPTION";
    $control_group = "M";
    $transcription_datastream = $transcription->constructDatastream($datastream_id, $control_group);
    $transcription_datastream->mimetype = 'text/plain';
  }
  else {
    $versionable_object_pid = $form_state['values']['container_object_pid'];
  }
  $tei_rdf_object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $versionable_object->id);
  if ($type == 'islandora:criticalEditionContainerCModel') {
    add_derived_transcription($tei_rdf_object, $versionable_object->id, $transcription, $transcription_datastream);
  }
}
