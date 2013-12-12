/**
 * @file
 *  Handles the versionable object ingest form specialy javascript nonsense.
 */
(function($) {
  "use strict";

  // Hide uploader.
  $(document).ready(function() {
    // Assume it's always hidden to start off.
    $('.form-type-plupload').hide();
    if ($('input[name="transcription_source"][value="upload"]').is(':checked')) {
      $('.form-type-plupload').show();
    }
    // Prevent enter submit on the source textfield as this will skip
    // validation of the #access = FALSE elements.
    $('input[name="source"]').keypress(function(event) {
      if (event.keyCode == '13') {
        event.preventDefault();
      }
    });
  });

  /**
   * A work around for the pluploader since it doesn't respect Drupal #states
   * invisible state. Currently Hard coded.
   */
  Drupal.behaviors.hideUploadTranscription = {
    attach: function() {
      $('input[name="transcription_source"][value="upload"]').once(function() {
        // Show / hide on first render.
        $(this).is(':checked') ? $('.form-type-plupload').show() : $('.form-type-plupload').hide();
        $('input[name="transcription_source"]').change(function() {
          $(this).val() == 'upload' ? $('.form-type-plupload').show() : $('.form-type-plupload').hide();
        });
      });
    }
  }

  /**
   * Performs a cached and delayed search.
   *
   * Override the existing function so that it refreshes the URI based on the
   * value of the select.
   */
  Drupal.ACDB.prototype.search = function (searchString) {
    var db = this;
    this.searchString = searchString;

    // See if this string needs to be searched for anyway.
    searchString = searchString.replace(/^\s+|\s+$/, '');
    if (searchString.length <= 0 ||
        searchString.charAt(searchString.length - 1) == ',') {
      return;
    }

    // See if this key has been searched for before.
    if (this.cache[searchString]) {
      return this.owner.found(this.cache[searchString]);
    }

    // Initiate delayed search.
    if (this.timer) {
      clearTimeout(this.timer);
    }
    this.timer = setTimeout(function () {
      db.owner.setStatus('begin');

      // Ajax GET request for autocompletion. We use Drupal.encodePath instead of
      // encodeURIComponent to allow autocomplete search terms to contain slashes.
      $.ajax({
        type: 'GET',
        url: db.uri + '/' + $('select[name="source_type"]').val() + '/' + Drupal.encodePath(searchString),
        dataType: 'json',
        success: function (matches) {
          if (typeof matches.status == 'undefined' || matches.status != 0) {
            // We don't cache values as we want to hide values when the select
            // field changes.
            db.cache = [];
            // Verify if these are still the matches the user wants to see.
            if (db.searchString == searchString) {
              db.owner.found(matches);
            }
            db.owner.setStatus('found');
          }
        },
        error: function (xmlhttp) {
          alert(Drupal.ajaxError(xmlhttp, db.uri));
        }
      });
    }, this.delay);
  }



})(jQuery);
