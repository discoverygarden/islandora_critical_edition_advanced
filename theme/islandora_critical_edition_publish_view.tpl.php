<?php
/**
 * @file
 * This is the template file for the Critical Edition published object
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
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jqPagination/js/jquery.jqpagination.js"></script>
<script type="text/javascript">
var $pg = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/jstreeV1_0_2/jquery.jstree.js"></script>
<script type="text/javascript">
var $jt = jQuery.noConflict();
</script>
<script type="text/javascript" src="<?php print $path; ?>/sites/all/modules/islandora_critical_edition_advanced/js/zoom-master/jquery.zoom.js"></script>
<script type="text/javascript">
var $zm = jQuery.noConflict();
</script>
<div id="critical_edition_publish_form_wrapper" data-pid="<?php print $islandora_object->id; ?>">
	<div style="width:100%;float:left;">
	  <?php print $publish_form; ?>
	</div>
</div>
