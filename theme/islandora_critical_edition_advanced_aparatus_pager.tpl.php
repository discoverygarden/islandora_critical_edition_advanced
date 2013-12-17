<?php
/**
 * @file
 * This is the template file for the Critical Edition elastislide object viewer.
 */
?>
<div id="aparatus_pager">
	<?php if ($tab_previous != "_blank_"): ?>
	<div id="aparatus_tabber_previous" class="aparatus_tab_previous"><a href="#" id="aparatus_previous_anchor"><strong><?php print $tab_previous;?></strong></a></div>
	<?php endif;?>
	<?php if ($tab_next != "_blank_"): ?>
	<div id="aparatus_tabber_next" class="aparatus_tab_next"><a href="#" id="aparatus_next_anchor"><strong><?php print $tab_next;?></strong></a></div>
	<?php endif;?>
</div>
