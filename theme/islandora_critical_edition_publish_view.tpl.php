<?php
/**
 * @file
 * This is the template file for the Critical Edition published object
 *
 */
?>
<?php print "I wrote this thing"; ?>
<div id="critical_edition_publish_form_wrapper" data-pid="<?php print $islandora_object->id; ?>">
	<div style="width:75%;float:left;">
	  <?php print $publish_form; ?>
	</div>
	<div style="width:25%;float:right;">
	  <?php print $scroll_list; ?>
	</div>
</div>