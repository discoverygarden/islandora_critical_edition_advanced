<?php
/**
 * @file
 * This is the template file for the TEI_RDF object
 *
 * Available variables:
 * - $anno_list_pane (html): The annotation list page,themed by image annotation
 * - $anno_img_pane (html): The image annotation pane,themed by image annotation
 */
?>
<div class="islandora-crited-wrapper">
  <div id="iframe_replacement" class=".iframe_replacement" style="height: 100%">
   <input id="full-window-button" type="button" value="<?php print t('Full Window'); ?>" />
    <div id="cwrc_wrapper" class="cwrc_wrapper islandora-crited-iframe-wrapper" style="width: 100%; height:600px">
      <div id="header" class="ui-layout-north">
        <!-- TODO: We need new full page buttons. So much going on in the header right now -->
        <div id="page_selector">Loading....</div>
        <div id="header-inner">
          <div class="header-nav">
            <a href="" id="page-prev"></a>
            <a href="" id="page-next"></a>
          </div>
        </div>
        <div id ="pageChange"></div>
        <div id="header_label_wrapper">
          <h1>CWRCWriter</h1>
        </div>
        <div id="headerButtons"></div>
      </div>
      <div class="cwrc ui-layout-west ui-widget-content">
        <div id="westTabs" class="tabs">
          <ul>
            <li><a href="#entities">Entities</a></li>
            <li><a href="#structure">Structure</a></li>
            <li><a href="#relations">Relations</a></li>
            <li id="annotation_tab"><a href="#image-annotations">Image Annotations</a></li>
          </ul>
          <div id="westTabsContent" class="ui-layout-content">
            <?php print $anno_list_pane; ?>
          </div>
        </div>
      </div>
      <div id="cwrc_main" class="ui-layout-center">
        <div class="ui-layout-center">
          <form method="post" action="">
            <textarea id="editor" name="editor" class="tinymce"></textarea>
          </form>
        </div>
        <div class="ui-layout-south">
          <div id="southTabs" class="tabs">
            <ul>
              <li><a href="#validation">Validation</a></li>
              <li><a href="#selection">Selection</a></li>
            </ul>
            <div id="southTabsContent" class="ui-layout-content"></div>
          </div>
        </div>
      </div>
      <div id="east_div" class="ui-layout-east">
        <!-- Image annotation -->
        <button id="create_annotation" class="menu_button">Annotate</button>
        <div class="image-annotation-wrapper">

          <!-- Persist a single player and build new interface to it -->
          <div id="canvas-body-wrapper" style="width: 100%; height: 100px;">
            <?php print $islandora_content; ?>
          </div>
        </div>
      </div>

    </div>
  </div>
</div>
