(function($) {
  /**
   * @file
   */
  Drupal.behaviors.selectTranscription = {
    attach: function(context, settings) {

      $('.transcription-select').change(function() {
        var pid = $(this).val();
        var paths = {
          "view": "/islandora/object/" + pid,
          "edit": "/islandora/transcription/edit/" + pid,
          "delete": "/islandora/object/" + pid + "/delete",
        };
        $(this).parents('.transcriptions-container').find('li').each(function() {
          var action = undefined;
          for (action in paths) {
            if ($(this).hasClass(action)) {
              $('a', this).attr('href', paths[action]);
            }
          }
        });
      });
    }
  };
})(jQuery);
