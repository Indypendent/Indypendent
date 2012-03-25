/**
 * Create a DARTadmin object
 */
Drupal.DARTadmin = {};

Drupal.behaviors.DARTadmin = function(context) {
  $('#edit-dart-special-key-vals-tile').click(Drupal.DARTadmin.tileInitVal);
  Drupal.DARTadmin.tileInitVal();
}

/**
 * This function hides or shows the "Initial Value of Tile" field based
 * if the "tile" checkbox has been checked.
 */
Drupal.DARTadmin.tileInitVal = function() {
  var textfield = $('#edit-dart-special-tile-init-wrapper');
  $('#edit-dart-special-key-vals-tile').attr('checked') ? textfield.slideDown('fast') : textfield.slideUp('fast');
}