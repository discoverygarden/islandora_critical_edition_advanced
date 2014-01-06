<?php

/**
 * @file
 * Workbench page and related forms.
 */

/**
 * Display's the current users work bench.
 */
function islandora_critical_edition_advanced_workbench() {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$module_path/css/islandora_critical_edition.workbench.css");
  // For now we are using views/ctools to style our drop downs if they are not
  // enabled plain links will be rendered.
  $views_module_path = drupal_get_path('module', 'views');
  drupal_add_css("$views_module_path/css/views-admin.ctools.css");
  drupal_add_css("$views_module_path/css/views-admin.seven.css");
  $default_list = islandora_bookmark_get_default_list();
  $bookmark_form = NULL;
  if ($default_list !== FALSE) {
    $bookmark_form = drupal_get_form('islandora_bookmark_detailed_form', $default_list);
  }
  return array(
    'description' => array(
      '#type' => 'markup',
      '#prefix' => '<p>',
      '#suffix' => '</p>',
      '#markup' => t('Welcome to your workbench. Here you can create new critical editions, access active editions, or remove editions from your workbench. Or you can ingest new source objects and manage collections in the Co-op.'),
    ),
    'actions' => array(
      'add_new_critical_edition' => array(
        '#theme' => 'links__ctools_dropbutton',
        '#links' => array(
          array(
            'title' => t('Add New Critical Edition'),
            'href' => 'workbench/add/islandora:criticalEditionContainerCModel',
          ),
        ),
      ),
      'add_new_schema' => array(
        '#theme' => 'links__ctools_dropbutton',
        '#links' => array(
          array(
            'title' => t('Add New Schema / Transform'),
            'href' => 'workbench/add/islandora:criticaleditionschemaCModel',
          ),
        ),
      ),
      'add_new_source_object' => array(
        '#prefix' => '<span class="ctools-dropbutton-wrapper">',
        '#suffix' => '</span>',
        '#theme' => 'links__ctools_dropbutton',
        '#links' => array(
          array(
            'title' => t('Add New Text / Image Object'),
            'href' => 'workbench/add/islandora:bookCModel',
          ),
          array(
            'title' => t('Add New Audio Object'),
            'href' => 'workbench/add/islandora:sp-audioCModel',
          ),
          array(
            'title' => t('Add New Video Object'),
            'href' => 'workbench/add/islandora:sp_videoCModel',
          ),
          array(
            'title' => t('Add New Collection'),
            'href' => 'workbench/add/islandora:collectionCModel',
          ),
        ),
      ),
    ),
    'bookmarks' => $bookmark_form,
  );
}

/**
 * Display form allowing the user to select collection.
 *
 * @param string $content_model
 *   The PID of the Content Model type to ingest.
 *
 * @return array
 *   A Drupal Form for ingesting a new object of the given type.
 */
function islandora_critical_edition_advanced_workbench_ingest($content_model) {
  global $user;
  module_load_include('inc', 'islandora', 'includes/ingest.form');
  $valid_content_models = array(
    'islandora:criticalEditionContainerCModel' => array(
      'label' => 'Critical Edition',
      'default_collection' => 'islandora:criticalEditionCollection',
      'select_collection' => TRUE,
    ),
    'islandora:bookCModel' => array(
      'label' => 'Text / Image Object',
      'default_collection' => 'islandora:sourceCollection',
      'select_collection' => TRUE,
    ),
    'islandora:sp-audioCModel' => array(
      'label' => 'Audio Object',
      'default_collection' => 'islandora:sourceCollection',
      'select_collection' => TRUE,
    ),
    'islandora:sp_videoCModel' => array(
      'label' => 'Video Object',
      'default_collection' => 'islandora:sourceCollection',
      'select_collection' => TRUE,
    ),
    'islandora:collectionCModel' => array(
      'label' => 'Collection',
      'default_collection' => 'islandora:sourceCollection',
      'select_collection' => TRUE,
    ),
    'islandora:criticaleditionschemaCModel' => array(
      'label' => 'Schema',
      'default_collection' => 'islandora:tei_schema_collection',
      'select_collection' => FALSE,
    ),
  );
  if (!in_array($content_model, array_keys($valid_content_models))) {
    drupal_not_found();
  }
  $info = $valid_content_models[$content_model];
  $tuque = islandora_get_tuque_connection();
  $object = $tuque->repository->constructObject('islandora');
  $object->owner = $user->name;
  $object->label = t('New @type', array('@type' => $info['label']));
  $object->models = $content_model;
  $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $info['default_collection']);
  drupal_set_title(t('Add New @type', array('@type' => $info['label'])));
  return drupal_get_form('islandora_ingest_form', array(
      'require_select_collection_step' => $info['select_collection'],
      'parent' => $valid_content_models[$content_model]['default_collection'],
      'models' => array($content_model),
      'objects' => array($object)));
}

/**
 * Autocomplete menu callback for collection objects.
 */
function islandora_critical_edition_advanced_workbench_collection_autocomplete($search) {
  $query = <<<EOT
    PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    PREFIX islandora: <http://islandora.ca/ontology/relsext#>
      SELECT DISTINCT ?object ?label FROM <#ri> WHERE {
        ?object fedora-model:hasModel <info:fedora/islandora:collectionCModel> .
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
 * Allows the user to select the Collection the Object will be a child of.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_select_collection_form(array $form, array &$form_state) {
  $shared_storage = islandora_ingest_form_get_shared_storage($form_state);
  $parent = $shared_storage['parent'];
  return array(
    'collection' => array(
      '#type' => 'textfield',
      '#title' => t('Select Collection'),
      '#required' => TRUE,
      '#default_value' => $parent,
      '#autocomplete_path' => 'workbench/collection_autocomplete',
    ),
  );
}

/**
 * Validate the form.
 */
function islandora_critical_edition_advanced_select_collection_form_validate(array $form, array &$form_state) {
  $object = islandora_object_load($form_state['values']['collection']);
  if (!$object) {
    form_set_error('collection', t('The collection must be a valid object.'));
  }
}

/**
 * Use the select collection as the parent of the new Object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_select_collection_form_submit(array $form, array &$form_state) {
  $shared_storage = &islandora_ingest_form_get_shared_storage($form_state);
  $shared_storage['parent'] = $form_state['values']['collection'];
  $object = islandora_ingest_form_get_object($form_state);
  $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOf', $form_state['values']['collection']);
}

/**
 * Undo the selection of a collection as the parent of the new Object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_select_collection_form_undo_submit(array $form, array &$form_state) {
  $shared_storage = islandora_ingest_form_get_shared_storage($form_state);
  unset($shared_storage['parent']);
  // Be warned this removes all parent relationships infomation.
  $object = islandora_ingest_form_get_object($form_state);
  $object->relationships->remove(FEDORA_RELS_EXT_URI, 'isMemberOf');
}

/**
 * Add the given object to the Work Bench.
 *
 * @param AbstractObject $object
 *   The object to add to the work bench.
 */
function islandora_critical_edition_advanced_add_to_workbench(AbstractObject $object) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    Bookmark::getList($default_list)->addPid($object->id);
  }
}

/**
 * Remove the given object from the Work Bench.
 *
 * We don't check that the object exists for removal, as we don't care if it
 * exists anymore.
 *
 * @param string $pid
 *   The object to remove to the work bench.
 */
function islandora_critical_edition_advanced_remove_from_workbench($pid) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    Bookmark::getList($default_list)->removePid($pid);
  }
}

/**
 * Toggles the given objects membership to the current users workbench.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_toggle_workbench_membership_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  form_load_include($form_state, 'inc', 'islandora_critical_edition_advanced', 'includes/workbench');
  $in_work_bench = FALSE;
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    $default_list = Bookmark::getList($default_list);
    $in_work_bench = in_array($object->id, $default_list->getPids());
  }
  $class = $in_work_bench ? 'remove-from-workbench hastip' : 'add-to-workbench hastip';
  return array(
    'object' => array(
      '#type' => 'value',
      '#value' => $object->id,
    ),
    'in_work_bench' => array(
      '#type' => 'value',
      '#value' => $in_work_bench,
    ),
    'submit' => array(
      '#type' => 'submit',
      '#attributes' => array('class' => array($class), 'title' => 'Add to Workbench'),
      '#value' => $in_work_bench ? t('Remove from Workbench') : t('Add to Workbench'),
    ),
  );
}

/**
 * Toggles the membership of the objects within the work bench.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_toggle_workbench_membership_form_submit(array $form, array &$form_state) {
  $object = islandora_object_load($form_state['values']['object']);
  if ($form_state['values']['in_work_bench']) {
    islandora_critical_edition_advanced_remove_from_workbench($object->id);
  }
  else {
    islandora_critical_edition_advanced_add_to_workbench($object);
  }
}
