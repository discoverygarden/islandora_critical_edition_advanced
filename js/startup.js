$('document').ready(function() {
  $( '#carousel' ).elastislide( {
    orientation : 'vertical',
    minItems: 2,
    onClick : function( el, position, evt ) {
      if($('#aparatusDialog').length == 0) {
        $(document.body).append(''+
          '<div id="aparatusDialog">'+
          '<div id="CriticalEditionViewer"></div>'+
          '</div>');
      }
      //console.log($(el).children().first().attr('data-pid'));
      // TODO: get the width/height dynamically.
      $('#aparatusDialog').dialog({
        title: 'Edit Header',
        modal: true,
        resizable: true,
        height: 600,
        width: 1000,
        open: function() {
          // Kick of the logic to fill this.
          $.ajax({
            type: 'POST',
            url: Drupal.settings.basePath + 'islandora/cwrc_viewer/prepare_advanced',
            data:{
              "pid": $(el).children().first().attr('data-pid')
            },
            success: function(data, status, xhr) {
              console.log(data);
              CriticalEditionViewer.AudioViewer.build(data);
            },
            error: function(xhRequest, ErrorText, thrownError) {
              console.log(ErrorText);
            },
          });
        },
      });
      return false;
    },
    onReady: function() {
      $('.elasti_img').hover(function() {
        // TODO: need an interactive tool tip perhaps?
        //console.log("hover");
      });
      return false;
    }
  });
  //$( '#elastislide-outer-wrapper' ).css('height',$('#critical_edition_publish_form_wrapper:first').height());
});

CriticalEditionViewer = {
	AudioViewer: {
		build: function(json_html) {
			if($('#critical_edition_publish_form_wrapper').lenght > 0) {
				$('#critical_edition_publish_form_wrapper').remove();
			}
			// console.log(json_html);
			$('#CriticalEditionViewer').append(json_html);
			console.log($('#MediaPlayer').attr('data-url'));
			jwplayer("mediaplayer").setup({
			    file: $('#MediaPlayer').attr('data-url'),
			    image: $('#MediaPlayer').attr('data-thumbnail'),
			    width: $('#MediaPlayer').attr('data-width'),
			  });
		},
		view_audio: function() {
		
		},
	},
};