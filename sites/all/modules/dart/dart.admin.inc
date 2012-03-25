<?php

/**
 * @file
 * Admin form handling for the DART module.
 */

/**
 * Form builder; Configure dart settings for this site.
 *
 * @ingroup forms
 */
function dart_admin_settings() {
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'dart') . '/dart.admin.js'
  );
  $form['#validate'][] = 'dart_admin_settings_validate';

  $form['settings'] = array(
    '#type' => 'vertical_tabs',
  );

  // Default tag settings.
  $form['settings']['tag_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Tag Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['tag_settings']['dart_global_network_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Network ID'),
    '#default_value' => variable_get('dart_global_network_id', ''),
    '#required' => FALSE,
    '#description' => t('The Network ID to use on all tags. This is required for networks that upgraded from DART to DoubleClick. If you were not provided a network ID from DoubleClick leave this blank.'),
    '#maxlength' => 32,
  );
  $form['settings']['tag_settings']['default_tag_settings'] = array(
    '#theme' => 'dart_global_defaults_form',
    '#description' => 'Enter the global values that DART tags should use for prefix, site & zone. Example: groups.drupal.org might use gdo.group/general; drupal.org might use drupal.default (notice there is no zone); ',
  );
  $form['settings']['tag_settings']['default_tag_settings']['dart_global_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('DART prefix'),
    '#default_value' => variable_get('dart_global_prefix', ''),
    '#required' => TRUE,
    '#description' => t('Typically this is a short version of your organization\'s name (provided by Doubleclick).'),
    '#maxlength' => 32,
  );
  $form['settings']['tag_settings']['default_tag_settings']['dart_global_site'] = array(
    '#type' => 'textfield',
    '#title' => t('Default DART site'),
    '#default_value' => variable_get('dart_global_site', ''),
    '#required' => TRUE,
    '#description' => t('The value to use when no "site" is specified on a given page.'),
    '#maxlength' => 32,
  );
  $form['settings']['tag_settings']['default_tag_settings']['dart_global_zone'] = array(
    '#type' => 'textfield',
    '#title' => t('Default DART zone'),
    '#default_value' => variable_get('dart_global_zone', ''),
    '#required' => FALSE,
    '#description' => t('The value to use when no "zone" is specified on a given page.'),
    '#maxlength' => 32,
  );
  $form['settings']['tag_settings']['dart_load_last'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('Load ads last'),
    '#default_value'  => variable_get('dart_load_last', '0'),
    '#description'    => t('Wait until the entire page has loaded before loading ads. This will allow your pages to load faster but may cause empty spaces to appear on your pages before the ads load. <strong>You must use this option if you are loading ad tags asyncronously.</strong>'),
  );

  // Global display options.
  $form['settings']['display_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Display Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['display_options']['dart_global_slug'] = array(
    '#type' => 'textfield',
    '#title' => t('Global Slug'),
    '#default_value' => variable_get('dart_global_slug', '<none>'),
    '#required' => FALSE,
    '#description' => t('Slug all ad tags with this label. Use @tag for no slug. Example: Advertisement', array('@tag' => '<none>')),
    '#maxlength' => 64,
  );
  $form['settings']['display_options']['dart_noscript'] = array(
    '#type' => 'checkbox',
    '#title' => t('display @tag tags', array('@tag' => '<noscript>')),
    '#default_value' => variable_get('dart_noscript', '1'),
    '#description' => t('Display @tag tags along with the standard javascript DART tags.', array('@tag' => '<noscript>')),
  );

  // Gloabal key/val pairs
  $form['settings']['global_key_vals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Global Key|Value Pairs'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['global_key_vals']['dart_special_key_vals'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Special key|val pairs'),
    '#default_value' => variable_get('dart_special_key_vals', array('tile' => 'tile', 'ord' => 'ord')),
    '#options' => array(
      'tile' => t('tile (value auto-increments on each tag)'),
      'ord' => t('ord (value changes on each page request)'),
    ),
    '#description' => t('Include these special values in all of your ad tags.'),
    '#weight' => -10,
  );
  $form['settings']['global_key_vals']['dart_special_tile_init'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial value of tile'),
    '#default_value' => variable_get('dart_special_tile_init', '0'),
    '#size' => 1,
    '#weight' => -5,
  );

  // Container for the key_value forms.
  ctools_include('plugins');
  ctools_plugin_load_class('ctools', 'export_ui', 'dart_ctools_export_ui', 'handler');
  $key_vals = variable_get('dart_key_vals', array());
  $key_val_count = max(DART_MAX_KEY_VALS, count($key_vals) + 5);
  $form['settings']['global_key_vals']['dart_kvs'] = array(
    '#theme' => 'dart_key_vals_form',
  );
  for ($i = 0; $i < $key_val_count; $i++) {
    $form['settings']['global_key_vals']['dart_kvs'][$i] = _dart_key_val_form($i, isset($key_vals[$i]) ? $key_vals[$i] : null);
  }

  // Javascript.
  $form['settings']['javascript'] = array(
    '#type' => 'fieldset',
    '#title' => t('Inject Javascript'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 20,
  );
  $form['settings']['javascript']['dart_js'] = array(
    '#type' => 'textarea',
    '#title' => t('Inject javascript'),
    '#description' => t('Inject this javascript into the @tag on each page request. You do not need to add code to create a random number for use as the ORD if you checked "Include ord value" above. This javascript will be injected above the key|value pairs you enter in the field below, so you can declare variables here that you will include as part of a key|value pair there.', array('@tag' => '<head>')),
    '#default_value' => variable_get('dart_js', ''),
    '#rows' => 5,
  );

  // Ad Test Settngs.
  $form['settings']['default_ad_test_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ad Test Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 50,
    '#description' => 'Enter the values that DART tags should use for prefix, site & zone when you add the ?adtest=true query string to any url. Additionally note that you can override these values at any time by adding ?adtest=true &amp;prefix=testprefix &amp;site=testsite &amp;zone=testzone to any url. <div><h3>Examples:</h3><ul><li>mysite.com?adtest=true</li><li>mysite.com?adtest=true&prefix=testprefix</li><li>mysite.com?adtest=true&prefix=testprefix&zone=testzone</li></ul></div>',
  );
  $form['settings']['default_ad_test_settings']['default_test_tag_settings'] = array(
    '#theme' => 'dart_global_defaults_form',
    '#description' => t('The default values to use when the query string ?adtest=true is present in the url.'),
  );
  $form['settings']['default_ad_test_settings']['default_test_tag_settings']['dart_test_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Default test prefix'),
    '#default_value' => variable_get('dart_test_prefix', ''),
    '#description' => t('The prefix to use when the query string ?adtest=true is present.'),
    '#required' => FALSE,
    '#maxlength' => 32,
  );
  $form['settings']['default_ad_test_settings']['default_test_tag_settings']['dart_test_site'] = array(
    '#type' => 'textfield',
    '#title' => t('Default test site'),
    '#default_value' => variable_get('dart_test_site', ''),
    '#description' => t('The site to use when the query string ?adtest=true is present.'),
    '#required' => FALSE,
    '#maxlength' => 32,
  );
  $form['settings']['default_ad_test_settings']['default_test_tag_settings']['dart_test_zone'] = array(
    '#type' => 'textfield',
    '#title' => t('Default test zone'),
    '#default_value' => variable_get('dart_test_zone', ''),
    '#description' => t('The zone to use when the query string ?adtest=true is present.'),
    '#required' => FALSE,
    '#maxlength' => 32,
  );

  return system_settings_form($form);
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function dart_admin_settings_validate($form, &$form_state) {
  // Validate "key_vals" before its name is changed.
  _dart_key_val_form_validate($form, $form_state);

  // Rename the key_vals value to dart_key_vals to avoid namespace conflicts.
  $form_state['values']['dart_key_vals'] = $form_state['values']['key_vals'];
  unset($form_state['values']['key_vals']);
}

/**
 * Theme the dart settings form for global prefix.site/zone.
 */
function theme_dart_global_defaults_form($variables) {
  $form = &$variables['form'];

  // Specify which fields we are theming.
  switch ($form['#parents'][0]) {
    case 'default_tag_settings':
      $mode = 'global';
      $element_name = 'dart_global_prefix_site_zone';
      $fields = array(
        'prefix' => 'dart_global_prefix',
        'site' => 'dart_global_site',
        'zone' => 'dart_global_zone',
      );
      break;
    case 'default_test_tag_settings':
      $mode = 'test';
      $element_name = 'dart_test_prefix_site_zone';
      $fields = array(
        'prefix' => 'dart_test_prefix',
        'site' => 'dart_test_site',
        'zone' => 'dart_test_zone',
      );
      break;
  }

  // Set widths.
  $form[$fields['prefix']]['#attributes']['style'] = 'width: 60px;';
  $form[$fields['site']]['#attributes']['style'] = 'width: 200px;';
  $form[$fields['zone']]['#attributes']['style'] = 'width: 200px;';

  // Store field descriptions so they can be displayed later.
  $descriptions = array();
  foreach ($fields as $name => $field) {
    if (!empty($field['#description'])) {
      $descriptions[] = ucwords($name) . ": " . $form[$field]['#description'];
      unset($form[$field]['#description']);
    }
  }

  // Remove titles.
  unset($form[$fields['prefix']]['#title'], $form[$fields['site']]['#title'], $form[$fields['zone']]['#title']);

  $replacements = array(
    '!prefix' => drupal_render($form[$fields['prefix']]),
    '!site' => drupal_render($form[$fields['site']]),
    '!zone' => drupal_render($form[$fields['zone']]),
  );

  $form[$element_name] = array(
    '#prefix' => '<div class="form-item"><label>' . ucwords($mode) . ' prefix . site / zone</label>',
    '#markup' => '<div class="container-inline">' . t('!prefix . !site / !zone', $replacements) . '</div>',
    "#suffix" => '<div class="description">' . $form['#description'] . theme('item_list', array('items' => $descriptions)) . '</div></div>',
  );

  unset($form[$fields['prefix']]);
  unset($form[$fields['site']]);
  unset($form[$fields['zone']]);

  return drupal_render($form[$element_name]);
}
