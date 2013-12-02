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
  islandora_critical_edition_add_js();
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
    if (isset($reference_object['PROXY_MP3'])) {
      $audio_url = url("islandora/object/{$reference_object->id}/datastream/PROXY_MP3/view", array('absolute' => TRUE));
      $audio_params = array(
        "pid" => $reference_object->id,
        "url" => $audio_url,
        "mime" => 'audio/mpeg',
        "width" => 175,
      );
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
    if (stristr($_SERVER['HTTP_USER_AGENT'], 'chrome') !== FALSE) {
      $viewer_dsid = 'OGG';
    }
    else {
      $viewer_dsid = 'MP4';
    }

    $video_params = array(
      'pid' => $reference_object->id,
    );
    // Video player.
    if (isset($reference_object[$viewer_dsid]) && islandora_datastream_access(FEDORA_VIEW_OBJECTS, $reference_object[$viewer_dsid])) {
      $video_url = url("islandora/object/{$reference_object->id}/datastream/$viewer_dsid/view");
      $video_params += array(
        'mime' => 'video/mp4',
        'url' => $video_url,
      );
    }
    // Thumbnail.
    if (isset($reference_object['TN']) && islandora_datastream_access(FEDORA_VIEW_OBJECTS, $reference_object['TN'])) {
      $video_params += array(
        'tn' => url("islandora/object/{$reference_object->id}/datastream/TN/view", array('absolute' => TRUE)),
      );
    }

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

  $object = $variables['object'];
  $apparatus = islandora_critical_edition_get_apparatus($object);
  $link = l(t('Here'), "islandora/object/$apparatus");
  $message = t('Apparatus Goes !here', array('!here' => $link));
  $variables['apparatus'] = array('#markup' => "<div><strong>$message</strong></div>");
  $variables['edition_actions'] = islandora_critical_edition_container_editor_links($object);
  $variables['version_actions'] = islandora_critical_edition_container_version_links($object);
  $variables['collation_actions'] = islandora_critical_edition_container_collation_links($object);
  $variables['versions'] = array(
    '#theme' => 'islandora_critical_edition_container_versions_table',
    '#object' => $object,
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
    'edit_permissions' => function_exists('islandora_xacml_editor_access') && islandora_xacml_editor_access($object),
    'workbench' => user_access('use islandora_bookmark'),
  );
  $links = array(
    'edit_metadata' => array(
      'title' => t('Edit Edition Metadata'),
      'href' => "islandora/object/{$object->id}/datastream/MODS/edit",
    ),
    'edit_permissions' => array(
      'title' => t('Edit Permissions'),
      'href' => "islandora/object/{$object->id}/manage/xacml",
    ),
    'workbench' => array(
      'title' => t('Add to Workbench'),
      'href' => '',
      'external' => TRUE,
      'fragment' => 'not_implemented',
    ),
  );
  return array(
    '#theme' => 'links__ctools_dropbutton',
    '#class' => array('edition-links'),
    '#links' => array_intersect_key($links, array_filter($permissions)),
  );
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
      'href' => "islandora/object/{$object->id}/manage/edition_container",
      'fragment' => 'create-vo',
    ),
    'import' => array(
      'title' => t('Import Version'),
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
          'title', 'header', 'rows', 'offset', 'limit', 'total', 'pager'))));
  // Render the table.
  $element = array(
    '#type' => 'fieldset',
    '#title' => t('@title (@start-@end of @total)', array(
                '@title' => $title,
                '@start' => $offset,
                '@end' => min($offset + $limit, $total),
                '@total' => $total,
              )),
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
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
          'object', 'limit', 'pager', 'columns'))));
  // Build Header.
  $headers = array(
    'version_name' => array('data' => t('Version Name'), 'field' => 'version_name'),
    'source_name' => array('data' => t('Source Name')),
    'transcriptions' => array('data' => t('Transcriptions')),
    'operations' => array('data' => t('Operations')),
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
  $versions = islandora_critical_edition_container_versions_table_data($object, tablesort_get_sort($headers), $offset, $limit);
  foreach ($versions as $pid => $version) {
    $source = $version['source'];
    $cells = array(
      'version_name' => array(
        '#markup' => l($version['label'], "islandora/object/{$pid}",
                   array('attributes' => array('title' => $version['label']))),
      ),
      'source_name' => array(
        '#markup' => l($source->label, "islandora/object/{$source->id}",
                   array('attributes' => array('title' => $source->label))),
      ),
      'transcriptions' => islandora_critical_edition_container_table_transcriptions_cell($pid, $version['transcriptions']),
      'operations' => islandora_critical_edition_container_versions_table_operations_cell($pid, $version),
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
function islandora_critical_edition_container_versions_table_data(AbstractObject $object, $sort = 'DESC', $offset = 0, $limit = 25) {
  // Sort must be uppercase in SPARQL.
  $sort = strtoupper($sort);
  $versions = array();
  // Get the versions and their sources between offset and limit.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
PREFIX islandora: <http://islandora.ca/ontology/relsext#>
SELECT DISTINCT ?object ?label ?source FROM <#ri> WHERE {
 ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:versionableObjectCModel> .
 ?x fedora-rels-ext:isMemberOf ?object ;
    fedora-model:hasModel <info:fedora/islandora:criticalEditionCModel> ;
    islandora:isCriticalEditionOf ?source .
 OPTIONAL { ?object fedora-model:label  ?label. }
}
ORDER BY $sort(?label)
OFFSET $offset
LIMIT $limit
EOT;
  // Do a query for all transcripts.
  $results = $object->repository->ri->sparqlQuery($query, $limit);
  foreach ($results as $result) {
    $pid = $result['object']['value'];
    $versions[$pid] = array(
      'label' => $result['label']['value'],
      // Unfortunately we can't get the source title as it is stored as literal
      // value in the resource index so we must get the object.
      'source' => islandora_object_load($result['source']['value']),
      'teirdf' => array(),
      'transcriptions' => array(),
    );
  }
  // Do a query for all transcripts and TEI-RDF objects we require.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
SELECT DISTINCT ?version ?child ?child_label ?content_model FROM <#ri> WHERE {
 ?version fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:versionableObjectCModel> .
 ?child fedora-rels-ext:isMemberOf ?version ;
        fedora-model:hasModel ?content_model .
 OPTIONAL {
  ?child  fedora-model:label ?child_label .
 }
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
 *   - partial: Create Partial versions from an existing version.
 *   - consolidate: Consolitate the TEI-RDF associated with a version.
 *   - edit: Edit the version metadata.
 *   - delete: Purge the version and it's children.
 */
function islandora_critical_edition_container_versions_table_operations_permissions() {
  return array(
    'partial' => user_access(ISLANDORA_INGEST),
    'consolidate' => user_access(ISLANDORA_ADD_DS),
    'edit' => user_access(ISLANDORA_METADATA_EDIT),
    'delete' => user_access(ISLANDORA_PURGE),
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
function islandora_critical_edition_container_versions_table_operations_cell($pid, array $version) {
  $permissions = islandora_critical_edition_container_versions_table_operations_permissions();
  // If we can't render any links skip the rest of the logic.
  if (count(array_filter($permissions)) == 0) {
    return NULL;
  }
  $links = array(
    'partial' => array(
      'title' => t('Create Partial Version'),
      'href' => '',
      'external' => TRUE,
      'fragment' => 'not_implemented',
    ),
    'consolidate' => array(
      'title' => t('Consolidate TEI'),
      'href' => "islandora/object/{$pid}/manage/versionable_object",
      'fragment' => 'update-datastreams',
    ),
    'edit' => array(
      'title' => t('Edit'),
      'href' => "islandora/object/{$pid}/datastream/MODS/edit",
    ),
    'delete' => array(
      'title' => t('Delete'),
      'href' => "islandora/object/{$pid}/delete",
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
    'transcriptions' => array('data' => t('Transcriptions')),
    'operations' => array('data' => t('Operations')),
  );
  // Operations may be hidden if the user doesn't have permission for any.
  $permissions = islandora_critical_edition_container_collation_table_operations_permissions();
  $columns['operations'] = isset($columns['operations']) && $columns['operations'] && count(array_filter($permissions));
  $headers = array_intersect_key($headers, array_filter($columns));
  // Init Pager.
  $total = islandora_critical_edition_number_of_child_versionable_objects($object);
  $page = pager_default_initialize($total, $limit, $pager['element']);
  $offset = $page * $limit;
  // Build Rows.
  $rows = array();
  $collations = islandora_calliope_find_mvds($object, tablesort_get_sort($headers), $limit, $offset);
  foreach ($collations as $pid => $collation) {
    $cells = array(
      'collation_name' => array(
        '#markup' => l($collation['label'], "islandora/object/{$pid}",
                   array('attributes' => array('title' => $collation['label']))),
      ),
      'transcriptions' => islandora_critical_edition_container_table_transcriptions_cell($pid, $collation['transcriptions'], array('edit')),
      'operations' => islandora_critical_edition_container_collation_table_operations_cell($pid, $object->id, $collation['transcriptions']),
    );
    // Only include cells which have headers.
    $cells = array_intersect_key($cells, $headers);
    $cells = array_map('drupal_render', $cells);
    foreach ($cells as $name => $data) {
      $rows[$pid][$name] = array('data' => $data, 'class' => $name);
    }
  }
  return theme('islandora_critical_edition_container_table', array(
      'title' => t('Collations'),
      'header' => $headers,
      'rows' => $rows,
      'limit' => $limit,
      'offset' => $offset,
      'total' => $total,
      'pager' => $pager,
    ));
}

/**
 * Gets all the required data for populating the collation table.
 *
 * For the versions table in the critical edition container display.
 *
 * @param AbstractObject $object
 *   The critical edition containter object.
 * @param string $sort
 *   Either 'DESC' or 'ASC'.
 * @param int $offset
 *   The offset from which to return the version table data.
 * @param int $limit
 *   The limit on the number of versions returned.
 *
 * @return array
 *   An associative array containing relevent data for building the collation
 *   display table.
 */
function islandora_critical_edition_container_collation_table_data(AbstractObject $object, $sort = 'DESC', $offset = 0, $limit = 25) {
  // Sort must be uppercase in SPARQL.
  $sort = strtoupper($sort);
  // Do a query for all collations we require.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
PREFIX islandora: <http://islandora.ca/ontology/relsext#>
SELECT DISTINCT ?object ?label FROM <#ri> WHERE {
 ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:mvdCModel> .
 OPTIONAL { ?object fedora-model:label  ?label. }
}
ORDER BY $sort(?label)
OFFSET $offset
LIMIT $limit
EOT;
  $collations = array();
  $results = $object->repository->ri->sparqlQuery($query, $limit);
  foreach ($results as $result) {
    $pid = $result['object']['value'];
    $collations[$pid] = array(
      'label' => $result['label']['value'],
      'transcriptions' => array(),
    );
  }
  // Add the transcription to the collation array.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
SELECT DISTINCT ?collation ?transcription ?transcription_label FROM <#ri> WHERE {
 ?collation fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
            fedora-model:hasModel <info:fedora/islandora:mvdCModel> .
 ?transcription fedora-rels-ext:isPartOf ?collation ;
                fedora-model:hasModel <info:fedora/islandora:transcriptionCModel> .
 OPTIONAL { ?transcription fedora-model:label ?transcription_label . }
}
EOT;
  // Add the transcription and TEI-RDF objects info to the versions array.
  $results = $object->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $collation = $result['collation']['value'];
    $transcription = $result['transcription']['value'];
    $collations[$pid]['transcriptions'][$transcription] = $result['transcription_label']['value'];
  }
  return $collations;
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
    'refresh' => array(
      'title' => t('Refresh'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/refresh",
    ),
    'compare' => array(
      'title' => t('Compare'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/compare",
    ),
    'table_view' => array(
      'title' => t('Table'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/compare",
    ),
    'delete' => array(
      'title' => t('Delete'),
      'href' => "islandora/collation/$critical_edition_pid/$collation/delete",
    ),
  );
  return array(
    '#theme' => 'links__ctools_dropbutton',
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
