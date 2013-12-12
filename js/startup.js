
(function($) {
$('document').ready(function() {
	$jq( '#versions-tab tbody').find('td[class="version_name"]').each(function(e) {
		$jq(this).children(':first').click(function(e) {
			e.preventDefault();
			  if($jq('#aparatusDialog').length == 0) {
				  $jq(document.body).append(''+
		          '<div id="aparatusDialog">'+
		          '<div id="CriticalEditionViewer"></div>'+
		          '</div>');
		      }
			  var clicked_item = this;
			  var pid = $(this).attr('data-pid');
			  var dialog_title = $jq(this).text();
//		      // TODO: get the width/height dynamically.
			  $ui('#aparatusDialog').dialog({
		        title: dialog_title,
		        modal: true,
		        resizable: true,
		        height: 600,
		        width: 1000,
		        open: function(e) {
		          console.log(pid);
		          // Kick of the logic to fill this.
		          $jq.ajax({
		            type: 'POST',
		            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/prepare_advanced/' + pid,
		            data:{
		              "pid": pid,
		            },
		            success: function(data, status, xhr) {
		             // console.log(data);
		              CriticalEditionViewer.data_pid = pid;
		              CriticalEditionViewer.Viewer.build(data);
		            },
		            error: function(xhRequest, ErrorText, thrownError) {
		              console.log(ErrorText + ":" + thrownError);
		            },
		          });
		        },
		      });
			return false;
		});
	  });
});
	
})(jQuery);


var CriticalEditionViewer = {
	cwrc_writer: null,
	cwrc_writer_helper: null,
	cwrc_params: null,
	data_pid: null,
	transformed_data: null,
	current_zoom: 0,
	pager_data: new Array(),
	Viewer: {
		get_page_transformed_tei: function(page) {
			$jq.ajax({
	            type: 'POST',
	            async: false,
	            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/transformed_page/' + page,
	            data:{
		          "versionable_object": CriticalEditionViewer.data_pid,
		        },
	            success: function(data, status, xhr) {
	              CriticalEditionViewer.Viewer.transformed_data = data;
	              //CriticalEditionViewer.Viewer.remove_translated_tei();
	              CriticalEditionViewer.Viewer.show_versionable_transcriptions();
	            },
	            error: function(xhRequest, ErrorText, thrownError) {
	              console.log(ErrorText + ":" + thrownError);
	            },
	          });
		},
		show_preloader: function() {
			$jq("#loadImg").css('z-index','800');
			$jq("#loadImg").css('display','inherit');
		},
		hide_preloader: function() {
			//document.getElementById('loadImg').style.display='none';
			$jq("#loadImg").css('display','none');
		},
		remove_translated_tei: function(){
			$jq('#translated_tei', window.frames[0].document).remove();
			//CriticalEditionViewer.Viewer.transformed_data = '<div></div>';
		},
		show_plain_image: function() {
			CriticalEditionViewer.Viewer.remove_translated_tei();
			CriticalEditionViewer.cwrc_writer.layout.close("west");
			CriticalEditionViewer.cwrc_writer.layout.close("north");
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width());
			CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
		},
		show_versionable_transcriptions: function() {
			if($jq('#translated_tei', window.frames[0].document).length == 0) {
				console.log("should be added...")
				$jq('#cwrc_main', window.frames[0].document).append('<div class="show_tei"id="translated_tei" style="width:100%;height:100%;position:absolute;top:0px;z-index:300;background-color:white"></div>');
				
				CriticalEditionViewer.Viewer.get_page_transformed_tei(CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position]);
				
				$jq('#translated_tei', window.frames[0].document).append(CriticalEditionViewer.Viewer.transformed_data);
				CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			}
		},
		show_tei_text: function() {
			CriticalEditionViewer.Viewer.remove_translated_tei();
			// TODO:
			if($jq('navi')) {
				
			}
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).addClass('showStructBrackets');
		},
		show_plain_text: function() {
			CriticalEditionViewer.Viewer.remove_translated_tei();
			// TODO:
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
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
				$jq("#navi").animate({
			      marginLeft:0},{
			      complete: function() {
			        //CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($('#cwrc_wrapper', window.frames[0].document).width()-$("#navi").width()));
			        CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
			        if(!$jq('#img_title').hasClass('img_selected')) {
			        	console.log("resizing west");
			        	CriticalEditionViewer.cwrc_writer.layout.open("west");
			          CriticalEditionViewer.cwrc_writer.layout.sizePane("west", $jq("#navi").width());
			        } else {
			        	CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($jq('#cwrc_wrapper', window.frames[0].document).width()-$jq("#navi").width()));
			        }
			      }
			    }, 700);
				
//				
			} else {
				CriticalEditionViewer.cwrc_writer.layout.close("west");
				
				if($jq('#img_title').hasClass('img_selected')) {
					CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#cwrc_wrapper', window.frames[0].document).width());
				}
				
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
				
				$jq("#navi").animate({
			        marginLeft:-$jq("#navi").width()},{
			        complete: function() {
			        	
			        },
			    }, 700);
				
			}
		},
		get_entities: function() {
			// Append to block_4_hidden.
			var cover = '<div id="navi" style="position:absolute;overflow:auto;width:35%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;"></div>';
			
			$jq("#view_box").append(cover);
			
			var tree_html = '<div id="demo3"><ul><li id="tree_annos"><a href="#">Annotations</a></li><li id="tree_entities"><a href="#">Entities</a></li></ul></div>';
			$jq('#navi').append(tree_html);
			
			var entity_list = '<ul>';
			var text_image_list = '<ul><li id="publish_txtimglnk_list"><a href="#">' + 'Text Image Links' + '</a><ul>';
			
			
			$jq('.entitiesList li', window.frames[0].document).each(function() {
				// If a class is found, its an entity entry.
				if($jq(this).attr("class")) {
					if($jq(this).attr("class") == "txtimglnk") {
						var inner_data = $jq(this).find('div[class="info"]').children().last().children().last().text();
						var clean_id= inner_data.replace("uuid: ", "");
						if(inner_data == "") {
							inner_data = $jq(this).attr("name");
						}
						text_image_list += '<li id="' + inner_data + '"><a href="#">' + $jq(this).children('.entityTitle').first().text() + '</a></li>';
					} else {
						entity_list += '<li id="publish_entities_list">';
						entity_list += '<a href="#">' + $jq(this).attr("class") + '</a>';
						entity_list += '<ul><li id="' + $jq(this).attr("name") + '"><a href="#">' + $jq(this).children('.entityTitle').first().text() + '</a></li></ul>';
						entity_list += '</li>';
					}
				}
			});
			
			
			text_image_list += '</ul></li></ul>';
			entity_list += '</ul>';
			if(entity_list != '<ul>') {
				$jq('#demo3').append(text_image_list);
			}
			$jq('#tree_entities').append(entity_list);
			
			// Build the annotation list
			var anno_list = '<ul>';
			var anno_arr = new Array();
			$jq('#comment_annos_block', window.frames[0].document).children().each(function() {
				anno_list += '<li id="annos_' + $jq(this).find('div[class="islandora_comment_type_title"]').text() + '">';
				anno_list += '<a href="#">' + $jq(this).find('div[class="islandora_comment_type_title"]').text() + '</a>';
				if(anno_arr[$jq(this).find('div[class="islandora_comment_type_title"]').text()] == null) {
					anno_arr[$jq(this).find('div[class="islandora_comment_type_title"]').text()] = new Array();
				}
				var idx = $jq(this).find('div[class="islandora_comment_type_title"]').text();
				$jq(this).find('div[class="islandora_comment_type_content"]').children().each(function() {
					anno_arr[idx].push($jq(this).find('div[class="comment_content"]').text());
					anno_list += '<ul><li id="' + $jq(this).attr('urn') + '"><a href="#">' + $jq(this).find('div[class="comment_content"]').text() + '</a></li></ul>'
				});
				anno_list += '</li>';
			});
			anno_list += '</ul>';
			$jq('#tree_annos').append(anno_list);
			$jq("#navi").css("margin-left",-$jq("#navi").width());
		},
		entity_click: function(e) {
			CriticalEditionViewer.cwrc_writer.highlightEntity($jq(e).closest("li").attr("id"));
			return false;
		},
		annos_click: function(e) {
			CriticalEditionViewer.cwrc_writer.highlightEntity(e);
			return false;
		},
		get_image_annotations: function() {
			
		},
		build_tree_view: function() {
			$jt('#demo3').jstree({
				"checkbox": {
		              real_checkboxes: false,
		              two_state: true
		           },
				"plugins" : ["themes", "ui", "checkbox"]
			}).bind('select_node.jstree', function (e, data) {
				// Hate this, but this version of jstree kinda
				// requires it.
				
				//$jq('#translated_tei', window.frames[0].document).find('span[data-cref="224568"]').css("background-color", "#FFFF00");
				
				var data_stuff = $jq('.jstree-clicked');
				
				// The following highlights entity's
				for(var i = 0;i<data_stuff.length;i++) {
					if($jq(data_stuff[i]).closest("li").attr("id")) {
						var li_id = $jq(data_stuff[i]).closest("li").attr("id");
						if(li_id.indexOf("ent_") !== -1) {
							CriticalEditionViewer.Viewer.annos_click(li_id);
						}
					}
				}
				for(var x = 0;x<data_stuff.length;x++) {
					if($jq(data_stuff[x]).closest("li").attr("id")) {
						var uuid = $jq(data_stuff[x]).closest("li").attr("id");
						if(uuid.indexOf("annos_") === -1) {
							var trimmed_uuid = uuid.replace("uuid: ", "");
							// Show the annotation.console.log(document.getElementById('viewer_iframe').contentWindow);
							document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + trimmed_uuid, window.frames[0].document), 'canvas_0', trimmed_uuid, "TextImageLink");
							if($jq('#translated_tei', window.frames[0].document).length > 0) {
								$jq('#translated_tei', window.frames[0].document).find('span[data-cref="' + trimmed_uuid + '"]').css("background-color", "#FFFF00");
							}
							
						}
					}
				}
				
				//console.log(data_stuff);
			}).bind('deselect_node.jstree', function() {
				//console.log("deselect");
				var data_stuff = $jq('.jstree-anchor');
				
				$jq('#entities > ul > li', window.frames[0].document).each(function(index, el) {
					$jq(this).removeClass('selected').css('background-color', '').find('div[class="info"]').hide();
					CriticalEditionViewer.cwrc_writer.delegator.editorCallback('highlightEntity_looseFocus', $jq(this));
				});
				
				for(var x = 0;x<data_stuff.length;x++) {
					if($jq(data_stuff[x]).closest("li").attr("id")) {
						var uuid = $jq(data_stuff[x]).closest("li").attr("id");
						if(uuid.indexOf("annos_") === -1) {
							// Show the annotation.console.log(document.getElementById('viewer_iframe').contentWindow);
							var trimmed_uuid = uuid.replace("uuid: ", "");
							$jq('.svg_' + trimmed_uuid, window.frames[0].document).remove();
							$jq('#translated_tei', window.frames[0].document).find('span[data-cref="' + trimmed_uuid + '"]').css("background-color", "");
						}
					}
				}
				
			});
		},
		show_versionable_meta: function() {
			$jq("#meta_overlay").animate({
			      marginTop:0},{
			      complete: function() {
			    	  $jq.ajax({
				            type: 'POST',
				            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/detail_meta_data',
				            data:{
				              "pid": CriticalEditionViewer.data_pid,
				            },
				            success: function(data, status, xhr) {
				              $jq("#meta_overlay").append(data);
				              // Expand the details
				              $jq("#meta_overlay").find('fieldset[class="islandora islandora-metadata collapsible collapsed"]').removeClass('collapsed');
				              CriticalEditionViewer.Viewer.hide_preloader();
				            },
				            error: function(xhRequest, ErrorText, thrownError) {
				              CriticalEditionViewer.Viewer.hide_preloader();
				            },
				       });
			      }
			    }, 700);
		},
		show_versionable_transcription: function() {
			$jq("#meta_overlay").animate({
			      marginTop:-$jq("#meta_overlay").height()},{
			      complete: function() {
			      }
			    }, 700);
		},
		show_versionable_permalink: function() {
			// The current menu path is the permalink...
			location.reload();
		},
		zoom_plus_click: function() {
			//console.log("plus clicked");
			CriticalEditionViewer.current_zoom = CriticalEditionViewer.current_zoom + 5;
			CriticalEditionViewer.Viewer.zoom_level_update();
		},
		zoom_minus_click: function() {
			//console.log("minus clicked");
			if(CriticalEditionViewer.current_zoom - 5 >= 0) {
				CriticalEditionViewer.current_zoom = CriticalEditionViewer.current_zoom - 5;
				CriticalEditionViewer.Viewer.zoom_level_update();
			}
			
		},
		zoom_level_update: function() {
			$jq('#zoom').text(CriticalEditionViewer.current_zoom);
			//console.log($jq('#zoom').text());
			// Need to destroy it every time.
			CriticalEditionViewer.Viewer.destroy_zoom();
			var img = $jq('#annotations', window.frames[0].document).find('div[class="base_img"]').children(0);
			$zm(img).parent()
			    .zoom({
			      magnify: CriticalEditionViewer.current_zoom
			    });
		},
		build_zoom: function() {
			$jq('#viewer_iframe_border').append(
					'<div id="zoom_wrapper">'+
						'<img onclick="CriticalEditionViewer.Viewer.zoom_plus_click();" style="float:left;cursor:pointer;cursor:hand;" src="' + Drupal.settings.basePath + Drupal.settings.islandora_critical_edition_advanced.module_base+'/img/zoom_plus.png"/>'+
						'<a id="zoom" onclick="return false;" class="zoom_image" href="#">0</a>'+
						'<img onclick="CriticalEditionViewer.Viewer.zoom_minus_click();"style="float:right;cursor:pointer;cursor:hand;" src="' + Drupal.settings.basePath + Drupal.settings.islandora_critical_edition_advanced.module_base+'/img/zoom_minus.png"/>'+
					'</div>');
			
			//$jq("#zoom_wrapper").css("border","1px solid red");
			$jq("#zoom_wrapper").css("margin-right","5px");
			$jq("#zoom_wrapper").css("float","right");
			$jq("#zoom_wrapper").css("text-align","center");
			$jq("#zoom_wrapper").css("position","absolute");
			$jq("#zoom_wrapper").css("right","5px");
			$jq("#zoom_wrapper").css("top","5px");
			$jq("#zoom_wrapper").css("z-index","30");
			
			$jq("#zoom_wrapper").css("width","150px");
			$jq("#zoom_wrapper").css("z-index","30");
			CriticalEditionViewer.Viewer.zoom_level_update();
			
		},
		destroy_zoom: function() {
			var img = $jq('#annotations', window.frames[0].document).find('div[class="base_img"]').children(0);
			$zm(img).trigger('zoom.destroy');
		},
		add_transcription_pager: function() {
			var pager = 
				'<div id="tr_pagination" class="pagination img_pager">' +
				    '<a href="#" class="first" data-action="first">&laquo;</a>'+
				    '<a href="#" class="previous" data-action="previous">&lsaquo;</a>'+
				    '<input id="tr_pagination_input" type="text" readonly="readonly" data-max-page="0" />'+
				    '<a href="#" class="next" data-action="next">&rsaquo;</a>'+
				    '<a href="#" class="last" data-action="last">&raquo;</a>'+
			    '</div>';
			$jq("#view_box_header").append(pager);
			$pg('#tr_pagination').jqPagination({
				max_page: $jq(".versionable_transcription_text").length,
			    paged: function(page) {
			    	$jq(".versionable_transcription_text").css("visibility", "hidden");
			    	$jq(".versionable_transcription_text").css("display", "none");
			    	$jq("#versionable_transcription_" + (page - 1)).css("visibility", "visible");
			    	$jq("#versionable_transcription_" + (page - 1)).css("display", "block");
			    }
			});
		},
		build: function(json_html) {
			
			
			if($jq('#CriticalEditionViewer').length > 0) {
				$jq('#CriticalEditionViewer').empty();
			}
			$jq('#CriticalEditionViewer').css("height", "80%")
			$jq('#CriticalEditionViewer').append(json_html);
			
			// Hide all transcriptions, showing only the first.
			$jq(".versionable_transcription_text").toggle();
			if($jq("#versionable_transcription_0").length > 0) {
				$jq("#versionable_transcription_0").toggle();
				
				// Now, set up the pager for the transcriptions.
				CriticalEditionViewer.Viewer.add_transcription_pager();
			}
			
			
			$jq('#loadImg div').width($jq('#view_box').width());
			$jq('#loadImg div').height($jq('#view_box').height());
			$jq('.data_anchor').click(function(e) {
				$jq('.data_anchor').css('font-weight', 'normal');
				//console.log($jq(this).css('font-weight') == 'bold');
				if($jq(this).css('font-weight') == 'bold') {
					$jq(this).css('font-weight', 'normal');
				} else {
					$jq(this).css('font-weight', 'bold');
				}
				
				e.preventDefault();
				if($jq("#meta_overlay").length == 0) {
					var meta_cover = '<div id="meta_overlay" style="position:absolute;overflow:auto;width:100%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;"></div>';
					$jq("#view_box").append(meta_cover);
					$jq("#meta_overlay").css("margin-top",-$jq("#meta_overlay").height());
				} else {
					$jq("#meta_overlay").animate({
				        marginTop:-$jq("#meta_overlay").height()},{
				        complete: function() {
				         // console.log("moved up complete");
				          $jq('.data_anchor').css('font-weight', 'normal');
				          $jq('#detail_tran').css('font-weight', 'bold');
				          $jq("#meta_overlay").remove();
				        },
				    }, 700);
				}
				
				switch ($jq(this).attr('id')) {
				case "detail_meta":
					// Show the preloader while we retrieve the data.
					//CriticalEditionViewer.Viewer.show_preloader();
					CriticalEditionViewer.Viewer.show_versionable_meta();
					break;
				case "detail_tran":
					//CriticalEditionViewer.Viewer.show_preloader();
					CriticalEditionViewer.Viewer.show_versionable_transcription();
					break;
				case "detail_perm":
					//CriticalEditionViewer.Viewer.show_preloader();
					CriticalEditionViewer.Viewer.show_versionable_permalink();
					break;
				}
				
			});
			
			// Set up img click handlers
			$jq(".work_action_img").click(function() {
				$jq(".work_action_img").removeClass("img_selected");
				$jq(this).addClass("img_selected");
				// Preform the approate action.
				switch ($jq(this).attr("title")) {
					case "Transcription":
						CriticalEditionViewer.Viewer.show_plain_text();
						break;
					case "TEI Text":
						CriticalEditionViewer.Viewer.show_tei_text();
						break;
					case "Image":
						CriticalEditionViewer.Viewer.show_plain_image();
						break;
					case "Diplomatic Transcriptions":
						CriticalEditionViewer.Viewer.show_versionable_transcriptions();
						break;
				}
			});
			$sb("#anno_entity_switch").switchButton();
			$sb("#til_switch").switchButton();
			
			$jq(".switch").change(function(e) {
				// Toggling 'checked' property actually shows a 
				// checkbox, which is not what we want.
				if($jq(this).attr("value") == 1) {
					$jq(this).attr("value",0);
				} else {
					$jq(this).attr("value",1);
				}
				switch ($jq(this).attr("id")) {
					case "anno_entity_switch":
						//$jq("#publish_txtimglnk_list").toggle();
						CriticalEditionViewer.Viewer.toggle_text_image_linking($jq(this).attr("value"));
						break;
					case "til_switch":
//						$jq("#publish_txtimglnk_list").toggle();
//						$jq("#tree_annos").toggle();
//						$jq("#tree_entities").toggle();
						CriticalEditionViewer.Viewer.toggle_text_image_linking($jq(this).attr("value"));
						break;
				}
			});
			
			if($jq("#viewer_iframe").length > 0) {
				$jq("#viewer_iframe").load(function (){
					//console.log("iframe load complete");
					
					$jq('#cwrc_wrapper', window.frames[0].document).height($jq('#view_box').height());
					// Set the writer object for access later
					CriticalEditionViewer.cwrc_writer = document.getElementById('viewer_iframe').contentWindow['writer'];
					console.log("writer");
					
					CriticalEditionViewer.cwrc_writer.layout.north.options.resizeable = false;
					console.log(CriticalEditionViewer.cwrc_writer);
					
					
					
					
					CriticalEditionViewer.cwrc_writer_helper = document.getElementById('viewer_iframe').contentWindow['islandoraCWRCWriter'];
						
					CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showEntityBrackets');
					
					CriticalEditionViewer.cwrc_params = document.getElementById('viewer_iframe').contentWindow['cwrc_params'];
					
					CriticalEditionViewer.cwrc_writer.pager_data = $jq('#page_choose', window.frames[0].document);
					
					$jq('#page_choose option', window.frames[0].document).each(function() {
						CriticalEditionViewer.pager_data[$jq(this).attr("value") - 1] = $jq(this).attr("value");
					});
					CriticalEditionViewer.Viewer.get_page_transformed_tei(CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position]);
					
					// Hide the resize bar buttons in the UI-Layout plugin.
					$jq('.ui-layout-toggler', window.frames[0].document).css("visibility", "hidden");
					
					$pg('.pagination').jqPagination({
						max_page: CriticalEditionViewer.pager_data.length,
					    paged: function(page) {
					    	CriticalEditionViewer.Viewer.remove_translated_tei();
					    	
					    	CriticalEditionViewer.Viewer.show_preloader();
					    	CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
					    	//console.log($jq('#page_choose', window.frames[0].document).val());
					    	$jq('#page_choose', window.frames[0].document).val(page);
					    	$jq("#page_choose :selected[true]", window.frames[0].document).attr('selected',false);
					    	$jq("#page_choose option[value="+page+"]", window.frames[0].document).attr('selected',true);
					    	CriticalEditionViewer.cwrc_params.position = $jq('#page_choose :selected', window.frames[0].document).attr('value');
					    	document.getElementById('viewer_iframe').contentWindow['PID'] = CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position];
					    	
					    	CriticalEditionViewer.Viewer.get_page_transformed_tei();
					    	
					    	CriticalEditionViewer.cwrc_writer_helper.Writer.load_next_anno_page();
					    	//console.log("after load next anno");
					    	//~~~
					    	$jq("#navi").remove();
					    	CriticalEditionViewer.Viewer.get_entities();
							CriticalEditionViewer.Viewer.build_tree_view();
							CriticalEditionViewer.Viewer.hide_preloader();
							
							CriticalEditionViewer.Viewer.show_versionable_transcriptions();
							
					    }
					});
					
					$jq('#editor_toolbargroup', window.frames[0].document).css("visibility", "hidden");
					$jq('#editor_toolbargroup', window.frames[0].document).css("display", "none");
					
					$jq('#editor_toolbargroup', window.frames[0].document).hide();
					
					$jq('#create_annotation', window.frames[0].document).css("visibility", "hidden");
					$jq('#create_annotation', window.frames[0].document).css("display", "none");
					CriticalEditionViewer.Viewer.show_plain_image();
					
					CriticalEditionViewer.Viewer.build_zoom();
					
					
					CriticalEditionViewer.Viewer.hide_preloader();
					
					// Build entity/anno tree off of existing.
					CriticalEditionViewer.Viewer.get_entities();
					CriticalEditionViewer.Viewer.build_tree_view();
					
					CriticalEditionViewer.Viewer.show_versionable_transcriptions();
				});
			}
			
			if($jq('#MediaPlayer').length > 0) {
				jwplayer("mediaplayer").setup({
				    file: $jq('#MediaPlayer').attr('data-url'),
				    image: $jq('#MediaPlayer').attr('data-thumbnail'),
				    width: $jq('#MediaPlayer').attr('data-width'),
				});
			}
		}
	},
};
