
(function($) {
  /**
   * @file
   */
  Drupal.behaviors.contentDisplay = {
    attach: function(context, settings) {
      $('td[class=content], td[class=context]').click(function(){
        $(this).find('.full_text').toggleClass('full_text_hidden');
        $(this).find('.teaser').toggleClass('teaser_only');
      });
    }
  };
})(jQuery);
