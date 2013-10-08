<?php
/**
 * @file
 * islandora_critioca;_edition_container.tpl.php
 */
$object = $variables['islandora_object'];
$pid = $object->id;
$apparatus = $variables['apparatus'][0];
$versionable_objects = $variables['apparatus'] + $variables['versionable_objects'];
drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
drupal_set_title($object->label);
?>

<div class="islandora_versionable_objects">
  <?php foreach ($versionable_objects as $versionable): ?>
    <div class="versionable_object">
      <?php
      $versionable_object = islandora_object_load($versionable);
      $source = url("/islandora/object/$versionable/TN/view");
      $path = url("/islandora/object/$versionable");
      $object_url = 'islandora/object/' . $versionable;
      $title = $versionable_object->label;
      $image_variables = array(
        'path' => "$object_url/datastream/TN/view",
        'alt' => $title,
        'title' => $title,
        'attributes' => array('class' => 'versionable_object'),
      );
  
      $thumbnail_img = theme('image', $image_variables);
      $link = l($thumbnail_img, $object_url, array('html' => TRUE, 'attributes' => array('title' => $title, 'alt' => $title)));
      $caption = l($title, $object_url);
      ?>
      <dl class="islandora_critical_edition_object">
        <dt class="islandora_critical_edition_object_thumb"><?php print $link; ?></dt>
        <dd class="islandora_critical_edition_object_caption"><?php print $caption; ?></dd>
      </dl>
    </div>
  <?php endforeach; ?>
</div>