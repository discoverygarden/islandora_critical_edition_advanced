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
  $transcriptions = array_keys($members, 'islandora:transcriptionCModel');
  $output = theme('islandora_transcription_object', array('transcriptions' => $transcriptions));
  return $output;
}
