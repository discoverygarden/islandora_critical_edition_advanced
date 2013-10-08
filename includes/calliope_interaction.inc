<?php

/**
 * @file
 * Holds calliope interactions for islandora_critical_editions.
 */

/**
 * Creates MVD from Versionable Object.
 *
 * @param AbstractObject $critical_edition_container_object
 *   Object representing Critical Edition object
 *
 * @return String
 *   Success/failure message
 */
function islandora_critical_create_mvd($critical_edition_container_object) {
  module_load_include('inc', 'islandora_calliope', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $versionable_objects = array_keys(islandora_critical_edition_get_members($critical_edition_container_object->id, 'islandora:versionableObjectCModel'));
  $transcriptions = array();
  foreach ($versionable_objects as $versionable_object) {
    $transcriptions = array_merge($transcriptions, array_keys(islandora_critical_get_related_transcriptions($versionable_object)));
  }
  $success = islandora_calliope_create_mvd($critical_edition_container_object->id, $transcriptions);
  drupal_set_message($success);
  drupal_goto('islandora/object/' . $critical_edition_container_object->id);
}
