$('document').ready(function() {
  $( '#carousel' ).elastislide( {
    orientation : 'vertical',
    minItems: 2,
    onClick : function( el, position, evt ) {
      if($('#aparatusDialog').length == 0) {
        $(document.body).append(''+
          '<div id="aparatusDialog">'+
          '<div id="CriticalEditionViewer"></div>'+
          '</div>');
      }
      console.log($(el).children().first().attr('data-pid'));
      // TODO: get the width/height dynamically.
      $('#aparatusDialog').dialog({
        title: 'Edit Header',
        modal: true,
        resizable: true,
        height: 600,
        width: 1000,
        open: function() {
          // Kick of the logic to fill this.
          $.ajax({
            type: 'POST',
            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/prepare_advanced',
            data:{
              "pid": $(el).children().first().attr('data-pid')
            },
            success: function(data, status, xhr) {
             // console.log(data);
              CriticalEditionViewer.Viewer.build(data);
            },
            error: function(xhRequest, ErrorText, thrownError) {
              console.log(ErrorText + ":" + thrownError);
            },
          });
        },
      });
      return false;
    },
    onReady: function() {
      $('.elasti_img').hover(function() {
        // TODO: need an interactive tool tip perhaps?
        //console.log("hover");
      });
      return false;
    }
  });
  //$( '#elastislide-outer-wrapper' ).css('height',$('#critical_edition_publish_form_wrapper:first').height());
});

var CriticalEditionViewer = {
	cwrc_writer: null,
	cwrc_writer_helper: null,
	cwrc_params: null,
	pager_data: new Array(),
	Viewer: {
		show_preloader: function() {
			
		},
		hide_preloader: function() {
			//document.getElementById('loadImg').style.display='none';
			$("#loadImg").css('display','none');
		},
		show_plain_image: function() {
			CriticalEditionViewer.cwrc_writer.layout.close("west");
			CriticalEditionViewer.cwrc_writer.layout.close("north");
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#CriticalEditionViewer').width());
		},
		show_tei_text: function() {
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).addClass('showStructBrackets');
		},
		show_plain_text: function() {
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showStructBrackets');
		},
		toggle_anno_entities: function(show) {
			if(show == 1) {
				CriticalEditionViewer.cwrc_writer.layout.open("west");
			} else {
				CriticalEditionViewer.cwrc_writer.layout.close("west");
			}
			//$('#cwrc_main .ui-layout-center', window.frames[0].document).append('<div style="float: left;height: 40px;background: #de9a44;margin: 3px;width: 80px;" class="slidedown"></div>')
		},
		toggle_text_image_linking: function(show) {
			if(show == 1) {
				CriticalEditionViewer.cwrc_writer.layout.open("west");
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
			} else {
				CriticalEditionViewer.cwrc_writer.layout.close("west");
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
			}
		},
		build: function(json_html) {
			
			
			if($('#CriticalEditionViewer').length > 0) {
				$('#CriticalEditionViewer').empty();
			}
			$('#CriticalEditionViewer').css("height", "80%")
			$('#CriticalEditionViewer').append(json_html);
			
			console.log($('#view_box').width());
			$('#loadImg div').width($('#view_box').width());
			$('#loadImg div').height($('#view_box').height());
			//$("#viewer_iframe").style(display,'none');
//			  $(window.frames[0].document).ready(function() {
//				  console.log($('#cwrc_wrapper', window.frames[0].document).height());
//			  });
			  
			$('.data_anchor').click(function(e) {
				// TODO: Do something with 'metadata', 'transcript' and 'permalink'.
			});
			
			// Set up img click handlers
			$(".work_action_img").click(function() {
				$(".work_action_img").removeClass("img_selected");
				$(this).addClass("img_selected");
				// Preform the approate action.
				switch ($(this).attr("title")) {
					case "Plain Text":
						CriticalEditionViewer.Viewer.show_plain_text();
						break;
					case "TEI Text":
						CriticalEditionViewer.Viewer.show_tei_text();
						break;
					case "Image":
						CriticalEditionViewer.Viewer.show_plain_image();
						break;
				}
			});
			$("#anno_entity_switch").switchButton();
			$("#til_switch").switchButton();
			
			$(".switch").change(function(e) {
				// Toggling 'checked' property actually shows a 
				// checkbox, which is not what we want.
				if($(this).attr("value") == 1) {
					$(this).attr("value",0);
				} else {
					$(this).attr("value",1);
				}
				switch ($(this).attr("id")) {
					case "anno_entity_switch":
						CriticalEditionViewer.Viewer.toggle_anno_entities($(this).attr("value"));
						break;
					case "til_switch":
						CriticalEditionViewer.Viewer.toggle_text_image_linking($(this).attr("value"));
						break;
				}
			});
			
			$('.pagination').jqPagination({
			    paged: function(page) {
			        // TODO: do something with the page variable
			    	console.log($('#page_choose', window.frames[0].document).val());
			    	$('#page_choose', window.frames[0].document).val(page);
			    	$("#page_choose option[value="+(page-1)+"]", window.frames[0].document).attr('selected',false);
			    	$("#page_choose option[value="+page+"]", window.frames[0].document).attr('selected',true);
			    	//$('#page_choose', window.frames[0].document).trigger('change');
			    	CriticalEditionViewer.cwrc_params.position = $('#page_choose :selected', window.frames[0].document).attr('value');
			          
			    	document.getElementById('viewer_iframe').contentWindow['PID'] = CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position];
			          
			    	CriticalEditionViewer.cwrc_writer_helper.Writer.load_next_anno_page();
			    }
			});
			
			// TODO: hide the writer from view while this is loading
			if($("#viewer_iframe").length > 0) {
				$("#viewer_iframe").load(function (){
					console.log("iframe load complete");
					
					$('#cwrc_wrapper', window.frames[0].document).height($('#view_box').height());
					
					// Set the writer object for access later
					CriticalEditionViewer.cwrc_writer = document.getElementById('viewer_iframe').contentWindow['writer'];
					CriticalEditionViewer.cwrc_writer_helper = document.getElementById('viewer_iframe').contentWindow['islandoraCWRCWriter'];
						
					CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showEntityBrackets');
					
					CriticalEditionViewer.cwrc_params = document.getElementById('viewer_iframe').contentWindow['cwrc_params'];
					
					CriticalEditionViewer.cwrc_writer.pager_data = $('#page_choose', window.frames[0].document);
					
					$('#page_choose option', window.frames[0].document).each(function() {
						CriticalEditionViewer.pager_data[$(this).attr("value") - 1] = $(this).attr("value");
					});
					console.log("pager length: " + CriticalEditionViewer.pager_data.length);
					//$('#jqpagination_input').attr("data-max-page", CriticalEditionViewer.pager_data.length);
					$('.pagination').jqPagination('option', 'max_page', CriticalEditionViewer.pager_data.length);
					
					
					$('#editor_toolbargroup', window.frames[0].document).css("visibility", "hidden");
					$('#editor_toolbargroup', window.frames[0].document).css("display", "none");
					
					$('#editor_toolbargroup', window.frames[0].document).hide();
					
					$('#create_annotation', window.frames[0].document).css("visibility", "hidden");
					$('#create_annotation', window.frames[0].document).css("display", "none");
					CriticalEditionViewer.Viewer.show_plain_image();
					CriticalEditionViewer.Viewer.hide_preloader();
				});
			}
			
			if($('#MediaPlayer').length > 0) {
				console.log("media player");
				jwplayer("mediaplayer").setup({
				    file: $('#MediaPlayer').attr('data-url'),
				    image: $('#MediaPlayer').attr('data-thumbnail'),
				    width: $('#MediaPlayer').attr('data-width'),
				});
			}
		}
	},
};