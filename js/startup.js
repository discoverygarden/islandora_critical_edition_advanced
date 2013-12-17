
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
	jtree_checked_items: null,
	j_data: null,
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
		},
		show_versionable_transcriptions: function() {
			if($jq('#translated_tei', window.frames[0].document).length == 0) {
				console.log("should be added...")
				$jq('#cwrc_main', window.frames[0].document).append('<div class="show_tei"id="translated_tei" style="width:100%;height:100%;position:absolute;top:0px;z-index:300;background-color:white"></div>');
				
				CriticalEditionViewer.Viewer.get_page_transformed_tei(CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position]);
				
				$jq('#translated_tei', window.frames[0].document).append(CriticalEditionViewer.Viewer.transformed_data);
				CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
				CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
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
				$jq("#navi").animate({
				      marginLeft:0},{
				      complete: function() {
				    	  CriticalEditionViewer.cwrc_writer.layout.open("west");
				
				      }
			    }, 700);
			} else {
				CriticalEditionViewer.cwrc_writer.layout.close("west");
				
				$jq("#navi").animate({
			        marginLeft:-$jq("#navi").width()},{
			        complete: function() {
			        	
			        },
			    }, 700);
			}
		},
		toggle_text_image_linking: function(show) {
			//$jq("#publish_txtimglnk_list").toggle();
			$jq(".jstree-last").toggle();
			if(show == 1) {
				
//				$jq("#navi").animate({
//			      marginLeft:0},{
//			      complete: function() {
			        //CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($('#cwrc_wrapper', window.frames[0].document).width()-$("#navi").width()));
			        CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
//			        if(!$jq('#img_title').hasClass('img_selected')) {
//			          CriticalEditionViewer.cwrc_writer.layout.open("west");
//			          CriticalEditionViewer.cwrc_writer.layout.sizePane("west", $jq("#navi").width());
//			        } else {
//			          CriticalEditionViewer.cwrc_writer.layout.sizePane("east", ($jq('#cwrc_wrapper', window.frames[0].document).width()-$jq("#navi").width()));
//			        }
//			    }, 700);
			} else {
				//
				//CriticalEditionViewer.cwrc_writer.layout.close("west");
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
				
				
//				if($jq('#img_title').hasClass('img_selected')) {
//				  CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#cwrc_wrapper', window.frames[0].document).width());
//				}
//				
//				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).toggleClass('showEntityBrackets');
				
				
				
				
				//
//				$jq("#navi").animate({
//			        marginLeft:-$jq("#navi").width()},{
//			        complete: function() {
//			        	
//			        },
//			    }, 700);
				
			}
		},
		get_entities: function() {
			// Append to block_4_hidden.
			var cover = '<div id="navi" style="position:absolute;overflow:auto;width:34%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;"></div>';
			
			$jq("#view_box").append(cover);
			
			var tree_html = '<div id="demo3"></div>';
			$jq('#navi').append(tree_html);
			
			CriticalEditionViewer.j_data = {"data" : [
			       						{ 
			    							"data" : "Annotations", 
			    							"metadata" : { id : "tree_anno_list" },
			    							"children" : []
			    						},
			    						{ 
			    							"data" : "Entities", 
			    							"metadata" : { id : "tree_ent_list" },
			    							"children" : []
			    						},
			    						{ 
			    							"data" : "Text image link", 
			    							"metadata" : { id : "tree_txtimglnk_list" },
			    							"children" : []
			    						}
			    					]};
			
			$jq('.entitiesList li', window.frames[0].document).each(function() {
				// If a class is found, its an entity entry.
				if($jq(this).attr("class")) {
					var inner_data = $jq(this).find('div[class="info"]').children().last().children().last().text();
					if($jq(this).attr("class") == "txtimglnk") {
						if(inner_data == "") {
							inner_data = $jq(this).attr("name");
						}
						if(inner_data.indexOf("ent_") == -1) {
							var child_frame = {
								"attr" : { "id" : inner_data }, 
								"data" : {
									"title" : $jq(this).children('.entityTitle').first().text(), 
									"attr" : { "href" : "#" } 
								}
							};
							CriticalEditionViewer.j_data["data"][2]['children'].push(child_frame);
						}
						
					} else {
						var child_frame = {
								"attr" : { "id" : $jq(this).attr("name") }, 
								"data" : {
									"title" : $jq(this).children('.entityTitle').first().text(), 
									"attr" : { "href" : "#" } 
								}
							};
						CriticalEditionViewer.j_data["data"][1]['children'].push(child_frame);
					}
				}
			});
			// Build the annotation list
			$jq('#comment_annos_block', window.frames[0].document).children().each(function() {
				if($jq(this).find('div[class="islandora_comment_type_title"]').text() != 'TextImageLink') {
					var child_frame = {
							"attr" : { "id" : "anno_" + $jq(this).find('div[class="islandora_comment_type_title"]').text() }, 
							"data" : {
								"title" : $jq(this).find('div[class="islandora_comment_type_title"]').text(), 
								"attr" : { "href" : "#" } 
							},
							"children" : [],
					};
					$jq(this).find('div[class="islandora_comment_type_content"]').children().each(function() {
						child_frame["children"].push({
							"attr" : { "id" : $jq(this).attr('urn') }, 
							"data" : {
								"title" : $jq(this).find('div[class="comment_content"]').text(), 
								"attr" : { "href" : "#" } 
							},
						});
					});
					CriticalEditionViewer.j_data["data"][0]['children'].push(child_frame);
				}
			});
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
		show_selected_anno: function(annos) {
			for(var i = 0;i<annos.length;i++) {
				if(annos[i].indexOf("anno_") == -1) {
					var trimmed_uuid = annos[i].replace("uuid: ", "");
					//$jq('.svg_' + trimmed_uuid, window.frames[0].document).remove();
					$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + trimmed_uuid + '"]').css("background-color", "#FFFF00");
					document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + trimmed_uuid, window.frames[0].document), 'canvas_0', trimmed_uuid, "TextImageLink");
				}
			}
		},
		hide_unselected_anno: function() {
			
			if(CriticalEditionViewer.jtree_checked_items != null) {
				for(var x = 0;x < CriticalEditionViewer.jtree_checked_items.length;x++) {
					$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + CriticalEditionViewer.jtree_checked_items[x].replace("uuid: ", "") + '"]').css("background-color", "");
					$jq('.svg_' + CriticalEditionViewer.jtree_checked_items[x].replace("uuid: ", ""), window.frames[0].document).remove();
				}
			}
		},
		build_tree_view: function() {
			$jt("#demo3").jstree({
				"json_data" : CriticalEditionViewer.j_data,
				"plugins" : ["themes", "json_data", "ui", "checkbox","sort"],
			}).bind('loaded.jstree', function(e, data) {
			    // invoked after jstree has loaded
				console.log("tree loaded");
				$jq(".jstree-last").toggle();
			}).bind("change_state.jstree", function (e, d) {
				CriticalEditionViewer.Viewer.hide_unselected_anno();
				var tagName = d.args[0].tagName;
			    var refreshing = d.inst.data.core.refreshing;
			    if ((tagName == "A" || tagName == "INS") &&
			      (refreshing != true && refreshing != "undefined")) {
			      //if a checkbox or it's text was clicked, 
			      //and this is not due to a refresh or initial load, run this code . . .
			      //console.log(d);
			      var checked_ids = [];
			      var unchecked_ids = [];
			      $jt("#demo3").jstree("get_checked",null,true).each(function () {
		              checked_ids.push(this.id);
		          });
			      $jt("#demo3").jstree("get_unchecked",null,true).each(function () {
			          unchecked_ids.push(this.id); 
		          });
			      CriticalEditionViewer.jtree_checked_items = checked_ids;
			      CriticalEditionViewer.Viewer.show_selected_anno(checked_ids);
			      
			    }
			})
			//console.log($jq("#tree_txtimglnk_list"));
			
//			$jt('#demo3').jstree({
//				"checkbox": {
//		              real_checkboxes: false,
//		              two_state: true
//		           },
//				"plugins" : ["themes", "ui", "checkbox"]
//			}).bind('select_node.jstree', function (e, data) {
//				console.log(e);
//				console.log($jq(data.node).attr('id'));
//				// Hate this, but this version of jstree kinda
//				// requires it.
//				var clicked_id = $jq(data.node).attr('id');
//				var data_stuff = $jq('.jstree-clicked');
//				
//				// The following highlights entity's
////				for(var i = 0;i<data_stuff.length;i++) {
////					if($jq(data_stuff[i]).closest("li").attr("id")) {
////						var li_id = $jq(data_stuff[i]).closest("li").attr("id");
////						if(clicked_id.indexOf("ent_") !== -1) {
////							CriticalEditionViewer.Viewer.annos_click(clicked_id.replace("uuid: ", ""));
////						}
////					}
////				}
////						document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + clicked_id.replace("uuid: ", ""), window.frames[0].document), 'canvas_0', clicked_id.replace("uuid: ", ""), "TextImageLink");
////						if($jq('#translated_tei', window.frames[0].document).length > 0) {
////							$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + clicked_id.replace("uuid: ", "") + '"]').css("background-color", "#FFFF00");
////						}
//						
//				for(var x = 0;x<data_stuff.length;x++) {
//					if($jq(data_stuff[x]).closest("li").attr("id")) {
//						var uuid = $jq(data_stuff[x]).closest("li").attr("id");
//						if(uuid.indexOf("annos_") === -1) {
//							var trimmed_uuid = uuid.replace("uuid: ", "");
//							document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + trimmed_uuid, window.frames[0].document), 'canvas_0', trimmed_uuid, "TextImageLink");
//							if($jq('#translated_tei', window.frames[0].document).length > 0) {
//								console.log("translate tei exists");
//								$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + trimmed_uuid + '"]').css("background-color", "#FFFF00");
//							}
//						}
//					}
//				}
//				
//			}).bind('deselect_node.jstree', function(e,data) {
////				console.log(e);
////				console.log(data);
//				var data_stuff = $jq('.jstree-anchor');
//				var clicked_id = $jq(data.node).attr('id');
////				$jq('#entities > ul > li', window.frames[0].document).each(function(index, el) {
////					$jq(this).removeClass('selected').css('background-color', '').find('div[class="info"]').hide();
////					CriticalEditionViewer.cwrc_writer.delegator.editorCallback('highlightEntity_looseFocus', $jq(this));
////				});
//				
//				//var trimmed_uuid = uuid.replace("uuid: ", "");
//				$jq('.svg_' + clicked_id.replace("uuid: ", ""), window.frames[0].document).remove();
//				$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + clicked_id.replace("uuid: ", "") + '"]').css("background-color", "");
//				
//				for(var x = 0;x<data_stuff.length;x++) {
//					if($jq(data_stuff[x]).closest("li").attr("id")) {
//						var uuid = $jq(data_stuff[x]).closest("li").attr("id");
//						if(uuid.indexOf("annos_") === -1) {
//							var trimmed_uuid = uuid.replace("uuid: ", "");
//							$jq('.svg_' + trimmed_uuid, window.frames[0].document).remove();
//							$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + trimmed_uuid + '"]').css("background-color", "");
//						}
//					}
//				}
//				
//			});
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
						CriticalEditionViewer.Viewer.toggle_anno_entities($jq(this).attr("value"));
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
				    type: $jq('#MediaPlayer').attr('data-mime'),
				});
			}
		}
	},
};
