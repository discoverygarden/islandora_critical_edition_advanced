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

function islandora_critical_edition_mvd_display($critical_edition_object) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/calliope');
  module_load_include('module', 'collationtools');
  $mvds = islandora_calliope_find_mvds($critical_edition_object);
  $output = "";
  foreach ($mvds as $mvd) {
    $left = $mvd['resources'][0]['name'];
    $right = $mvd['resources'][1]['name'];
    $output .= collationtools_tableapparatus();
  }
  return $output;
}
