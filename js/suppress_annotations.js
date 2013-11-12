(function($) {
  Drupal.behaviors.IslandoraCriticalEditionAdvanced = {
    attach: function(context, settings) {
      $("#annotation_tab").hide();
      $("#create_annotation").hide();
      $("#page_selector").hide();
      $(".header-nav").css('visibility','hidden');
    }
  };
})(jQuery);
