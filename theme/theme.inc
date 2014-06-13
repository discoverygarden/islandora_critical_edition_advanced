<?php

/**
 * @file
 * This file contains theming functions for islandora_critical_edition_advanced.
 */

/**
 * Implements hook_preprocess_theme().
 */
function islandora_critical_edition_preprocess_islandora_critical_apparatus(array &$variables) {
  islandora_critical_edition_advanced_prep_tabs();
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  // @TODO add string translation
  $variables['datastreams'] = variable_get('critical_apparatus_streams', islandora_critical_apparatus_get_datastreams());
}

/**
 * Implements hook_preprocess_theme().
 */
function islandora_critical_edition_preprocess_islandora_transcription_object(array &$variables) {
  islandora_critical_edition_advanced_prep_tabs();
  $multiple = FALSE;
  if (isset($variables['transcriptions']) && count($variables['transcriptions']) > 1) {
    $multiple = TRUE;
  }
  $variables['multiple'] = $multiple;
}

/**
 * Implements hook_preprocess_theme().
 */
function islandora_critical_edition_preprocess_islandora_critical_media_object(array &$variables) {
  global $base_url;
  module_load_include('inc', 'islandora_critical_edition', 'theme/theme');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/solution_packs');

  $critical_edition_module_path = drupal_get_path('module', 'islandora_critical_edition');
  $validate_path = variable_get('islandora_critical_edition_validate_url', '/validator/validate.html');
  drupal_add_js(array('islandora_critical_edition' => array('source_type' => 'media')), 'setting');
  islandora_critical_edition_add_js($variables, TRUE);
  $islandora_object = $variables['islandora_object'];
  $pages = islandora_paged_content_get_pages($islandora_object);
  $pids = array_keys($pages);
  drupal_add_css($critical_edition_module_path . "/css/islandora_critical_edition.css");
  // Fill additional session variables.
  $variables['images_path'] = url($critical_edition_module_path . '/CWRC-Writer/src/img/', array('absolute' => TRUE));

  drupal_add_js(array(
    'islandora_critical_edition' => array(
      'base_url' => $base_url)), 'setting');
  drupal_add_js(array(
    'islandora_critical_edition' => array(
      'validate_path' => $validate_path)), 'setting');
  // Passing the page pid, instead of getting if from the
  // URL on the javascript side.
  drupal_add_js(array(
    'islandora_critical_edition' => array(
      'page_pid' => $pids[0])), 'setting');

  drupal_add_js(drupal_get_path('module', 'islandora_critical_edition_advanced') . "/js/suppress_annotations.js");
  $reference_object = $variables['reference_object'];
  // Use the theme files from image annotation for annotation list
  // and image annotation pane's.
  $variables['anno_list_pane'] = theme('islandora_anno_list_pane');
  $variables['anno_img_pane'] = theme('islandora_anno_image_pane');
  if ($variables['content_model'] == 'islandora:sp-audioCModel') {
    $audio_params = islandora_critical_edition_advanced_get_audio_params($reference_object);
    if (isset($reference_object['PROXY_MP3'])) {
      $audio_params = islandora_critical_edition_advanced_get_audio_params($reference_object);
    }
    // Thumbnail.
    if (isset($reference_object['TN']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $reference_object['TN'])) {
      $tn_url = url("islandora/object/{$reference_object->id}/datastream/TN/view");
      $params = array(
        'title' => $reference_object->label,
        'path' => $tn_url,
      );
      $variables['islandora_thumbnail_img'] = theme('image', $params);
      $audio_params['tn'] = $tn_url;
    }
    drupal_add_css("$critical_edition_module_path/css/islandora_critical_edition.css");
    $viewer = islandora_get_viewer($audio_params, 'islandora_audio_viewers', $reference_object);

    if ($viewer) {
      $variables['islandora_content'] = $viewer;
    }
    elseif (isset($variables['islandora_thumbnail_img']) && isset($islandora_object['PROXY_MP3']) &&
        islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['PROXY_MP3'])) {

      $variables['islandora_content'] = l($variables['islandora_thumbnail_img'], $audio_url, array('html' => TRUE));
    }
    elseif (isset($islandora_object['PROXY_MP3']) && islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $islandora_object['PROXY_MP3'])) {
      $variables['islandora_content'] = l($islandora_object->label, $audio_url);
    }
  }
  if ($variables['content_model'] == 'islandora:sp_videoCModel') {
    $video_params = islandora_critical_edition_advanced_get_video_params($reference_object);
    $viewer = islandora_get_viewer($video_params, 'islandora_video_viewers', $reference_object);
    if ($viewer) {
      $variables['islandora_content'] = $viewer;
    }
  }
}

/**
 * Adds all js and css for a tabbed display.
 */
function islandora_critical_edition_advanced_prep_tabs() {
  module_load_include('inc', 'islandora_image_annotation', 'includes/utils');
  islandora_image_annotation_include_common_css();
  $critical_edition_module_path = drupal_get_path('module', 'islandora_critical_edition');
  drupal_add_js($critical_edition_module_path . '/CWRC-Writer/src/js/lib/jquery/jquery-1.8.3.js');
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery-ui-1.9.0.custom.min.js");
  $critical_edition_advanced_module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_js($critical_edition_advanced_module_path . "/js/activate_tabs.js");
  drupal_add_css("$critical_edition_advanced_module_path/css/jquery-ui.css");
}

/**
 * Implements hook_preprocess_theme().
 *
 * This function is resposible for fetching the required data to render the
 * critical edition object.
 */
function islandora_critical_edition_preprocess_islandora_critical_edition_container(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$module_path/css/critical_edition_container.css");
  // For now we are using views/ctools to style our drop downs if they are not
  // enabled plain links will be rendered.
  $views_module_path = drupal_get_path('module', 'views');
  drupal_add_css("$views_module_path/css/views-admin.ctools.css");
  drupal_add_css("$views_module_path/css/views-admin.seven.css");
  drupal_add_css("$module_path/css/version_display.css");
  drupal_add_js("$module_path/js/version_display.js");
  $object = $variables['object'];
  $variables['edit_link'] = "<div class='editing_view'><a href ='/islandora/object/$object/critical_edition/edit'><button>" . t('Go to Editing View') . "</button></a></div>";
  $variables['apparatus'] = array(
    '#markup' => theme('islandora_critical_edition_publish_view', array(
      'islandora_object' => $object,
      'publish_form' => NULL,
        )
    ),
  );
  $variables['versions'] = array(
    '#theme' => 'islandora_critical_edition_container_versions_table',
    '#object' => $object,
    '#columns' => array(
      'version_name' => TRUE,
      'object_type' => TRUE,
      'genre' => TRUE,
      'content' => TRUE,
      'context' => TRUE,
    ),
  );
  $variables['collations'] = array(
    '#theme' => 'islandora_critical_edition_container_collations_table',
    '#object' => $object,
  );
}

/**
 * Implements hook_preprocess_theme().
 *
 * This function is resposible for fetching the required data to render the
 * critical edition object.
 */
function islandora_critical_edition_preprocess_islandora_critical_edition_container_version(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$module_path/css/critical_edition_container.css");
  drupal_add_js("$module_path/js/select_transcription.js");
  // For now we are using views/ctools to style our drop downs if they are not
  // enabled plain links will be rendered.
  $views_module_path = drupal_get_path('module', 'views');
  drupal_add_css("$views_module_path/css/views-admin.ctools.css");
  drupal_add_css("$views_module_path/css/views-admin.seven.css");

  $object = $variables['object'];

  $variables['version_actions'] = islandora_critical_edition_container_version_links($object);
  $variables['versions'] = array(
    '#theme' => 'islandora_critical_edition_container_versions_table',
    '#object' => $object,
    '#columns' => array(
      'version_name' => TRUE,
      'object_type' => TRUE,
      'genre' => TRUE,
      'content' => TRUE,
      'context' => TRUE,
    ),
  );
}

/**
 * Implements hook_preprocess_theme().
 *
 * This function is resposible for fetching the required data to render the
 * critical edition object.
 */
function islandora_critical_edition_preprocess_islandora_critical_edition_container_edit(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$module_path/css/critical_edition_container.css");
  drupal_add_js("$module_path/js/select_transcription.js");
  // For now we are using views/ctools to style our drop downs if they are not
  // enabled plain links will be rendered.
  $views_module_path = drupal_get_path('module', 'views');
  drupal_add_css("$views_module_path/css/views-admin.ctools.css");
  drupal_add_css("$views_module_path/css/views-admin.seven.css");

  $object = $variables['object'];
  $apparatus = islandora_critical_edition_get_apparatus($object);

  $message = "<a title = 'Critical Apparatus' class='hastip' title='Manage Apparatus' href ='/islandora/object/$apparatus/manage/apparatus'><button>" . t('Manage Apparatus') . "</button></a>";
  $variables['apparatus_link'] = array('#markup' => "<div class = 'apparatus_link'><strong>$message</strong></div>");

  $variables['view_link'] = "<div class ='reading_view'><a title='Go to Reading View' class='hastip' href ='islandora/object/$object'><button>" . t('Go to Reading View') . "</button></a></div>";
  $variables['edition_actions'] = islandora_critical_edition_container_editor_links($object);
  $variables['version_actions'] = islandora_critical_edition_container_version_links($object);
  $variables['collation_actions'] = islandora_critical_edition_container_collation_links($object);

  // Hidden form for adding the object to the users workbench.
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/workbench');
  $variables['toggle_workbench_membership_form'] = drupal_get_form('islandora_critical_edition_advanced_toggle_workbench_membership_form', $object);

  $variables['versions'] = array(
    '#theme' => 'islandora_critical_edition_container_versions_table',
    '#object' => $object,
    '#show_actions' => TRUE,
    '#columns' => array(
      'version_name' => TRUE,
      'operations' => TRUE,
      'transcriptions' => TRUE,
    ),
  );
  $variables['collations'] = array(
    '#theme' => 'islandora_critical_edition_container_collations_table',
    '#object' => $object,
  );
}

/**
 * Get the definition for the editor links.
 */
function islandora_critical_edition_container_editor_links(AbstractObject $object) {
  $permissions = array(
    'edit_metadata' => user_access(ISLANDORA_METADATA_EDIT),
    'edit_permissions' => !function_exists('islandora_xacml_editor_access') || islandora_xacml_editor_access($object),
  );
  $links = array(
    'edit_metadata' => array(
      '#theme' => 'link',
      '#prefix' => '<div class="edit_critical_edition_metadata">',
      '#suffix' => '</div>',
      '#text' => t('Edit Edition Metadata'),
      '#path' => "islandora/object/{$object->id}/datastream/MODS/edit",
      '#options' => array(
        'html' => FALSE,
        'attributes' => array(
          'title' => 'Edit Edition Metadata',
          'class' => 'hastip',
        ),
      ),
    ),
    'edit_permissions' => array(
      '#theme' => 'link',
      '#prefix' => '<div class="edit_critical_edition_permissions">',
      '#suffix' => '</div>',
      '#text' => t('Edit Permissions'),
      '#path' => "islandora/object/{$object->id}/manage/xacml",
      '#options' => array(
        'html' => FALSE,
        'attributes' => array(
          'title' => 'Edit Permissions',
          'class' => 'hastip',
        ),
      ),
    ),
  );
  return array_intersect_key($links, array_filter($permissions));
}

/**
 * Get the definition for the version links.
 */
function islandora_critical_edition_container_version_links(AbstractObject $object) {
  $permissions = array(
    'add' => user_access(ISLANDORA_INGEST),
    'import' => user_access(ISLANDORA_INGEST),
  );
  $links = array(
    'add' => array(
      'title' => t('Add New Version'),
      'href' => "islandora/object/{$object->id}/manage/edition_container/ingest",
      'fragment' => 'create-vo',
    ),
    'import' => array(
      'title' => t('Associate Existing Version'),
      'href' => "islandora/object/{$object->id}/manage/edition_container",
      'fragment' => 'attach-vo',
    ),
  );
  return array(
    '#theme' => 'links__ctools_dropbutton',
    '#class' => array('version-links'),
    '#links' => array_intersect_key($links, array_filter($permissions)),
  );
}

/**
 * Get the definition for the collation links.
 */
function islandora_critical_edition_container_collation_links(AbstractObject $object) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $links = array(
    array(
      'title' => t('Add New Collation'),
      'href' => "islandora/object/{$object->id}/manage/edition_container",
      'fragment' => 'mvd',
    ),
  );
  $num_transcriptions = islandora_critical_edition_number_of_child_transcriptions($object);
  return array(
    '#theme' => 'links__ctools_dropbutton',
    '#class' => array('collation-links'),
    '#links' => (user_access('edit mvds') && $num_transcriptions >= 2) ? $links : array(),
  );
}

/**
 * Implements theme_hook().
 *
 * Renders out the table for display in the critical edition container.
 */
function theme_islandora_critical_edition_container_table(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  // Create local variables.
  extract(array_intersect_key($variables, drupal_map_assoc(array(
    'title', 'header', 'rows', 'offset', 'limit', 'total', 'pager', 'actions'))));
  // Render the table.
  $element = array(
    '#type' => 'fieldset',
    '#title' => '<span>' . t('@title (@start-@end of @total)', array(
      '@title' => $title,
      '@start' => $offset,
      '@end' => min($offset + $limit, $total),
      '@total' => $total)) . '</span>' . drupal_render($actions),
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#attributes' => array('class' => array("versions_table")),
    ),
    'pager' => array(
      '#theme' => 'pager',
      '#tags' => $pager['tags'],
      '#element' => $pager['element'],
      '#quantity' => $pager['quantity'],
    ),
  );
  return drupal_render($element);
}

/**
 * Implements theme_hook().
 *
 * For the versions table in the critical edition container display.
 */
function theme_islandora_critical_edition_container_versions_table(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  // Create local variables.
  extract(array_intersect_key($variables, drupal_map_assoc(array(
    'object', 'limit', 'pager', 'columns', 'show_actions'))));
  // Build Header.
  $headers = array(
    'version_name' => array(
      'data' => t('Version'),
      'field' => 'version_name',
      'class' => array('version_name'),
      'align' =>
      'left'),
    'object_type' => array(
      'data' => t('Type'),
      'field' => 'object_type',
      'class' => array('object_type'),
      'align' => 'left'),
    'genre' => array(
      'data' => t('Genre'),
      'field' => 'genre',
      'class' => array('genre'),
      'align' => 'left'),
    'content' => array(
      'data' => t('Content'),
      'align' => 'left',
      'class' => array('content')),
    'context' => array(
      'data' => t('Context'),
      'align' => 'left',
      'class' => array('context')),
    'operations' => array(
      'data' => t('Operations'),
      'align' => 'left',
      'class' => 'version_operations',
    ),
    'transcriptions' => array('data' => t('Transcriptions'), 'align' => 'left'),
  );
  // Operations may be hidden if the user doesn't have permission for any.
  $permissions = islandora_critical_edition_container_versions_table_operations_permissions();
  $columns['operations'] = isset($columns['operations']) && $columns['operations'] && count(array_filter($permissions));
  $headers = array_intersect_key($headers, array_filter($columns));
  // Init Pager.
  $total = islandora_critical_edition_number_of_child_versionable_objects($object);
  $page = pager_default_initialize($total, $limit, $pager['element']);
  $offset = $page * $limit;
  // Build Rows.
  $rows = array();
  $sort_field = tablesort_get_order($headers);
  $sort_field = $sort_field['sql'];
  $versions = islandora_critical_edition_container_versions_table_data($object, $sort_field, tablesort_get_sort($headers), $offset, $limit);
  foreach ($versions as $pid => $version) {
    $character_break = 40;
    $source = $version['source'];
    $content = prep_versiontable_string($version['content']);
    $context = prep_versiontable_string($version['context']);
    $cells = array(
      'version_name' => array(
        '#markup' => l($version['label'], "islandora/object/{$pid}", array(
          'attributes' =>
          array(
            'title' => $version['label'],
            'data-pid' => $pid,
            'class' => "hastip",
          ),
            )
        ),
      ),
      'object_type' => array(
        '#markup' => $version['object_type'],
      ),
      'genre' => array(
        '#markup' => $version['genre'],
      ),
      'content' => array('#markup' => $content),
      'context' => array('#markup' => $context),
      'operations' => islandora_critical_edition_container_versions_table_operations_cell($pid, $version, $source, $object->id),
      'transcriptions' => islandora_critical_edition_container_table_transcriptions_cell($pid, $version['transcriptions']),
    );
    // Only include cells which have headers.
    $cells = array_intersect_key($cells, $headers);
    $cells = array_map('drupal_render', $cells);
    foreach ($cells as $name => $data) {
      $rows[$pid][$name] = array('data' => $data, 'class' => $name);
    }
  }
  return theme('islandora_critical_edition_container_table', array(
    'title' => t('Versions'),
    'actions' => $show_actions ? islandora_critical_edition_container_version_links($object) : array(),
    'header' => $headers,
    'rows' => $rows,
    'limit' => $limit,
    'offset' => $offset,
    'total' => $total,
    'pager' => $pager,
  ));
}

/**
 * Gets all the required data for populating the Versions table.
 *
 * For the versions table in the critical edition container display.
 *
 * @param AbstractObject $object
 *   The critical edition containter object.
 * @param string $sort
 *   The column to sort on.
 * @param string $dir
 *   Either 'DESC' or 'ASC'.
 * @param int $offset
 *   The offset from which to return the version table data.
 * @param int $limit
 *   The limit on the number of versions returned.
 *
 * @return array
 *   An associative array containing relevent data for building the versions
 *   display table.
 */
function islandora_critical_edition_container_versions_table_data(AbstractObject $object, $sort = 'version_name', $dir = 'DESC', $offset = 0, $limit = 25) {
  // For now hardcode local variables for each field, at a later point they can
  // be put in the admin form.
  // Fields are hardcoded for now, but should eventually move to a configurable
  // setting.
  $pid_field = 'PID';
  $label_field = 'fgs_label_s';
  $source_pid_field = 'source_ms';
  $source_label_field = 'mods_titleInfo_title_ms';
  $object_type_field = 'mods_typeOfResource_ms';
  $genre_field = 'mods_genre_ms';
  $content_field = 'mods_abstract_ms';
  $context_field = 'mods_note_source note_ms';
  $child_relationship_field = 'RELS_EXT_isMemberOf_uri_ms';
  $model_field = 'RELS_EXT_hasModel_uri_ms';
  $query = "$child_relationship_field:\"info:fedora/{$object->id}\"";
  $query .= ' && ';
  $query .= "$model_field:\"info:fedora/islandora:versionableObjectCModel\"";
  // Handle Sorting.
  $sort_fields = array(
    'version_name' => $label_field,
    'object_type' => $object_type_field,
    'genre' => $genre_field,
  );
  $dir = strtolower($dir);
  $sort = $sort_fields[$sort];
  $params = array('sort' => "{$sort} {$dir}");
  $url = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));
  $solr = new Apache_Solr_Service($url['host'], $url['port'], $url['path'] . '/');
  $solr->setCreateDocuments(0);
  $versions = array();
  try {
    $results = $solr->search($query, $offset, $limit, $params);
    $results = json_decode($results->getRawResponse(), TRUE);
    foreach ($results['response']['docs'] as $result) {
      $pid = $result[$pid_field];
      $versions[$pid] = array(
        'label' => $result[$label_field],
        'source' => '',
        'object_type' => isset($result[$object_type_field][0]) ? $result[$object_type_field][0] : t('Undefined'),
        'genre' => isset($result[$genre_field][0]) ? $result[$genre_field][0] : t('Undefined'),
        'content' => isset($result[$content_field]) ? reset($result[$content_field]) : t('Undefined'),
        'context' => isset($result[$context_field]) ? reset($result[$context_field]) : t('Undefined'),
        'teirdf' => array(
          'pid' => '',
          'label' => '',
        ),
        'transcriptions' => array(),
      );
    }
  }
  catch (Exception $e) {
    watchdog_exception('islandora_critical_edition_advanced', $e, 'Got an exception while searching for versions.', array(), WATCHDOG_ERROR);
  }
  $versionable_objects;

  // Do a query for all transcripts and TEI-RDF objects we require.
  $query = <<<EOT
    PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    PREFIX islandora: <http://islandora.ca/ontology/relsext#>
    SELECT DISTINCT ?version ?child ?child_label ?source ?content_model FROM <#ri> WHERE {
        ?version fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
        fedora-model:hasModel <info:fedora/islandora:versionableObjectCModel> .
        ?child fedora-rels-ext:isMemberOf ?version ;
        fedora-model:hasModel ?content_model .
        OPTIONAL {  ?child fedora-model:label ?child_label . }
        OPTIONAL { ?child islandora:isCriticalEditionOf ?source . }
        FILTER (?content_model != <info:fedora/fedora-system:FedoraObject-3.0>)
    }
EOT;
  // Add the transcription and TEI-RDF objects info to the versions array.
  $results = $object->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $pid = $result['version']['value'];
    if (!isset($versions[$pid])) {
      continue;
    }
    $version = &$versions[$pid];
    $child = $result['child']['value'];
    $child_label = $result['child_label']['value'];
    switch ($result['content_model']['value']) {
      case 'islandora:criticalEditionCModel':
        $version['source'] = $result['source']['value'];
        $version['teirdf'] = array(
          'pid' => $child,
          'label' => $child_label,
        );
        break;

      case 'islandora:transcriptionCModel':
        $version['transcriptions'][$child] = $child_label;
        break;
    }
  }
  return $versions;
}

/**
 * Gets the permissions for all the version operations.
 *
 * @return array
 *   An associative array of boolean values indicating the user can perform the
 *   action.
 *   - consolidate: Consolitate the TEI-RDF associated with a version.
 *   - edit: Edit the version metadata.
 *   - delete: Purge the version and it's children.
 */
function islandora_critical_edition_container_versions_table_operations_permissions() {
  return array(
    'consolidate' => user_access(ISLANDORA_ADD_DS),
    'edit' => user_access(ISLANDORA_METADATA_EDIT),
    'delete' => user_access(ISLANDORA_PURGE),
    'source' => user_access(ISLANDORA_VIEW_OBJECTS),
    'persist' => user_access(ISLANDORA_ADD_DS),
    'download' => user_access(ISLANDORA_VIEW_OBJECTS),
    'upload' => user_access(ISLANDORA_ADD_DS),
  );
}

/**
 * Generates the transciptions cell for the given version's transcriptions.
 *
 * @param string $version
 *   The PID of the versionable object the transcriptions belong to.
 * @param array $transcriptions
 *   An associative array where the keys are the pids of the transcriptions and
 *   the values are the object's labels.
 * @param array $links
 *   The set of links to limit the display to, if none are provided all will be
 *   shown.
 *
 * @return array
 *   A renderable array containing the links the user has access to.
 */
function islandora_critical_edition_container_table_transcriptions_cell($version, array $transcriptions, array $links = array()) {
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_js("$module_path/js/select_transcription.js");
  $permissions = array(
    // Assuming we can always view if we got to this point.
    'view' => TRUE,
    'add' => user_access(ISLANDORA_INGEST),
    'edit' => user_access(ISLANDORA_METADATA_EDIT),
    'delete' => user_access(ISLANDORA_PURGE),
  );
  // Limit links to those provided.
  if (!empty($links)) {
    $permissions = array_intersect_key($permissions, drupal_map_assoc($links));
  }
  reset($transcriptions);
  $transcription = key($transcriptions);
  $links = array(
    'view' => array(
      'title' => t('View'),
      'href' => "islandora/object/{$transcription}",
      'attributes' => array('title' => t('View the selected Transcript')),
    ),
    'add' => array(
      'title' => t('Add'),
      'href' => "islandora/object/{$version}/manage/versionable_object",
      'fragment' => 'add-transcription',
      'attributes' => array('title' => t('Add a new Transcript')),
    ),
    'edit' => array(
      'title' => t('Edit'),
      'href' => "islandora/transcription/edit/{$transcription}",
      'attributes' => array('title' => t('Edit the selected Transcript')),
    ),
    'delete' => array(
      'title' => t('Delete'),
      'href' => "islandora/object/{$transcription}/delete",
      'attributes' => array('title' => t('Permanently delete the selected Transcript')),
    ),
  );
  // We can only add if there are no transcriptions.
  if (count($transcriptions) == 0 && isset($permissions['add']) && $permissions['add']) {
    return array(
      '#theme' => 'links__ctools_dropbutton',
      '#links' => array($links['add']),
    );
  }
  elseif (count($transcriptions) > 0) {
    return array(
      '#prefix' => '<div class="transcriptions-container">',
      '#suffix' => '</div>',
      'select' => array(
        '#type' => 'select',
        '#options' => $transcriptions,
        '#attributes' => array(
          'class' => array('transcription-select'),
        ),
      ),
      'links' => array(
        '#theme' => 'links__ctools_dropbutton',
        // Only display links the user has access to.
        '#links' => array_intersect_key($links, array_filter($permissions)),
        '#class' => array('transcription-links'),
      ),
    );
  }
  else {
    return NULL;
  }
}

/**
 * Generates the operations cell for the given version.
 *
 * @param string $pid
 *   The PID of the version.
 * @param array $version
 *   An associative array where the keys are properites of the verions and its
 *   children.
 *
 * @return array
 *   A renderable array containing the links the user has access to.
 */
function islandora_critical_edition_container_versions_table_operations_cell($pid, array $version, $source_pid, $critical_edition_pid) {
  $permissions = islandora_critical_edition_container_versions_table_operations_permissions();
  // If we can't render any links skip the rest of the logic.
  if (count(array_filter($permissions)) == 0) {
    return NULL;
  }
  // Get the TEI-RDF object if it exists.
  $tei_rdfs = array_keys(islandora_critical_edition_get_members($pid, 'islandora:criticalEditionCModel'));
  $tei_rdf = $tei_rdfs[0];
  $links = array(
    'edit' => array(
      'title' => t("Edit Object Metadata"),
      'href' => "islandora/object/{$pid}/datastream/MODS/edit",
    ),
    'consolidate' => array(
      'title' => t('Consolidate TEI'),
      'href' => "islandora/object/{$pid}/manage/versionable_object",
      'fragment' => 'update-datastreams',
    ),
    'upload' => array(
      'title' => t('Upload TEI'),
      'href' => "islandora/object/{$pid}/manage/versionable_object",
      'fragment' => 'upload-tei',
    ),
    'download' => array(
      'title' => t('Download TEI'),
      'href' => "islandora/object/{$tei_rdf}/datastream/TEI/download",
      'fragment' => 'update-datastreams',
    ),
    'persist' => array(
      'title' => t('Update Source OCR'),
      'href' => "islandora/object/{$pid}/manage/versionable_object",
      'fragment' => 'persist-ocr',
    ),
    'source' => array(
      'title' => t('View Source Object'),
      'href' => "islandora/object/{$source_pid}",
    ),
    'delete' => array(
      'title' => t('Delete'),
      'href' => "islandora/object/{$pid}/delete/$critical_edition_pid",
    ),
  );
  return array(
    '#prefix' => '<div class="operations-container">',
    '#suffix' => '</div>',
    '#theme' => 'links__ctools_dropbutton',
    // Only display links the user has access to.
    '#links' => array_intersect_key($links, array_filter($permissions)),
    '#class' => array('operation-links'),
  );
}

/**
 * Implements theme_hook().
 *
 * For the versions table in the critical edition container display.
 */
function theme_islandora_critical_edition_container_collations_table(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  // Create local variables.
  extract(array_intersect_key($variables, drupal_map_assoc(array(
    'object', 'limit', 'pager', 'columns'))));
  // Build Header.
  $headers = array(
    'collation_name' => array('data' => t('Collation Name'), 'field' => 'collation_name'),
    'transcriptions' => array('data' => t('Transcriptions'), 'class' => array('collation_transcription')),
    'collation_filter' => array('data' => t('Filter'), 'class' => array('collation_filter')),
    'operations' => array('data' => t('Operations'), 'class' => 'collation_operations'),
  );
  // Operations may be hidden if the user doesn't have permission for any.
  $permissions = islandora_critical_edition_container_collation_table_operations_permissions();
  $columns['operations'] = isset($columns['operations']) && $columns['operations'] && count(array_filter($permissions));
  $headers = array_intersect_key($headers, array_filter($columns));
  // Init Pager.
  $mvds = islandora_calliope_find_mvds($object);
  $total = count($mvds);
  $page = pager_default_initialize($total, $limit, $pager['element']);
  $offset = $page * $limit;
  // Build Rows.
  $rows = array();
  $collations = islandora_calliope_find_mvds($object, tablesort_get_sort($headers), $limit, $offset);
  foreach ($collations as $pid => $collation) {
    $cells = array(
      'collation_name' => array('#markup' => $collation['name']),
      'transcriptions' => islandora_critical_edition_container_table_transcriptions_cell($pid, $collation['transcriptions'], array(FALSE)),
      'collation_filter' => array('#markup' => $collation['filter']),
      'operations' => islandora_critical_edition_container_collation_table_operations_cell($pid, $object->id, $collation['transcriptions']),
    );
    // Only include cells which have headers.
    $cells = array_intersect_key($cells, $headers);
    $cells = array_map('drupal_render', $cells);
    foreach ($cells as $name => $data) {
      $rows[$pid][$name] = array('data' => $data, 'class' => $name);
    }
  }
  $output = theme('islandora_critical_edition_container_table', array(
    'title' => t('Collations'),
    'header' => $headers,
    'rows' => $rows,
    'limit' => $limit,
    'offset' => $offset,
    'total' => $total,
    'pager' => $pager,
  ));
  return $output;
}

/**
 * Implements theme_hook().
 *
 * For the versions table in the critical edition container display.
 */
function theme_islandora_critical_edition_container_collation_view_table(array &$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  // Create local variables.
  extract(array_intersect_key($variables, drupal_map_assoc(array(
    'object', 'limit', 'pager', 'columns'))));
  // Build Header.
  $headers = array(
    'collation_name' => array('data' => t('Collation Name'), 'field' => 'collation_name'),
    'transcriptions' => array('data' => t('Transcriptions')),
    'collation_filter' => array('data' => t('Description')),
    'operations' => array('data' => t('Operations')),
  );
  // Operations may be hidden if the user doesn't have permission for any.
  $permissions = islandora_critical_edition_container_collation_table_operations_permissions();
  $columns['operations'] = isset($columns['operations']) && $columns['operations'] && count(array_filter($permissions));
  // Init Pager.
  $mvds = islandora_calliope_find_mvds($object);
  $total = count($mvds);
  $page = pager_default_initialize($total, $limit, $pager['element']);
  $offset = $page * $limit;
  // Build Rows.
  $rows = array();
  $collations = islandora_calliope_find_mvds($object, tablesort_get_sort($headers), $limit, $offset);
  foreach ($collations as $pid => $collation) {
    $cells = array(
      'collation_name' => array('#markup' => $collation['name']),
      'transcriptions' => islandora_critical_edition_container_table_transcriptions_cell($pid, $collation['transcriptions'], array(FALSE)),
      'collation_filter' => array('#markup' => $collation['description']),
      'operations' => islandora_critical_edition_container_collation_table_operations_cell($pid, $object->id, $collation['transcriptions']),
    );
    // Only include cells which have headers.
    $cells = array_intersect_key($cells, $headers);
    $cells = array_map('drupal_render', $cells);
    foreach ($cells as $name => $data) {
      $rows[$pid][$name] = array('data' => $data, 'class' => $name);
    }
  }
  $output = theme('islandora_critical_edition_container_table', array(
    'title' => t('Collations'),
    'header' => $headers,
    'rows' => $rows,
    'limit' => $limit,
    'offset' => $offset,
    'total' => $total,
    'pager' => $pager,
  ));
  return $output;
}

/**
 * Generates the operations cell for the given collation.
 *
 * @param string $collation
 *   The PID of the collation.
 *
 * @return array
 *   A renderable array containing the links the user has access to.
 */
function islandora_critical_edition_container_collation_table_operations_cell($collation, $critical_edition_pid, array $transcriptions) {
  $permissions = islandora_critical_edition_container_collation_table_operations_permissions();
  // If we can't render any links skip the rest of the logic.
  if (count(array_filter($permissions)) == 0) {
    return NULL;
  }
  $links = array(
    'compare' => array(
      'title' => t('Compare'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/compare",
    ),
    'table_view' => array(
      'title' => t('Table'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/apparatus",
    ),
    'refresh' => array(
      'title' => t('Refresh'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/refresh",
    ),
    'delete' => array(
      'title' => t('Delete'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/delete",
    ),
  );
  return array(
    '#prefix' => '<div class="operations-container">',
    '#suffix' => '</div>',
    '#theme' => 'links__ctools_dropbutton',
    // Only display links the user has access to.
    '#links' => array_intersect_key($links, array_filter($permissions)),
    '#class' => array('operation-links'),
  );
}

/**
 * Gets the permissions for all the collation operations.
 *
 * @return array
 *   An associative array of boolean values indicating the user can perform the
 *   action.
 */
function islandora_critical_edition_container_collation_table_operations_permissions() {
  return array(
    'refresh' => user_access('edit mvds'),
    'compare' => user_access('view collation tools'),
    'table_view' => user_access('view collation tools'),
    'delete' => user_access(ISLANDORA_PURGE),
  );
}

/**
 * Wraps strings in span classes.
 *
 * @param string $input
 *   Input string
 * @param Integer $character_break
 *   Length of teaser string to be displayed
 */
function prep_versiontable_string($input, $character_break = 25) {
  if (strlen($input) < $character_break) {
    return $input;
  }
  $last_space = strrpos(substr($input, 0, $character_break + 1), ' ');
  $teaser = substr($input, 0, $last_space);
  $teaser = "<span class='teaser teaser_only'>$teaser</span>";
  $content = "$teaser<span class='full_text full_text_hidden'>" . substr($input, $last_space) . "</span>";
  return $content;
}
