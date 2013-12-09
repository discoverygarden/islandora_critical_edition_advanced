<?php
/**
 * @file
 * islandora-basic-collection.tpl.php
 */
$objects = array();
if (isset($variables['transcriptions'])) {
  foreach ($variables['transcriptions'] as $transcription) {
    $flat_pid = islandora_escape_pid_for_function($transcription);
    $objects[$flat_pid] = islandora_object_load($transcription);
  }
}
// Fallback if Transcription object is viewed directly.
else {
  $objects[] = $variables['islandora_object'];
}
$object = $objects[key($objects)];
if ($object['TRANSCRIPTION']) {
  $transcription = str_replace("\n", "<br />", $object['TRANSCRIPTION']->content);
}
else{
  $transcription = t("This file has no transcription datastream");
}
module_load_include('inc', 'islandora', 'includes/breadcrumb');
drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
drupal_set_title($object->label);
?>

<?php if (!$variables['multiple']): ?>
  <div class="islandora_transcription_object">
    <?php
    $transcription_object = reset($objects);
    print $transcription;
    ?>
  </div>
<?php else:; ?>
  <div id="tabs">
    <ul>
      <?php foreach ($objects as $flat_pid => $object): ?>
        <li><a href="#<?php print $flat_pid; ?>"><?php print $object->label; ?></a></li>
      <?php endforeach; ?>
    </ul>
    <?php foreach ($objects as $flat_pid => $object): ?>
      <div id="<?php print $flat_pid; ?>">
        <p><?php print str_replace("\n", "<br />", $object['TRANSCRIPTION']->content); ?></p>
      </div>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
