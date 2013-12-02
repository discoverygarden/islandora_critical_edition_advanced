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
			$("#loadImg").css('display','inherit');
		},
		hide_preloader: function() {
			//document.getElementById('loadImg').style.display='none';
			$("#loadImg").css('display','none');
		},
		show_plain_image: function() {
			CriticalEditionViewer.cwrc_writer.layout.close("west");
			CriticalEditionViewer.cwrc_writer.layout.close("north");
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#CriticalEditionViewer').width());
			
			CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
		},
		show_tei_text: function() {
			// TODO:
			if($('navi')) {
				
			}
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).addClass('showStructBrackets');
		},
		show_plain_text: function() {
			// TODO:
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
				$("#navi").animate({
			      marginLeft:0},{
			      complete: function() {
			        //CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($('#cwrc_wrapper', window.frames[0].document).width()-$("#navi").width()));
			        CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
			        if(!$('#img_title').hasClass('img_selected')) {
			        	console.log("resizing west");
			        	CriticalEditionViewer.cwrc_writer.layout.open("west");
			          CriticalEditionViewer.cwrc_writer.layout.sizePane("west", $("#navi").width());
			        } else {
			        	CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($('#cwrc_wrapper', window.frames[0].document).width()-$("#navi").width()));
			        }
			      }
			    }, 700);
				
//				
			} else {
				CriticalEditionViewer.cwrc_writer.layout.close("west");
				
				if($('#img_title').hasClass('img_selected')) {
					CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $('#cwrc_wrapper', window.frames[0].document).width());
				}
				
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
				
				$("#navi").animate({
			        marginLeft:-$("#navi").width()},{
			        complete: function() {
			        	
			        },
			    }, 700);
				
//				CriticalEditionViewer.cwrc_writer.layout.close("west");
//				
			}
		},
		get_entities: function() {
			// Append to block_4_hidden.
			
			
			var cover = '<div id="navi" style="border:1px solid blue;position:absolute;overflow:auto;width:35%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;">test text</div>';
			
			$("#view_box").append(cover);
			//$("#view_box").style('position', 'relative');
			//$("#westTabs", window.frames[0].document).css('position', 'absolute');
			
			var tree_html = '<div id="demo3"><ul><li id="tree_annos"><a href="#">Annotations</a></li><li id="tree_entities"><a href="#">Entities</a></li></ul></div>';
			$('#navi').append(tree_html);
			
			var entity_list = '<ul>';
			$('.entitiesList li', window.frames[0].document).each(function() {
				// If a class is found, its an entity entry.
				if($(this).attr("class")) {
					console.log($(this).attr("class"));
					console.log($(this).children('.entityTitle').first().text());
					entity_list += '<li id="phtml_2">';
					entity_list += '<a href="#">' + $(this).attr("class") + '</a>';
					entity_list += '<ul><li id="' + $(this).attr("name") + '"><a onclick="CriticalEditionViewer.Viewer.entity_click(this);" href="#">' + $(this).children('.entityTitle').first().text() + '</a></li></ul>';
					entity_list += '</li>';
				}
			});
			entity_list += '</ul>';
			$('#tree_entities').append(entity_list);
			
			// Build the annotation list
			var anno_list = '<ul>';
			var anno_arr = new Array();
			$('#comment_annos_block', window.frames[0].document).children().each(function() {
				anno_list += '<li id="annos_' + $(this).find('div[class="islandora_comment_type_title"]').text() + '">';
				anno_list += '<a href="#">' + $(this).find('div[class="islandora_comment_type_title"]').text() + '</a>';
				if(anno_arr[$(this).find('div[class="islandora_comment_type_title"]').text()] == null) {
					anno_arr[$(this).find('div[class="islandora_comment_type_title"]').text()] = new Array();
				}
				var idx = $(this).find('div[class="islandora_comment_type_title"]').text();
				$(this).find('div[class="islandora_comment_type_content"]').children().each(function() {
					anno_arr[idx].push($(this).find('div[class="comment_content"]').text());
					anno_list += '<ul><li id="' + $(this).find('div[class="comment_content"]').text() + '"><a onclick="CriticalEditionViewer.Viewer.annos_click(this);" href="#">' + $(this).find('div[class="comment_content"]').text() + '</a></li></ul>'
				});
				anno_list += '</li>';
			});
			anno_list += '</ul>';
			$('#tree_annos').append(anno_list);
			$("#navi").css("margin-left",-$("#navi").width());
			
//			var cover = '<div id="navi" style="width:100%;height:100%;background-color: #FFFFFF;position: absolute;z-index: 500;left: 0;top: 0;">test text</div>';
//			
//			$("#westTabs", window.frames[0].document).append(cover);
//			$("#westTabs", window.frames[0].document).css('position', 'absolute');
			//console.log($("#navi", window.frames[0].document));
		},
		entity_click: function(e) {
			console.log("clicked");
			console.log(e);
			console.log($(e).closest("li").attr("id"));
			CriticalEditionViewer.cwrc_writer.highlightEntity($(e).closest("li").attr("id"));
			return false;
		},
		annos_click: function(e) {
			console.log("clicked");
			console.log(e);
			console.log($(e).closest("li").attr("id"));
			CriticalEditionViewer.cwrc_writer.highlightEntity($(e).closest("li").attr("id"));
			return false;
		},
		get_image_annotations: function() {
			
		},
		build_tree_view: function() {
			$('#demo3').jstree({
				"plugins" : ["html_data","ui","crrm"],
				"core" : { "initially_open" : [ "phtml_1" ] }
			});
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
						CriticalEditionViewer.Viewer.toggle_text_image_linking($(this).attr("value"));
						break;
					case "til_switch":
						CriticalEditionViewer.Viewer.toggle_text_image_linking($(this).attr("value"));
						break;
				}
			});
			
			$('.pagination').jqPagination({
			    paged: function(page) {
			    	CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
			    	console.log($('#page_choose', window.frames[0].document).val());
			    	$('#page_choose', window.frames[0].document).val(page);
			    	$("#page_choose :selected[true]", window.frames[0].document).attr('selected',false);
			    	$("#page_choose option[value="+page+"]", window.frames[0].document).attr('selected',true);
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
					
					// Build entity/anno tree off of existing.
					CriticalEditionViewer.Viewer.get_entities();
					CriticalEditionViewer.Viewer.build_tree_view();
					
					//$('#entities', window.frames[0].document).addClass("jstree jstree-0 jstree-default jstree-focused");
					
					
					$('#demo3').jstree({
						"plugins" : ["html_data","ui","crrm"],
						"core" : { "initially_open" : [ "phtml_1" ] }
					});
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
//
//$(function () {
//	// TO CREATE AN INSTANCE
//	// select the tree container using jQuery
//	$("#demo1")
//		// call `.jstree` with the options object
//		.jstree({
//			// the `plugins` array allows you to configure the active plugins on this instance
//			"plugins" : ["themes","html_data","ui","crrm","hotkeys"],
//			// each plugin you have included can have its own config object
//			"core" : { "initially_open" : [ "phtml_1" ] }
//			// it makes sense to configure a plugin only if overriding the defaults
//		})
//		// EVENTS
//		// each instance triggers its own events - to process those listen on the container
//		// all events are in the `.jstree` namespace
//		// so listen for `function_name`.`jstree` - you can function names from the docs
//		.bind("loaded.jstree", function (event, data) {
//			// you get two params - event & data - check the core docs for a detailed description
//		});
//	// INSTANCES
//	// 1) you can call most functions just by selecting the container and calling `.jstree("func",`
//	setTimeout(function () { $("#demo1").jstree("set_focus"); }, 500);
//	// with the methods below you can call even private functions (prefixed with `_`)
//	// 2) you can get the focused instance using `$.jstree._focused()`. 
//	setTimeout(function () { $.jstree._focused().select_node("#phtml_1"); }, 1000);
//	// 3) you can use $.jstree._reference - just pass the container, a node inside it, or a selector
//	setTimeout(function () { $.jstree._reference("#phtml_1").close_node("#phtml_1"); }, 1500);
//	// 4) when you are working with an event you can use a shortcut
//	$("#demo1").bind("open_node.jstree", function (e, data) {
//		// data.inst is the instance which triggered this event
//		data.inst.select_node("#phtml_2", true);
//	});
//	setTimeout(function () { $.jstree._reference("#phtml_1").open_node("#phtml_1"); }, 2500);
//});
