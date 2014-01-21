(function($) {
  $('document').ready(function() {
	  if($(".vertical-tabs-list").children().length > 1) {
	    $('.aparatus_tab_previous').click(function(e) {
	    	e.preventDefault();
	      if($(this).first().text().trim() != '_blank_' && $(this).first().text() != "") {
	        select_vertical_tab('previous');
	      }
	      return true;
	    });
	    $('.aparatus_tab_next').click(function(e) {
	    	e.preventDefault();
	      if($(this).first().text().trim() != '_blank_' && $(this).first().text() != "") {
	        select_vertical_tab('next');
	      }
	      return true;
	    });
	  } else {
		  // Hide the pager, its not required.
		  $('#aparatus_pager').addClass('hide_content');
		  //.hide_content
	  }
	  
	  
    
  });

  function select_vertical_tab(direction) {
    if(direction == 'previous') {
      $('li.selected').prev().addClass('selected');
      $('li.selected').next().removeClass('selected');
      $(".vertical-tabs-pane:visible").hide().prev().show();
    } else {
      $('li.selected').next().addClass('selected');
      $('li.selected').prev().removeClass('selected');
      $(".vertical-tabs-pane:visible").hide().next().show();//.next().show().removeAttr('style').prev().hide();
    }
  }
})(jQuery);



