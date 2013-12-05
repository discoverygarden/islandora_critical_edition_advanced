<?php
/**
 * @file
 * Template for the islandora critical edition object display.
 */
// @todo Render out the critical apparatus object view.
?>
<?php print drupal_render($apparatus); ?>
<div class="edition-container">
  <?php print drupal_render($version_actions); ?>
  <?php print drupal_render($edition_actions); ?>
</div>
<?php print drupal_render($versions); ?>
<?php print drupal_render($collation_actions); ?>
<?php print drupal_render($collations); ?>
