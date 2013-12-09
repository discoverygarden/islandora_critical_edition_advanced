<?php
/**
 * @file
 * islandora_versionable_object.tpl.php
 */
$object = $variables['islandora_object'];
$members = $variables['members'];
$critical_edition = array_search('islandora:criticalEditionCModel', $members);
$edition_object = islandora_object_load($critical_edition);
$transcriptions = array_keys($members, 'islandora:transcriptionCModel');
drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
drupal_set_title($object->label);
?>
<p>ISLANDORA VERSIONABLE OBJECT TEST.  FROM THE .tpl FILE</p>
<div class="islandora_tei_editor_object">
  <?php if (!$critical_edition  && empty($transcriptions)): ?>
    <?php print t("This Versionable Object has no associated objects."); ?>
  <?php endif; ?>
  <?php if ($critical_edition): ?>
    <?php print l($edition_object->label, "islandora/object/${critical_edition}"); ?>
  <?php endif; ?>
</div>

<div class ="islandora_critical_transcriptions" >
  <?php foreach ($transcriptions as $transcription): ?>
    <?php
    $transcription_object = islandora_object_load($transcription);
    print l($transcription_object->label, "islandora/object/$transcription_object");
    print "<br />";
    ?>
  <?php endforeach; ?>
</div>
