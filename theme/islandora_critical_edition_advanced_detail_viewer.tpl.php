<?php
/**
 * @file
 * This is the template file for the Critical Edition detail viewer
 * viewer.
 *
 */
?>
<div id="AudioLayout" class="easyui-layout" style="width:100%;height:100%;">
	<div id="view_box_header" style="width:100%;height:30px;border:1px solid red;">
		<div style="float:left;">
			<a class="data_anchor" id ="detail_meta" href="#">Metadata | </a>
		</div>
		<div style="float:left;">
			<a class="data_anchor" style="font-weight:bold;" id="detail_tran" href="#"> Transcript | </a>
		</div>
		<div style="float:left;">
			<a class="data_anchor" id="detail_perm" href="#"> Permalink</a>
		</div>
	</div>
	<?php if (!$player_params || !$media_viewer): ?>
	<div style="width:100%;height:40px;border:1px solid red;">
			<div style="float:left;" class="action_img">
				<img id="tei_plain_text" title="Plain Text" class="work_action_img" style="cursor: pointer;"src="<?php print $module_base;?>/img/text_plain.png" alt="image" />
			</div>
			<div style="float:left;" class="action_img">
				<img id="tei_text_img" title="TEI Text" class="work_action_img" style="cursor: pointer;" src="<?php print $module_base;?>/img/text_tei.png" alt="image" />
			</div>
			<div style="float:left;" class="action_img">
				<img id="img_title" title="Image" class="work_action_img img_selected" style="cursor: pointer;" src="<?php print $module_base;?>/img/picture.png" alt="image" />
			</div>
			<div class="switch-wrapper" style="height:100%;border:1px solid green;float:left">
				<label style="float:left;" for="anno_entity_switch">Annotations and Entitys</label><input class="switch " id="anno_entity_switch" type="checkbox" value="0">
			</div>
			<div class="switch-wrapper" style="height:100%;border:1px solid green;float:left">
				<label style="float:left;" for="til_switch">Text-Image Linking</label><input class="switch" id="til_switch" type="checkbox" value="0">
			</div>
			<div id="jqpagination" class="pagination img_pager">
			    <a href="#" class="first" data-action="first">&laquo;</a>
			    <a href="#" class="previous" data-action="previous">&lsaquo;</a>
			    <input id="jqpagination_input" type="text" readonly="readonly" data-max-page="0" />
			    <a href="#" class="next" data-action="next">&rsaquo;</a>
			    <a href="#" class="last" data-action="last">&raquo;</a>
		    </div>
	</div>
	<?php endif; ?>
	<div id="view_box" style="width:100%;height:100%;border:1px solid green;overflow:auto;position: relative;">
	<?php if ($player_params || $media_viewer): ?>
		<?php foreach ($transcription_text as $key => $value):?>
			<div id="versionable_transcription_<?php print $key;?>" class="versionable_transcription_text" style="width:49%;height:100%;border:1px solid blue;float:left;overflow:auto;position: absolute;">
				<h2><?php print $transcription_text[$key]['title'];?></h2>
				<p><pre><?php print $transcription_text[$key]['text'];?></pre></p>
			</div>
		<?php endforeach;?>
		<?php if ($player_params): ?>
		  <div id="MediaPlayer" style="width:50%;height:100%;border:1px solid blue;float:right;"
			data-url="<?php print $player_params['url'];?>" 
			data-mimetype="<?php print $player_params['mimetype'];?>" 
			data-thumbnail="<?php print $player_params['thumbnail'];?>" 
			data-width="<?php print $player_params['width'];?>" 
			data-height="<?php print $player_params['height'];?>" >
			<?php print $media_viewer;?>
		  </div>
		<?php endif; ?>
		<?php if ($media_viewer): ?>
		  <div id="MediaPlayer" style="width:50%;height:100%;border:1px solid blue;float:right;">
		    <img style="width: 100%;height:100%" class="associated_tn_img" src="<?php print $media_viewer;?>
		  </div>
		<?php endif; ?>
	<?php else: ?>
		  <div id="viewer_iframe_border" style="width:100%;height:100%;border:1px solid red;">
				<div id="loadImg" class="loader-background-image">
					<div style="width:100%;height:100%;border:1px solid red;background-color:white;">
						<img class="loader-overlay" src="<?php print $module_base;?>/img/engine.png" alt="" />
					</div>
				</div>
		  	<iframe id="viewer_iframe" border=0 name=iframe style="width: 100%;height:100%;" src="/islandora/critical_edition/viewer/trimed/<?php print $islandora_object;?>"></iframe>
		  </div>
		  
	<?php endif;?>
	</div>
	<?php 
	print '<pre>';
  var_dump(get_defined_vars());
print '</pre>';
?>
</div>