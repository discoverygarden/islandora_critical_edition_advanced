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
}
