<?php

/**
 * @file
 * Display for critical edition.
 */

/**
 * Sets up transcription tab for versionable object display.
 *
 * @param AbstractObject $object
 *   Object representing Versionable Object
 *
 * @return String
 *   Formatted output for display
 */
function islandora_critical_edition_display($object) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $members = islandora_critical_edition_get_members($object->id);
  $transcription = array_search('islandora:transcriptionCModel', $members);
  $transcription_object = islandora_object_load($transcription);
  $output = theme('islandora_transcription_object', array('islandora_object' => $transcription_object));
  return $output;
}
