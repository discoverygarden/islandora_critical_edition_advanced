<?php
/**
 * @file
 * Template for the islandora critical edition object display.
 */
?>
<?php
if (user_access(ISLANDORA_CRITICAL_EDITION_ADVANCED_MODIFY)) {
  print $edit_link;
}
?>
<div class ="critical_apparatus">
  <?php print drupal_render($apparatus); ?>
</div>
