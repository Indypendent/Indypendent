/**
 * @file
 * Implements JQuery UI and history using BBQ.
 * See http://benalman.com/code/projects/jquery-bbq/examples/fragment-jquery-ui-tabs
 */
(function ($) {
  Drupal.behaviors.qt_ui_tabs = {
    attach: function (context, settings) {

      options = Drupal.settings.quicktabs;
      params = $.deparam.fragment();

      // This selector will be reused when selecting actual tab widget A elements.
      tab_a_selector = 'ul.ui-tabs-nav a';

      var tabs = $('.quicktabs-ui-wrapper').tabs({
        event: 'change'
      }).addClass("init");

      // Define our own click handler for the tabs, overriding the default.
      tabs.find( tab_a_selector ).click(function(){
        var state = {},
          
          // Get the id of this tab widget.
          id = $(this).closest( '.quicktabs-ui-wrapper' ).attr( 'id' ),
          
          // Get the index of this tab.
          idx = $(this).parent().prevAll().length;
        
        el = id.split("quicktabs-");
        qt_id = 'qt_' + el[1];
        // Only fire if history is set.
        if (options[qt_id].history) {
          state[ id ] = idx;
          $.bbq.pushState( state );
        }
        else {
          name = 'quicktabs-' + options[qt_id].name;
          $('#' + name).tabs('select', idx);
        }
      });
      
      $(window).bind( 'hashchange', function(e) {
        // Should only fire once when page loads. 
        if (tabs.hasClass("init")) {
          for (var key in options) {
            name = 'quicktabs-' + options[key].name;
            active_tab = parseInt(options[key].active_tab);
            // If there is an active tab and the tab isn't already defined by a hash.
            if (active_tab > 0 && !params[name]) {
              $("#" + name).tabs('select', active_tab);
            }
          }
          tabs.removeClass("init");
        }
        tabs.each(function() {
          var idx = $.bbq.getState( this.id, true );
          $(this).find( tab_a_selector ).eq( idx ).triggerHandler( 'change' );
        });

      })
      
      $(window).trigger( 'hashchange' );

    }
  }
})(jQuery);
