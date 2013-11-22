<?php
/**
 * @file
 * This is the template file for the Critical Edition detail viewer
 * viewer.
 *
 */
?>
<div id="AudioLayout" class="easyui-layout" style="width:100%;height:100%;">
	<div style="width:100%;height:30px;border:1px solid red;">
		<div style="float:left;">
			<a href="#">Metadata |</a>
		</div>
		<div style="float:left;">
			<a href="#"> Transcript |</a>
		</div>
		<div style="float:left;">
			<a href="#"> Permalink</a>
		</div>
	</div>
	<div style="width:100%;height:400px;border:1px solid green;">
		<div style="width:49%;height:100%;border:1px solid blue;float:left;">
		<p></p>
		</div>
		<div id="MediaPlayer" style="width:50%;height:100%;border:1px solid blue;float:right;"
			data-url="<?php print $player_params['url'];?>" 
			data-mimetype="<?php print $player_params['mimetype'];?>" 
			data-thumbnail="<?php print $player_params['thumbnail'];?>" 
			data-width="<?php print $player_params['width'];?>" 
			data-height="<?php print $player_params['height'];?>" >
			<?php print $media_viewer;?>
		</div>
	</div>
	<div style="width:100%;height:40px;border:1px solid red;">
	four
	</div>
</div>