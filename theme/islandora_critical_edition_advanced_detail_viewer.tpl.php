<?php
/**
 * @file
 * This is the template file for the Critical Edition detail viewer
 * viewer.
 */
?>
<?php if (!$media_viewer): ?>
			<div id="critical-edition-viewer">
				<ul class="workbench_toolbar_lists action_img">
					<li title="Diplomatic Transcriptions" class="work_action_img diplomatic-transcriptions img_selected"></li>
					<li title="Transcription" class="work_action_img transcription"></li>
					<li title="Image" class="work_action_img image"></li>
					<li title="TEI Markup" class="work_action_img tei-markup"></li>
<!-- 					<li title="TEI Markup" class="work_action_img tei-markup"></li> -->
					<li title="Detail Metadata" class="work_action_img detail-meta data_anchor"></li>
					<li title="Show/Hide annotations" data-value="0" class="work_action_img anno-entity-switch switch"></li>
					<li title="Show/Hide Text Image Links" data-value="0" class="work_action_img til-switch switch"></li>
					
					<div id="jqpagination" class="pagination img_pager">
						    <a href="#" class="first" data-action="first">&laquo;</a>
						    <a href="#" class="previous" data-action="previous">&lsaquo;</a>
						    <input id="jqpagination_input" type="text" readonly="readonly" data-max-page="0" />
						    <a href="#" class="next" data-action="next">&rsaquo;</a>
						    <a href="#" class="last" data-action="last">&raquo;</a>
				    	</div>
				</ul>
				
			    <?php if (!$media_viewer): ?>
			    <div id="zoom_wrapper" style="display: none;">
					<img onclick="CriticalEditionViewer.Viewer.zoom_plus_click();" style="float:left;cursor:pointer;cursor:hand;" src="<?php print $module_base;?>/img/zoom_plus.png"/>
					<a id="zoom" onclick="return false;" class="zoom_image" href="#">0</a>
					<img onclick="CriticalEditionViewer.Viewer.zoom_minus_click();"style="float:right;cursor:pointer;cursor:hand;" src="<?php print $module_base;?>/img/zoom_minus.png"/>
				</div>
			    <?php endif; ?>
			</div>
	<?php endif; ?>
<div id="AudioLayout" class="easyui-layout" style="width:100%;height:100%;">
	
	<div id="view_box" style="width:100%;height:100%;overflow:auto;position: relative;">
	<?php if ($media_viewer): ?>
		<?php foreach ($transcription_text as $key => $value):?>
			<div id="versionable_transcription_<?php print $key;?>" class="versionable_transcription_text" style="width:49%;height:100%;float:left;overflow:auto;position: absolute;">
				<h2><?php print $transcription_text[$key]['title'];?></h2>
				<p><pre><?php print $transcription_text[$key]['text'];?></pre></p>
			</div>
		<?php endforeach;?>
		<?php if ($media_viewer): ?>
		  <div id="MediaPlayer" style="width:50%;height:100%;float:right;" data-url="<?php print ($player_params['url']) ?>" data-thumbnail="<?php print ($player_params['tn']) ?>" data-mime="<?php print ($player_params['mime']) ?>">
		    <?php if ($player_ready === FALSE): ?>
		      <img src="<?php print $media_viewer;?>" alt="img" style="width:100%;height:100%;"/>
		    <?php else: ?>
		      <?php print $media_viewer;?>
		    <?php endif;?>
		  </div>
		<?php endif; ?>
	<?php else: ?>
		  <div id="viewer_iframe_border" style="width:100%;height:100%;">
				<div id="loadImg" class="loader-background-image">
					<div style="width:100%;height:100%;background-color:white;">
						<img class="loader-overlay" src="<?php print $module_base;?>/img/engine.png" alt="" />
					</div>
				</div>
		  	<iframe id="viewer_iframe" border=0 name=iframe style="width: 100%;height:100%;" src="/islandora/critical_edition/viewer/trimed/<?php print $islandora_object;?>"></iframe>
		  </div>
	<?php endif;?>
	</div>
		<div id="append_data">
	</div>
</div>
