<?php
/**
 * @file
 * Function to add thumbnail to critical edition.
 */

/**
 * Adds randomized thumbnail to Critical Edition.
 *
 * @param array $form_state
 *   Drupal from state
 */
function islandora_critical_edition_advanced_add_thumbnail($form_state) {
  $path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  $fedora_object = $form_state['islandora']['objects'][0];
  $exclude_list = array(".", "..", ".DS_Store");
  $image_dir = "$path/thumbnails/";
  if (!is_dir($image_dir)) {
    return;
  }
  $images = array_values(array_diff(scandir($image_dir), $exclude_list));
  $count = count($images);
  if ($count == 0) {
    return;
  }
  $range = count($images) - 1;
  $thumbnail = $image_dir . $images[mt_rand(0, $range)];
  $tn = $fedora_object->constructDatastream('TN');
  $tn->label = 'Thumbnail';
  $tn->mimetype = 'image/jpg';
  $tn->setContentFromFile($thumbnail);
  $fedora_object->ingestDatastream($tn);
}
