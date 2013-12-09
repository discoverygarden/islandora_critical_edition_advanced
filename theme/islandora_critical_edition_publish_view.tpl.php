<?php
/**
 * @file
 * This is the template file for the Critical Edition published object
 *
 *<div style="width:25%;float:right;">
 *  <?php print $scroll_list; ?>
 *</div>
 *
 */
?>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition/CWRC-Writer/src/js/lib/jquery/jquery-1.8.3.js"></script>
<script type="text/javascript">
var $jq = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jquery-ui.js"></script>
<script type="text/javascript">
var $ui = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jQuery-switchButton/jquery.switchButton.js"></script>
<script type="text/javascript">
var $sb = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jqPagination/js/jquery.jqpagination.js"></script>
<script type="text/javascript">
var $pg = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jstree/dist/jstree.js"></script>
<script type="text/javascript">
var $jt = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/zoom-master/jquery.zoom.js"></script>
<script type="text/javascript">
var $zm = jQuery.noConflict();
</script>
<?php print "I wrote this thing"; ?>
<div id="critical_edition_publish_form_wrapper" data-pid="<?php print $islandora_object->id; ?>">
	<div style="width:100%;float:left;">
	  <?php print $publish_form; ?>
	</div>
</div>

<!-- drupal_add_js("$module_path/js/jquery-ui.js"); -->
<!-- drupal_add_js("$module_path/js/jQuery-switchButton/jquery.switchButton.js"); -->
<!-- drupal_add_js("$module_path/js/jqPagination/js/jquery.jqpagination.js"); -->
<!-- drupal_add_js("$module_path/js/jstree/dist/jstree.js"); -->