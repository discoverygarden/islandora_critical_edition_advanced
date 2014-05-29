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
  $exclude_list = array(".", "..", ".DS_Store", "scaled");
  $image_dir = "$path/thumbnails";
  $thumbnail_size = variable_get('critical_edition_thumbnail_size', 175);
  if (!is_dir($image_dir)) {
    return;
  }
  if (!is_dir("$image_dir/scaled")) {
    mkdir("$image_dir/scaled");
  }
  $images = array_values(array_diff(scandir($image_dir), $exclude_list));
  $count = count($images);
  if ($count == 0) {
    return;
  }
  $range = count($images) - 1;
  $image = $images[mt_rand(0, $range)];
  $thumbnail = "$image_dir/scaled/{$thumbnail_size}_{$image}";
  if (!is_file("$image_dir/scaled/{$thumbnail_size}_{$image}")) {
    $success = islandora_critical_edition_advanced_create_thumbnail("$image_dir", $image, $thumbnail_size);
    if (!$success) {
      $thumbnail = "$image_dir/$image";
    }
  }

  $tn = $fedora_object->constructDatastream('TN');
  $tn->label = 'Thumbnail';
  $tn->mimetype = 'image/jpg';
  $tn->setContentFromFile($thumbnail);
  $fedora_object->ingestDatastream($tn);
}

/**
 * Creates and stores thumbnailed image.
 *
 * @param string $file_dir
 *   Directory holding original image
 * @param string $image
 *   Filename of image being thumbnailed
 *
 * @return bool
 *   Success status
 */
function islandora_critical_edition_advanced_create_thumbnail($file_dir, $image, $thumbnail_size) {
  $imagick = new Imagick("$file_dir/$image");
  $imagick->thumbnailimage($thumbnail_size, NULL);
  $imagick->setimagefilename("$file_dir/scaled/{$thumbnail_size}_{$image}");
  return $imagick->writeimage();
}
