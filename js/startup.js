
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
		          // Kick of the logic to fill this.
		          $jq.ajax({
		            type: 'POST',
		            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/prepare_advanced/' + pid,
		            data:{
		              "pid": pid,
		            },
		            success: function(data, status, xhr) {
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
	tei_document_text: null,
	checked_txtimglnk : null,
	checked_entities : null,
	checked_annos: null,
	jtree_checked_items: null,
	j_data: null,
	cwrc_writer: null,
	cwrc_writer_helper: null,
	cwrc_params: null,
	data_pid: null,
	transformed_data: null,
	pretty_transformed_data: null,
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
	              CriticalEditionViewer.Viewer.transformed_data = data['transformed'];
	              CriticalEditionViewer.Viewer.pretty_transformed_data = data['pretty_print'];
	              
	              CriticalEditionViewer.Viewer.remove_pretty_print_tei();
	              CriticalEditionViewer.Viewer.show_versionable_transcriptions();
	            },
	            error: function(xhRequest, ErrorText, thrownError) {
	              console.log(ErrorText + ":" + thrownError);
	            },
	          });
		},
		check_meta_data_state: function() {
			CriticalEditionViewer.Viewer.show_versionable_transcription();
		},
		show_preloader: function() {
			$jq("#loadImg").css('z-index','800');
			$jq("#loadImg").css('display','inherit');
		},
		hide_preloader: function() {
			$jq("#loadImg").css('display','none');
		},
		remove_translated_tei: function(){
			$jq('#translated_tei', window.frames[0].document).remove();
		},
		remove_pretty_print_tei: function(){
			$jq('#pretty_translated_tei', window.frames[0].document).remove();
		},
		show_plain_image: function() {
			CriticalEditionViewer.Viewer.remove_pretty_print_tei();
			CriticalEditionViewer.Viewer.remove_translated_tei();
			CriticalEditionViewer.cwrc_writer.layout.close("west");
			CriticalEditionViewer.cwrc_writer.layout.close("north");
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width());
			if($jq("#navi").length > 0) {
				CriticalEditionViewer.Viewer.toggle_anno_entities(0);
			}
		},
		show_versionable_transcriptions: function() {
			CriticalEditionViewer.Viewer.remove_pretty_print_tei();
			if($jq('#translated_tei', window.frames[0].document).length == 0) {
				$jq('#cwrc_main', window.frames[0].document).append('<div class="show_tei" id="translated_tei" style="overflow:auto;width:100%;height:100%;position:absolute;top:0px;z-index:300;background-color:white"></div>');
				$jq('#translated_tei', window.frames[0].document).append(CriticalEditionViewer.Viewer.transformed_data);
				CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
				CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
			}
		},
		show_pretty_print_transform: function() {
			$jq('#cwrc_main', window.frames[0].document).append('<div class="show_tei" id="pretty_translated_tei" style="overflow:auto;width:100%;height:100%;position:relative;top:0px;z-index:300;background-color:white"></div>');
			$jq('#pretty_translated_tei', window.frames[0].document).append(CriticalEditionViewer.Viewer.pretty_transformed_data);
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			document.getElementById('viewer_iframe').contentWindow.prettyPrint();
			CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
		},
		show_tei_markup: function() {
			CriticalEditionViewer.Viewer.remove_pretty_print_tei();
			CriticalEditionViewer.Viewer.show_pretty_print_transform();
		},
		show_tei_text: function() {
			CriticalEditionViewer.Viewer.remove_pretty_print_tei();
			CriticalEditionViewer.Viewer.remove_translated_tei();
			// TODO:
			if($jq('navi')) {
				
			}
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).addClass('showStructBrackets');
		},
		show_plain_text: function() {
			CriticalEditionViewer.Viewer.remove_translated_tei();
			CriticalEditionViewer.Viewer.remove_pretty_print_tei();
			// TODO:
			CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showStructBrackets');
		},
		toggle_anno_entities: function(show, toggle_text) {
			if(show == 1) {
				$jq("#navi").animate({
				      marginLeft:0},{
				      complete: function() {
				    	  if($jq("li[title='Image']").hasClass("img_selected")) {
				    		  CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()-($jq("#navi").width() + 100));
				    	  }
				    	  
				        CriticalEditionViewer.cwrc_writer.layout.open("west");
				      }
			    }, 700);
				
			} else {
				if(!$jq("li[title='Show/Hide annotations']").hasClass("img_selected") &&
						!$jq("li[title='Show/Hide Text Image Links']").hasClass("img_selected")) {
					CriticalEditionViewer.cwrc_writer.layout.close("west");
					$jq("#navi").animate({
				        marginLeft:-$jq("#navi").width()},{
				        complete: function() {
				        	if($jq("li[title='Image']").hasClass("img_selected")) {
					    		  CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width());
					    	  }
				        },
				    }, 700);
					
				}
				
			}
			
			// Show one or the other?
			switch (toggle_text) {
			case "Show/Hide annotations":
				if(show == 1) {
					$jq("#tree_entities_node").show();
					$jq("#tree_annotations_node").show();
				} else {
					$jq("#tree_entities_node").hide();
					$jq("#tree_annotations_node").hide();
				}
				break;
			case "Show/Hide Text Image Links":
				if(show == 1) {
					$jq("#tree_txtimglnk_node").show();
				} else {
					$jq("#tree_txtimglnk_node").hide();
				}
				break;
			}
			// Or, show everything if both are selected
			if($jq("li[title='Show/Hide annotations']").hasClass("img_selected") &&
					$jq("li[title='Show/Hide Text Image Links']").hasClass("img_selected")) {
				$jq("#tree_txtimglnk_node").show();
				$jq("#tree_entities_node").show();
				$jq("#tree_annotations_node").show();
			}
		},
		toggle_text_image_linking: function(show) {
			if(show == 1) {
				$jq("#tree_txtimglnk_node").show();
			    CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).addClass('showEntityBrackets');
			} else {
				$jq("#tree_txtimglnk_node").hide();
				CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showEntityBrackets');
			}
		},
		get_entities: function() {
			// Append to block_4_hidden.
			var cover = '<div id="navi" style="position:absolute;overflow:auto;width:34%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;"></div>';
			
			$jq("#view_box").append(cover);
			
			var search_box = '<div><input id="search_box" type="text" placeholder="Search..." value=""/></div>'
			$jq("#navi").append(search_box);
			
			
			
			var tree_html = '<div id="demo3"></div>';
			$jq('#navi').append(tree_html);
			
			CriticalEditionViewer.j_data = {"data" : [
			       						{ 
			    							"data" : "Annotations", 
			    							"attr" : { "id" : "tree_annotations_node"},
			    							"metadata" : { id : "tree_anno_list" },
			    							"children" : []
			    						},
			    						{ 
			    							"data" : "Entities", 
			    							"attr" : { "id" : "tree_entities_node"},
			    							"metadata" : { id : "tree_ent_list" },
			    							"children" : []
			    						},
			    						{ 
			    							"data" : "Text image link", 
			    							"attr" : { "id" : "tree_txtimglnk_node"},
			    							"metadata" : { id : "tree_txtimglnk_list" },
			    							"children" : []
			    						}
			    					]};
			
			$jq('.entitiesList li', window.frames[0].document).each(function() {
				if($jq(this).attr("name")) {
					var inner_data = $jq(this).find('div[class="info"]').children().last().children().last().text();
					if($jq(this).attr("class") == "txtimglnk") {
						if(inner_data == "" || inner_data.indexOf("certainty") != -1) {
							inner_data = $jq(this).attr("name");
						}
							var child_frame = {
								"attr" : { "id" : $jq(this).attr("name"),
								  "data-uuid" : inner_data.replace("uuid: ", ""),
								  "class" : "tree-txtimgitem-item",
								}, 
								"data" : {
									"title" : $jq(this).children('.entityTitle').first().text(), 
									"attr" : { "href" : "#" } 
								}
							};
							CriticalEditionViewer.j_data["data"][2]['children'].push(child_frame);
					} else {
						var child_frame = {
								"attr" : { "id" : $jq(this).attr("name"),
								  "data-uuid" : inner_data.replace("uuid: ", ""),
								  "class" : "tree-entity-item",
								},  
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
				if($jq(this).find('div[class="islandora_comment_type_title"]').text() != 'TextImageLink' &&
						$jq(this).find('div[class="islandora_comment_type_title"]').text() != 'unclassified') {
					var child_frame = {
							"attr" : { "id" : "anno_" + $jq(this).find('div[class="islandora_comment_type_title"]').text()}, 
							"data" : {
								"title" : $jq(this).find('div[class="islandora_comment_type_title"]').text(), 
								"attr" : { "href" : "#" } 
							},
							"children" : [],
					};
					$jq(this).find('div[class="islandora_comment_type_content"]').children().each(function() {
						child_frame["children"].push({
							"attr" : { "id" : $jq(this).attr('urn'),
								"class" : "tree-anno-item",}, 
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
			// TODO: method stub as required.
		},
		show_selected_anno: function(annos) {
			for(var i = 0;i<annos.length;i++) {
				if(annos[i].indexOf("anno_") == -1) {
					var trimmed_uuid = annos[i].replace("uuid: ", "");
					CriticalEditionViewer.Viewer.annos_click(annos[i]);
					CriticalEditionViewer.cwrc_writer.delegator.editorCallback('highlightEntity_gainFocus', $jq('.entitiesList', window.frames[0].document).find('li[name="' + trimmed_uuid + '"]'));
					$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + trimmed_uuid + '"]').css("background-color", "#FFFF00");
					document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + trimmed_uuid, window.frames[0].document), 'canvas_0', trimmed_uuid, "TextImageLink");
				}
			}
		},
		hide_unselected_anno: function() {
			if(CriticalEditionViewer.jtree_checked_items != null) {
				$jq('#entities > ul > li', window.frames[0].document).each(function(index, el) {
                    $jq(this).removeClass('selected').css('background-color', '').find('div[class="info"]').hide();
                      CriticalEditionViewer.cwrc_writer.highlightEntity($jq(this).closest("li").attr("id"));
            	}); 
				for(var x = 0;x < CriticalEditionViewer.jtree_checked_items.length;x++) {
					$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + CriticalEditionViewer.jtree_checked_items[x].replace("uuid: ", "") + '"]').css("background-color", "");
					$jq('.svg_' + CriticalEditionViewer.jtree_checked_items[x].replace("uuid: ", ""), window.frames[0].document).remove();
				}
			}
		},
		show_annos: function(annos) {
			for(var i = 0;i<annos.length;i++) {
				document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + annos[i], window.frames[0].document), 'canvas_0', annos[i], "TextImageLink");

			}
		},
		remove_annos : function() {
			if(CriticalEditionViewer.checked_annos) {
				for(var x = 0;x < CriticalEditionViewer.checked_annos.length;x++) {
					$jq('.svg_' + CriticalEditionViewer.checked_annos[x], window.frames[0].document).remove();
				}
			}
		},
		show_entities: function(annos) {
			for(var i = 0;i<annos.length;i++) {
				CriticalEditionViewer.Viewer.annos_click(annos[i]);
				CriticalEditionViewer.cwrc_writer.delegator.editorCallback('highlightEntity_gainFocus', $jq('.entitiesList', window.frames[0].document).find('li[name="' + annos[i] + '"]'));
			}
		},
		remove_entities : function() {
			if(CriticalEditionViewer.checked_entities) {
				var prevHighlight = $jq('#entityHighlight', CriticalEditionViewer.cwrc_writer.editor.getBody());
				if (prevHighlight.length == 1) {
					var parent = prevHighlight.parent()[0];
					prevHighlight.contents().unwrap();
					parent.normalize();
				}
			}
		},
		show_txtimglnk: function(annos) {
			for(var i = 0;i<annos.length;i++) {
				$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + annos[i] + '"]').css("background-color", "#FFFF00");
				document.getElementById('viewer_iframe').contentWindow.paint_commentAnnoTargets($jq('#anno_' + annos[i], window.frames[0].document), 'canvas_0', annos[i], "TextImageLink");
			}
		},
		remove_txtimglnk : function() {
			if(CriticalEditionViewer.checked_txtimglnk) {
				for(var i = 0;i<CriticalEditionViewer.checked_txtimglnk.length;i++) {
					var tid_id = CriticalEditionViewer.checked_txtimglnk[i];
					$jq('#translated_tei', window.frames[0].document).find('span[data-source="' + tid_id + '"]').css("background-color", "");
					$jq('.svg_' + CriticalEditionViewer.checked_txtimglnk[i], window.frames[0].document).remove();
				}
			}
		},
		build_tree_view: function() {
			$jq("#search_box").keyup(function () {
				$jt("#demo3").jstree("search", $jq(this).val());
			}); 
			
			$jt("#demo3").jstree({
				"json_data" : CriticalEditionViewer.j_data,
				"search" : {
					"case_insensitive" : true,
				},
				"plugins" : ["themes", "json_data", "ui", "checkbox","sort","search"],
			}).bind('loaded.jstree', function(e, data) {
			    // invoked after jstree has loaded
				$jq("#tree_txtimglnk_node").hide();
			}).bind("change_state.jstree", function (e, d) {
				CriticalEditionViewer.Viewer.hide_unselected_anno();
				var tagName = d.args[0].tagName;
			    var refreshing = d.inst.data.core.refreshing;
			    if ((tagName == "A" || tagName == "INS") &&
			      (refreshing != true && refreshing != "undefined")) {
			    	var checked_annos = [];
			    	var checked_entities = [];
			    	var checked_txtimglnk =[];
			    	$jq('.tree-txtimgitem-item').each(function() {
			    		var data_tag = $jq(this).attr("data-uuid");
			    		if($jq(this).hasClass("jstree-checked") && data_tag.indexOf("ent_") == -1) {
			    			checked_txtimglnk.push(data_tag);
			    		}
			    	});
			    	$jq('.tree-entity-item').each(function() {
			    		if($jq(this).hasClass("jstree-checked")) {
			    			checked_entities.push(this.id);
			    		}
			    	});
			    	$jq('.tree-anno-item').each(function() {
			    		if($jq(this).hasClass("jstree-checked")) {
			    			checked_annos.push(this.id);
			    		}
			    	});
			      CriticalEditionViewer.Viewer.remove_annos();
			      CriticalEditionViewer.checked_annos = checked_annos;
			      CriticalEditionViewer.Viewer.show_annos(checked_annos);
			      
			      CriticalEditionViewer.Viewer.remove_entities();
			      CriticalEditionViewer.checked_entities = checked_entities;
			      CriticalEditionViewer.Viewer.show_entities(checked_entities);
			      
			      CriticalEditionViewer.Viewer.remove_txtimglnk();
			      CriticalEditionViewer.checked_txtimglnk = checked_txtimglnk;
			      CriticalEditionViewer.Viewer.show_txtimglnk(checked_txtimglnk);
			    }
			}).bind("search.jstree", function (e, data) {
				// TODO: something else cool with the selected nodes??
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
			$jq("li[title='Detail Metadata']").removeClass("img_selected");
			$jq("#meta_overlay").animate({
			      marginTop:-$jq("#meta_overlay").height()},{
			      complete: function() {
			      }
			    }, 700);
		},
		show_tei_document_text: function() {
			$jq("#tei_doc_text", window.frames[0].document).remove();
			if($jq("#tei_doc_text", window.frames[0].document).length == 0) {
				CriticalEditionViewer.tei_document_text = CriticalEditionViewer.cwrc_writer.fm.getDocumentContent(false);
				$jq('#cwrc_main', window.frames[0].document).append('<div class="show_tei" id="tei_doc_text" style="overflow:auto;width:100%;height:100%;position:absolute;top:0px;z-index:300;background-color:white"><textarea style="width: 100%;height:100%" readonly id="tei_text_area"></textarea></div>');
				$jq('#tei_text_area', window.frames[0].document).val(CriticalEditionViewer.tei_document_text);
				CriticalEditionViewer.cwrc_writer.layout.sizePane("east", $jq('#CriticalEditionViewer').width()/2);
			}
		},
		remove_tei_document_text:function() {
			$jq("#tei_doc_text", window.frames[0].document).remove();
		},
		show_versionable_permalink: function() {
			// The current menu path is the permalink...
			location.reload();
		},
		zoom_plus_click: function() {
			CriticalEditionViewer.current_zoom = CriticalEditionViewer.current_zoom + 5;
			CriticalEditionViewer.Viewer.zoom_level_update();
		},
		zoom_minus_click: function() {
			if(CriticalEditionViewer.current_zoom - 5 >= 0) {
				CriticalEditionViewer.current_zoom = CriticalEditionViewer.current_zoom - 5;
				CriticalEditionViewer.Viewer.zoom_level_update();
			}
			
		},
		zoom_level_update: function() {
			$jq('#zoom').text(CriticalEditionViewer.current_zoom);
			// Need to destroy it every time.
			CriticalEditionViewer.Viewer.destroy_zoom();
			var img = $jq('#annotations', window.frames[0].document).find('div[class="base_img"]').children(0);
			$zm(img).parent()
			    .zoom({
			      magnify: CriticalEditionViewer.current_zoom
			    });
		},
		build_zoom: function() {
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
			
			// Set up img click handlers
			$jq(".work_action_img").click(function() {
				
				if($jq(this).attr("title") == "Show/Hide annotations" || $jq(this).attr("title") == "Show/Hide Text Image Links") {
					if($jq(this).attr("data-value") == 1) {
							$jq(this).removeClass("img_selected");
							$jq(this).attr("data-value",0);
						} else {
							$jq(this).addClass("img_selected");
							$jq(this).attr("data-value",1);
						}
					CriticalEditionViewer.Viewer.toggle_anno_entities($jq(this).attr("data-value"), $jq(this).attr("title"));
					switch ($jq(this).attr("title")) {
					case "Show/Hide annotations":
						CriticalEditionViewer.Viewer.check_meta_data_state();
						break;
					case "Show/Hide Text Image Links":
						CriticalEditionViewer.Viewer.check_meta_data_state();
						CriticalEditionViewer.Viewer.toggle_text_image_linking($jq(this).attr("data-value"));
						break;
					}
				} else {
					
					// Preform the approate action.
					switch ($jq(this).attr("title")) {
						case "Transcription":
							CriticalEditionViewer.Viewer.remove_tei_document_text();
							$jq(".work_action_img").removeClass("img_selected");
							$jq(this).addClass("img_selected");
							CriticalEditionViewer.Viewer.check_meta_data_state();
							CriticalEditionViewer.Viewer.show_plain_text();
							break;
						case "TEI Markup":
							$jq(".work_action_img").removeClass("img_selected");
							$jq(this).addClass("img_selected");
							CriticalEditionViewer.Viewer.check_meta_data_state();
							CriticalEditionViewer.Viewer.show_tei_document_text();
							break;
						case "Image":
							CriticalEditionViewer.Viewer.remove_tei_document_text();
							$jq(".work_action_img").removeClass("img_selected");
							$jq(this).addClass("img_selected");
							CriticalEditionViewer.Viewer.check_meta_data_state();
							CriticalEditionViewer.Viewer.show_plain_image();
							break;
						case "Diplomatic Transcriptions":
							CriticalEditionViewer.Viewer.remove_tei_document_text();
							$jq(".work_action_img").removeClass("img_selected");
							$jq(this).addClass("img_selected");
							CriticalEditionViewer.Viewer.check_meta_data_state();
							CriticalEditionViewer.Viewer.show_versionable_transcriptions();
							break;
//						case "TEI Markup":
//							$jq(".work_action_img").removeClass("img_selected");
//							$jq(this).addClass("img_selected");
//							CriticalEditionViewer.Viewer.check_meta_data_state();
//							CriticalEditionViewer.Viewer.show_tei_markup();
//							break;
						case "Detail Metadata" :
							if($jq("#meta_overlay").length == 0) {
								var meta_cover = '<div id="meta_overlay" style="position:absolute;overflow:auto;width:100%;height:100%;background-color: #FFFFFF;z-index: 500;top: 0;"></div>';
								$jq("#view_box").append(meta_cover);
								$jq("#meta_overlay").css("margin-top",-$jq("#meta_overlay").height());
								$jq("li[title='Detail Metadata']").addClass("img_selected");
							} else {
								$jq("#meta_overlay").animate({
							        marginTop:-$jq("#meta_overlay").height()},{
							        complete: function() {
							          $jq('.data_anchor').css('font-weight', 'normal');
							          $jq('#detail_tran').css('font-weight', 'bold');
							          $jq("#meta_overlay").remove();
							          $jq("li[title='Detail Metadata']").removeClass("img_selected")
							        },
							    }, 700);
							}
							CriticalEditionViewer.Viewer.show_versionable_meta();
							break;
					}
				}
				
			});
			
			if($jq("#viewer_iframe").length > 0) {
				$jq("#viewer_iframe").load(function (){
					
					var common_tei_css_Link = document.createElement("link") 
					common_tei_css_Link.href = Drupal.settings.islandora_critical_edition_advanced.common_tei_css;  
					common_tei_css_Link.rel = "stylesheet"; 
					common_tei_css_Link.type = "text/css"; 
					
					var diplomatic_tei_css_Link = document.createElement("link") 
					diplomatic_tei_css_Link.href = Drupal.settings.islandora_critical_edition_advanced.diplomatic_tei; 
					diplomatic_tei_css_Link.rel = "stylesheet"; 
					diplomatic_tei_css_Link.type = "text/css";
					
					window.frames[0].document.body.appendChild(common_tei_css_Link);
					window.frames[0].document.body.appendChild(diplomatic_tei_css_Link);
					
					
					CriticalEditionViewer.cwrc_writer = document.getElementById('viewer_iframe').contentWindow['writer'];
					CriticalEditionViewer.cwrc_writer.layout.north.options.resizeable = false;
					CriticalEditionViewer.cwrc_writer_helper = document.getElementById('viewer_iframe').contentWindow['islandoraCWRCWriter'];
						
					CriticalEditionViewer.cwrc_writer.editor.$('body', window.frames[0].document).removeClass('showEntityBrackets');
					
					CriticalEditionViewer.cwrc_params = document.getElementById('viewer_iframe').contentWindow['cwrc_params'];
					
					CriticalEditionViewer.cwrc_writer.pager_data = $jq('#page_choose', window.frames[0].document);
					
					$jq('#page_choose option', window.frames[0].document).each(function() {
						CriticalEditionViewer.pager_data[$jq(this).attr("value") - 1] = $jq(this).attr("value");
					});
					// Hide the resize bar buttons in the UI-Layout plugin.
					$jq('.ui-layout-toggler', window.frames[0].document).css("visibility", "hidden");
					
					$pg('.pagination').jqPagination({
						max_page: CriticalEditionViewer.pager_data.length,
					    paged: function(page) {
					    	CriticalEditionViewer.Viewer.remove_translated_tei();
					    	CriticalEditionViewer.Viewer.remove_pretty_print_tei();
					    	
					    	CriticalEditionViewer.Viewer.show_preloader();
					    	CriticalEditionViewer.Viewer.toggle_text_image_linking(0);
					    	CriticalEditionViewer.Viewer.show_plain_image();
					    	
					    	$jq('#page_choose', window.frames[0].document).val(page);
					    	$jq("#page_choose :selected[true]", window.frames[0].document).attr('selected',false);
					    	$jq("#page_choose option[value="+page+"]", window.frames[0].document).attr('selected',true);
					    	CriticalEditionViewer.cwrc_params.position = $jq('#page_choose :selected', window.frames[0].document).attr('value');
					    	document.getElementById('viewer_iframe').contentWindow['PID'] = CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position];	
					    	CriticalEditionViewer.cwrc_writer_helper.Writer.load_next_anno_page();
					    	//~~~
					    	$jq("#navi").remove();
					    	CriticalEditionViewer.Viewer.get_entities();
							CriticalEditionViewer.Viewer.build_tree_view();
							CriticalEditionViewer.Viewer.hide_preloader();
							
							CriticalEditionViewer.Viewer.get_page_transformed_tei(CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position]);
							CriticalEditionViewer.Viewer.remove_tei_document_text();
							$jq(".work_action_img").removeClass("img_selected");
							$jq("li[title='Diplomatic Transcriptions']").addClass("img_selected")
							
					    }
					});
					
					$jq('#editor_toolbargroup', window.frames[0].document).css("visibility", "hidden");
					$jq('#editor_toolbargroup', window.frames[0].document).css("display", "none");
					
					$jq('#editor_toolbargroup', window.frames[0].document).hide();
					
					$jq('#create_annotation', window.frames[0].document).css("visibility", "hidden");
					$jq('#create_annotation', window.frames[0].document).css("display", "none");
					CriticalEditionViewer.Viewer.build_zoom();
					
			    	CriticalEditionViewer.Viewer.get_entities();
					CriticalEditionViewer.Viewer.build_tree_view();
					CriticalEditionViewer.tei_document_text = CriticalEditionViewer.cwrc_writer.fm.getDocumentContent(false);
					CriticalEditionViewer.Viewer.get_page_transformed_tei(CriticalEditionViewer.cwrc_params.pages[ CriticalEditionViewer.cwrc_params.position]);
					CriticalEditionViewer.Viewer.show_plain_image();
					$jq(".work_action_img").removeClass("img_selected");
					$jq("li[title='Diplomatic Transcriptions']").addClass("img_selected")
					CriticalEditionViewer.Viewer.show_versionable_transcriptions();
					// Disable editing in the editor.
					window.frames[0].tinymce.activeEditor.getBody().setAttribute('contenteditable', false);
					CriticalEditionViewer.Viewer.hide_preloader();
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
