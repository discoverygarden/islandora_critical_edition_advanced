
(function($) {
  /**
   * @file
   */
  Drupal.behaviors.contentDisplay = {
    attach: function(context, settings) {
      $('.teaser').click(function() {
       $(this).siblings('.full_text').toggleClass('full_text_hidden');
      });
      $('.full_text').click(function() {
       $(this).toggleClass('full_text_hidden');
      });
    }
  };
})(jQuery);

