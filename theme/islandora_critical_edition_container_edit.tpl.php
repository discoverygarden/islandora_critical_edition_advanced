<?php
/**
 * @file
 * Template for the islandora critical edition object display.
 */
// @todo Render out the critical apparatus object view.
?>
<?php print "$view_link"; ?>
<?php print drupal_render($apparatus_link); ?>

<div class="edition-container">
  <?php print drupal_render($version_actions); ?>
</div>
<?php print drupal_render($versions); ?>
<?php print drupal_render($collation_actions); ?>
<?php print drupal_render($collations); ?>
