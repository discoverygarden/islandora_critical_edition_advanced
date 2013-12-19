<?php
/**
 * @file
 * Template for the islandora critical edition object display.
 */
// @todo Render out the critical apparatus object view.
?>
<div id="workbench_toolbar">
  <?php print "$view_link"; ?>
  <?php print drupal_render($apparatus_link); ?>
  <?php print drupal_render($edition_actions); ?>
  <?php print drupal_render($toggle_workbench_membership_form); ?>
</div>

<div class="edition-container">
  <div class ="version_container">
    <?php print drupal_render($versions); ?>
  </div>
  <div class ="collation_container">
    <?php print drupal_render($collation_actions); ?>
    <?php print drupal_render($collations); ?>
  </div>
</div>
