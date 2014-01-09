(function($) {
  Drupal.behaviors.IslandoraCriticalTEI = {
    attach: function(context, settings) {
      var diplomatic = Drupal.t("Show Diplomatic Transcription");
      var reading = Drupal.t("Show Reading Transcription");
      var tei_body = $('#tei_body');
      function switch_css() {
        tei_body.toggleClass('reading_tei');
        tei_body.toggleClass('diplomatic_tei');
      }
      $("#change_view").toggle(
          function() {
            $(this).text(reading);
            switch_css();
          },
          function() {
            $(this).text(diplomatic);
            switch_css();
          });
    }
  };
})(jQuery);


