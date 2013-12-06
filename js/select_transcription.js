(function($) {
  "use strict";
  $('document').ready(function() {
	  /**
	   * @file
	   */
	  Drupal.behaviors.selectTranscription = {
	    attach: function(context, settings) {
	      // Bind Ajax behaviors to all items showing the class.
	      $('.transcription-select').once(function () {
	        $(this).change(function() {
	          var pid = $(this).val();
	          console.log("clicked: " + pid);
	          // This is hard coded for now, as we need to save time, sorry.
	          var paths = {
	            "view": "/islandora/object/" + pid,
	            "edit": "/islandora/transcription/edit/" + pid,
	            "delete": "/islandora/object/" + pid + "/delete",
	          };
	          $(this).parents('.transcriptions-container').find('li').each(function() {
	            var action = undefined;
	            for(action in paths) {
	              if ($(this).hasClass(action)) {
	                $('a', this).attr('href', paths[action]);
	              }
	            }
	          });
	        });
	      });
	    }
	  };
  });
})(jQuery);
