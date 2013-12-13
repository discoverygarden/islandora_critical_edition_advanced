<?php
/**
 * @file
 * This is the template file for the Critical Edition elastislide object viewer.
 */
?>

<div class="main clearfix">
	<div class="column">
		<!-- Elastislide Carousel -->
		<ul id="carousel" class="elastislide-list">
			<?php foreach ($image_data as $img_item): ?>
      			<li><img data-pid="<?php print $img_item['pid'];?>" class="elasti_img" style="cursor: pointer;" src="<?php print $img_item['TN'];?>" alt="image" /></li>
    		<?php endforeach; ?>
		</ul>
	</div>
</div>
