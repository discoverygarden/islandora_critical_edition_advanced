<?php

/**
 * @file
 * This file contains theming functions for islandora_critical_edition_advanced.
 */

/**
 * Implements hook_preprocess_islandora_critical_edition().
 */
function islandora_critical_edition_preprocess_islandora_critical_apparatus(&$variables) {
  islandora_critical_edition_advanced_prep_tabs();
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  // @TODO add string translation
  $variables['datastreams'] = variable_get('critical_apparatus_streams', islandora_critical_apparatus_get_datastreams());
}

/**
 * Implements hook_preprocess_islandora_critical_edition_container().
 */
function islandora_critical_edition_preprocess_islandora_critical_edition_container(&$variables) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$path/css/critical_edition_container.css");
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $members = islandora_critical_edition_get_members($variables['islandora_object']->id);
  $variables['apparatus'] = array_keys($members, 'islandora:criticalApparatusCModel');
  $variables['versionable_objects'] = array_keys($members, 'islandora:versionableObjectCModel');
}

/**
 * Implements hook_preprocess_islandora_critical_edition_container().
 */
function islandora_critical_edition_preprocess_islandora_transcription_object(&$variables) {
  islandora_critical_edition_advanced_prep_tabs();
  $multiple = FALSE;
  if (isset($variables['transcriptions']) && count($variables['transcriptions']) > 1) {
    $multiple = TRUE;
  }
  $variables['multiple'] = $multiple;
}

/**
 * Implements hook_preprocess_islandora_critical_edition_container().
 */
function islandora_critical_edition_preprocess_islandora_critical_media_object(&$variables) {
  global $base_url;
  module_load_include('inc', 'islandora_critical_edition', 'theme/theme');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/solution_packs');

  $critical_edition_module_path = drupal_get_path('module', 'islandora_critical_edition');
  $validate_path = variable_get('islandora_critical_edition_validate_url', '/validator/validate.html');
  islandora_critical_edition_advanced_add_js();
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

function islandora_critical_edition_advanced_add_js() {
   global $base_url;
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  module_load_include('inc', 'islandora_image_annotation', 'includes/utils');

  // Local Critical Editions module path.
  $critical_edition_module_path = drupal_get_path('module', 'islandora_critical_edition');

  // Islandora Image Annotation base path, required module.
  $image_annotation_module_path = drupal_get_path('module', 'islandora_image_annotation');

  $validate_path = variable_get('islandora_critical_edition_validate_url', '/validator/validate.html');
  // Use the theme files from image annotation for annotation list
  // and image annotation pane's.
  $variables['anno_list_pane'] = theme('islandora_anno_list_pane');
  $variables['anno_img_pane'] = theme('islandora_anno_image_pane');

  // Fill additional session variables.
  $variables['images_path'] = url($critical_edition_module_path . '/CWRC-Writer/src/img/', array('absolute' => TRUE));
  $image_anno_images = $image_annotation_module_path . '/shared_canvas/imgs/';
  drupal_add_js(array('islandora_critical_edition' => array('image_anno_img' => $image_anno_images)), 'setting');

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
        'page_pid' => 'islandora:1098')), 'setting');

  // To avoid hard coded values in js, this is the best option.
  drupal_add_js(array(
      'islandora_critical_edition' => array(
        'module_base' => $critical_edition_module_path)), 'setting');
  drupal_add_js($critical_edition_module_path . "/js/writer_backend_delegate.js");
  drupal_add_js($critical_edition_module_path . '/js/writer_wrapper.js');

  drupal_add_js($critical_edition_module_path . '/CWRC-Writer/src/js/lib/jquery/jquery-1.8.3.js');
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery-ui-1.9.0.custom.min.js");
  islandora_image_annotation_include_common_js();

  drupal_add_css($critical_edition_module_path . "/css/islandora_critical_edition_style.css");
  drupal_add_css($critical_edition_module_path . "/css/style.css");
  drupal_add_js($critical_edition_module_path . "/js/writer_backend_delegate.js");
  drupal_add_js($critical_edition_module_path . '/js/writer_wrapper.js');
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery.layout-latest.min.js");
  drupal_add_css($critical_edition_module_path . "/css/islandora_critical_edition_editor.css");
  drupal_add_css($critical_edition_module_path . "/css/islandora_critical_edition.css");

  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery.layout.resizeTabLayout-1.3.js");

  drupal_add_css($critical_edition_module_path . "/CWRC-Writer/src/smoothness/jquery-ui-1.10.3.custom.min.css");
  islandora_image_annotation_include_common_css();

  drupal_add_js($critical_edition_module_path . '/CWRC-Writer/src/js/lib/jquery/jquery.collapsiblepanel.js');
  drupal_add_css($critical_edition_module_path . '/CWRC-Writer/src/js/lib/jquery/jquery.collapsiblepanel.css');
  drupal_add_css($critical_edition_module_path . "/css/style_2.css");
  drupal_add_css($critical_edition_module_path . "/CWRC-Writer/src/css/tei.css");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery.tmpl.min.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jquery/jquery.watermark.min.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/tinymce/tiny_mce_src.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/tinymce/jquery.tinymce.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/snippet/jquery.snippet.min.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/tinymce/copy_event.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jstree/jquery.hotkeys.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/jstree/jquery.jstree.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/moment/moment.min.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/objtree/ObjTree.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/utilities.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/filemanager.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/entitieslist.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/entities_model.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/relations.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/validation.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/selection.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/delegator.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_addevent.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_addorg.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_addperson.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_addplace.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_citation.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_correction.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_date.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_header.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_keyword.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_link.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_message.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_note.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_search.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_settings.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_title.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_triple.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialogs/dialog_filemanager.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/structuretree.js");

  drupal_add_js($critical_edition_module_path . "/js/dialog_text_image_anno.js");
  
  drupal_add_css($critical_edition_module_path . "/CWRC-Writer/src/css/layout-default-latest.css");
  drupal_add_css($critical_edition_module_path . "/css/image_local.css");

  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/editor.js");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/tagger.js");

  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/dialog.js");

  drupal_add_js($critical_edition_module_path . "/js/startup.js");
  drupal_add_js($critical_edition_module_path . "/js/init.js");
  drupal_add_css($critical_edition_module_path . "/CWRC-Writer/src/js/lib/snippet/jquery.snippet.css");
  drupal_add_js($critical_edition_module_path . "/CWRC-Writer/src/js/lib/tinymce/jquery.tinymce.js");
}