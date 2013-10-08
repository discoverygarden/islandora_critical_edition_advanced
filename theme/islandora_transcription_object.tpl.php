<?php
/**
 * @file
 * islandora-basic-collection.tpl.php
 */
$object = $variables['islandora_object'];
$transcription = $object['TRANSCRIPTION']->content;
module_load_include('inc', 'islandora', 'includes/breadcrumb');
drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
drupal_set_title($object->label);
?>

<div class="islandora_transcription_object">
  <?php if (isset($transcription)): ?>
    <?php print $transcription; ?>
  <?php endif; ?>
</div>
