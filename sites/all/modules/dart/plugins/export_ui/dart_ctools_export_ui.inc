<?php

/**
 * @file
 * A Ctools Export UI plugin for DART Tags.
 */

/**
 * Define this Export UI plugin.
 */
$plugin = array(
  'schema' => 'dart_tags',
  'access' => 'administer DART tags',
  'create access' => 'administer DART tags',
  'delete access' => 'administer DART tags',

  // Define the menu item.
  'menu' => array(
    'menu item' => 'dart_tags',
    'menu prefix' => 'admin/structure',
    'menu title' => 'DART Tags',
    'menu description' => 'Administer DART tags.',
  ),
  // Define user interface texts.
  'title singular' => t('tag'),
  'title plural' => t('tags'),
  'title singular proper' => t('DART tag'),
  'title plural proper' => t('DART tags'),
  // Define the class to use as a handler for DART tags.
  'handler' => array(
    'class' => 'dart_tag_ui',
    'parent' => 'ctools_export_ui',
  ),
  // Define the names of the functions that provide the add/edit forms.
  'form' => array(
    'settings' => 'dart_tag_form',
    'submit' => 'dart_tag_form_submit',
    'validate' => 'dart_tag_form_validate',
  ),
);

/**
 * Form builder; Create an ad tag.
 */
function dart_tag_form(&$form, &$form_state) {
  $tag = $form_state['item'];


  $form['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Make this tag active.'),
    '#default_value' => $tag->active,
  );

  $form['settings'] = array(
    '#type' => 'vertical_tabs',
  );

  // Tag settings.
  $form['settings']['tag_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tag Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['tag_settings']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('A human readable name for this ad tag.'),
    '#required' => TRUE,
    '#maxlength' => 128,
    '#default_value' => $tag->name,
  );
  $form['settings']['tag_settings']['machinename'] = $form['info']['machinename'];
  $form['settings']['tag_settings']['machinename']['#title'] = t('Machine Name');
  unset($form['info']);
  $form['settings']['tag_settings']['pos'] = array(
    '#type' => 'textfield',
    '#title' => t('Position (pos)'),
    '#description' => t('Example: rt_rail_box1'),
    '#required' => TRUE,
    '#maxlength' => 32,
    '#default_value' => $tag->pos,
  );
  $form['settings']['tag_settings']['sz'] = array(
    '#type' => 'textfield',
    '#title' => t('Size (sz)'),
    '#description' => t('Example: 300x600,300x250'),
    '#required' => TRUE,
    '#maxlength' => 64,
    '#default_value' => $tag->sz,
  );

  // Tag settings overrides.
  $form['settings']['tag_settings']['overrides'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default DART Setting Overrides'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Override the !settings for this ad tag.', array('!settings' => l(t('default settings'), 'admin/configure/dart'))),
  );
  $default_site = variable_get('dart_global_site', '');
  $form['settings']['tag_settings']['overrides']['site'] = array(
    '#type' => 'textfield',
    '#title' => t('Site'),
    '#description' => t('In rare circumstances you may want to override the !settings: %default', array('!settings' => l(t('default DART site'), 'admin/configure/dart'), '%default' => empty($default_site) ? t('No default value has been set.') : $default_site)),
    '#required' => FALSE,
    '#maxlength' => 32,
    '#default_value' => $tag->settings['overrides']['site'],
  );
  $default_zone = variable_get('dart_global_zone', '');
  $form['settings']['tag_settings']['overrides']['zone'] = array(
    '#type' => 'textfield',
    '#title' => t('Zone'),
    '#description' => t('In rare circumstances you may want to override the !settings: %default', array('!settings' => l(t('default DART zone'), 'admin/configure/dart'), '%default' => empty($default_zone) ? t('No default value has been set.') : $default_zone)),
    '#required' => FALSE,
    '#maxlength' => 32,
    '#default_value' => $tag->settings['overrides']['zone'],
  );
  $form['settings']['tag_settings']['overrides']['slug'] = array(
    '#type' => 'textfield',
    '#title' => t('Slug'),
    '#description' => t('Override the default slug for this ad tag. Use !none for no slug. Leave this field empty to use the default slug. Example: Advertisement', array('!none' => check_plain('<none>'))),
    '#required' => FALSE,
    '#maxlength' => 64,
    '#default_value' => $tag->settings['overrides']['slug'],
  );

  // Display options.
  $form['settings']['display_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['display_options']['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create a block for this tag'),
    '#description' => t('Create a block (configurable at admin/build/block) in which this ad tag is displayed.'),
    '#default_value' => $tag->block,
  );
  $form['settings']['display_options']['scriptless'] = array(
    '#type' => 'checkbox',
    '#title' => t('Render this tag without javascript'),
    '#description' => t('Use this option for DART tags included in emails.'),
    '#default_value' => $tag->settings['options']['scriptless'],
  );
  $form['settings']['display_options']['method'] = array(
    '#type' => 'radios',
    '#title' => t('Output Method'),
    '#description' => t('If you are unsure, use "adj" and/or contact doubleclick.'),
    '#options' => array(
      'adj' => t('&lt;script&gt; (adj)'),
      'adi' => t('&lt;iframe&gt; (adi)'),
    ),
    '#default_value' => !empty($tag->settings['options']['method']) ? $tag->settings['options']['method'] : 'adj',
    '#weight' => -1,
  );

  // Key/val pairs.
  // @todo: add the ability to dynamically add more key|val form elements (D7)
  $key_val_count = max(DART_MAX_KEY_VALS, count($tag->settings['key_vals']) + 5);

  $form['settings']['key_vals_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Key|Value Pairs'),
    '#attributes' => array('#id' => 'key_vals'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['settings']['key_vals_group']['key_vals_form_count'] = array(
    '#type' => 'hidden',
    '#value' => $key_val_count,
  );
  // Container for the key_value forms.
  $form['settings']['key_vals_group']['key_vals'] = array(
    '#theme' => 'dart_key_vals_form',
  );
  for ($i = 0; $i < $key_val_count; $i++) {
    $form['settings']['key_vals_group']['key_vals'][$i] = _dart_key_val_form($i, isset($tag->settings['key_vals'][$i]) ? $tag->settings['key_vals'][$i] : null);
  }
}

/**
 * Form validation for the dart_tag form.
 */
function dart_tag_form_validate($form, &$form_state) {
  _dart_key_val_form_validate($form, $form_state);
}

/**
 * Form submission for the dart_tag form.
 */
function dart_tag_form_submit($form, &$form_state) {
  if (count($form_state['values']['key_vals']) >= $form_state['values']['key_vals_form_count']) {
    drupal_set_message('It appears that you ran out of key|value pair form fields. More fields will be added the next time you edit this tag.');
  }
}

/**
 * Form builder; Create the key|value pair subform.
 */
function _dart_key_val_form($delta, $defaults = array()) {
  $form = array(
    '#tree' => TRUE,
  );

  $form['key'] = array(
    '#type' => 'textfield',
    '#size' => '35',
    '#title' => t('Key'),
    '#default_value' => isset($defaults['key']) ? $defaults['key'] : '',
    '#parents' => array('key_vals', $delta, 'key'),
  );

  $form['val'] = array(
    '#type' => 'textfield',
    '#size' => '35',
    '#title' => t('Value'),
    '#default_value' => isset($defaults['val']) ? $defaults['val'] : '',
    '#parents' => array('key_vals', $delta, 'val'),
  );
  $form['eval'] = array(
    '#type' => 'checkbox',
    '#title' => t('Evaluate as javascript'),
    '#default_value' => isset($defaults['eval']) ? $defaults['eval'] : '',
    '#parents' => array('key_vals', $delta, 'eval'),
  );

  return $form;
}

/**
 * Form validation for any form that includes fields created by
 * dart_key_val_form().
 */
function _dart_key_val_form_validate($form, &$form_state) {
  // Check that each key|val pair has both a key and val.
  foreach ($form_state['values']['key_vals'] as $i => $key_vals) {
    // Remove any empty key|val pairs (even if "Evaluate as js" is checked").
    if (empty($key_vals['key']) && empty($key_vals['val'])) {
      unset($form_state['values']['key_vals'][$i]);
      continue;
    }
    if (!empty($key_vals['key']) && empty($key_vals['val'])) {
      form_set_error('key_vals][' . $i . '][val', t('If you enter a key you must also provide a val.'));
    }
    if (!empty($key_vals['val']) && empty($key_vals['key'])) {
      form_set_error('key_vals][' . $i . '][key', t('If you enter a val you must also provide a key.'));
    }
  }

  // Re index the key_vals array in case some were removed.
  $form_state['values']['key_vals'] = array_values($form_state['values']['key_vals']);
}

/**
 * Theme the dart tag form for key value pairs.
 */
function theme_dart_key_vals_form($variables) {
  $form = &$variables['form'];
  $rows = array();
  $headers = array(
    t('Key'),
    t('Value'),
    t('Evaluate as javascript'),
  );

  foreach (element_children($form) as $key) {
    // No need to print the field title every time.
    unset($form[$key]['key']['#title'], $form[$key]['val']['#title'], $form[$key]['eval']['#title']);

    // Build the table row.
    $row = array(
      'data' => array(
        array(
          'data' => drupal_render($form[$key]['key']),
          'class' => 'key_vals-key',
        ),
        array(
          'data' => drupal_render($form[$key]['val']),
          'class' => 'key_vals-val',
        ),
        array(
          'data' => drupal_render($form[$key]['eval']),
          'class' => 'key_vals-eval',
          'style' => 'text-align: center',
        ),
      ),
    );

    // Add additional attributes to the row, such as a class for this row.
    if (isset($form[$key]['#attributes'])) {
      $row = array_merge($row, $form[$key]['#attributes']);
    }
    $rows[] = $row;
  }

  $output = theme('table', array('header' => $headers, 'rows' => $rows));
  $output .= drupal_render_children($form);
  return $output;
}
