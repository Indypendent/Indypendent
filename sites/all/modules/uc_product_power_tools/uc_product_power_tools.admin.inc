<?php

/**
 * @file
 * Settings and config for uc_product_power_tools module.
 */

/**
 * Administration settings form.
 *
 * @see uc_product_power_tools_admin_settings_submit()
 * @see theme_uc_product_power_tools_admin_settings()
 * @ingroup forms
 */
function uc_product_power_tools_admin_settings() {
  // Get a list of all product classes.
  $productclasses = '';
  $enabledclasses = '';
  $result = db_query('SELECT pcid, name from {uc_product_classes}');
  while ($classes = $result->fetchAssoc()) {
    $productclasses[$classes['pcid']] = $classes['name'];
    $enabledclasses[$classes['pcid']] = FALSE;
  }

  // Add entry for Default Products to end of the list:
  $productclasses['product'] = 'Default Ubercart Product Content Type';
  $enabledclasses['product'] = FALSE;

  // Get a list of enabled classes.
  $result = db_query('SELECT pcid, enabled from {uc_power_tools}');
  while ($classes = $result->fetchAssoc()) {
    $enabledclasses[$classes['pcid']] = $classes['enabled'];
  }

  $form = array('#tree' => TRUE);
  $form['content'] = array(
    '#markup' => t('<p>Product "Power Tools" give you a number of options for altering the product creation form.  This is useful when you have product classes that do not require all of the possible product fields.  Unneeded fields can be disabled and hidden, or preset and hidden.</p><p>Tweakable options:<ul><li>Auto Generate SKU</li><li>List Position</li><li>List Price &amp; Cost</li><li>Default Quantity &amp; Package Quantity</li><li>Shippable (can hide other shipping fields for non-shippable products)</li><li>Set default stock levels (If stock module is installed)</li></ul>'),
  );

  foreach ($productclasses as $pcid => $classname) {
    $form['classes'][$pcid] = array(
      '#type'          => 'radios',
      '#title'         => $classname,
      '#title_display' => 'invisible',
      '#options'       => array(TRUE => t('Enabled'), FALSE => t('Disabled')),
      '#default_value' => $enabledclasses[$pcid],
    );
  }

  $form['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Update'),
  );

  return $form;
}

/**
 * Themes uc_product_power_tools_admin_settings() form.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @see uc_product_power_tools_admin_settings()
 * @ingroup themeable
 */
function theme_uc_product_power_tools_admin_settings($variables) {
  $form = $variables['form'];

  $header = array(
    t('Product Class'),
    t('"Power Tools" Status'),
    t('Actions'),
  );

  $row = array();
  foreach (element_children($form['classes']) as $element) {
    $classname = $form['classes'][$element]['#title'];
    $row[] = array(
      l($classname, 'admin/store/products/power-tools/' . $element),
      drupal_render($form['classes'][$element]),
      l(t('Edit'), 'admin/store/products/power-tools/' . $element),
    );
  }
   //   array('data' => drupal_render($form['status'][$module]), 'align' => 'center'),
  $output  = theme('table', array('header' => $header, 'rows' => $row));
  $output .= drupal_render($form['submit']);
  $output  = drupal_render_children($form) . $output;

  return $output;
}

/**
 * Submit handler for uc_product_power_tools_admin_settings() form.
 *
 * @see uc_product_power_tools_admin_settings()
 */
function uc_product_power_tools_admin_settings_submit($form, &$form_state) {
  foreach ($form_state['values']['classes'] as $key => $value) {
    // Check each class to see if it's in the db, if not add it as disabled.
    // If it's in the db, update its status to whatever was selected.
    $result = db_query('SELECT * FROM {uc_power_tools} WHERE pcid = :pcid', array(':pcid' => $key));
    if ($result->fetchAssoc()) {
      db_update('uc_power_tools')
        ->fields(array('enabled' => $value))
        ->condition('pcid', $key, '=')
        ->execute();
    }
    else {
      db_insert('uc_power_tools')
        ->fields(array('pcid' => $key, 'enabled' => $value))
        ->execute();
    }
  }
  drupal_set_message(t('Power Tools Class Settings updated.'));
}

/**
 * Administration settings form.
 *
 * @see uc_product_power_tools_class_settings()
 * @ingroup forms
 */
function uc_product_power_tools_class_settings() {
  $tmp = explode("/", $_GET['q']);
  $prodclass = $tmp[4];
  $data = db_query('SELECT * FROM {uc_power_tools} WHERE pcid = :pcid', array(':pcid' => $prodclass))->fetchAssoc();
  $class = ($prodclass != 'product') ? db_query('SELECT name from {uc_product_classes} WHERE pcid = :pcid', array(':pcid' => $prodclass))->fetchAssoc() : array('name' => 'Default Ubercart Product Content Type');
  $sign_flag = variable_get('uc_sign_after_amount', FALSE);
  $currency_sign = variable_get('uc_currency_sign', '$');

  // ------------
  // Issue #883544
  // Feature Request
  // Ability to clone an existing product class' settings.
  //

  $form = array();

  $form['clone_class'] = array(
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
      '#title'       => t('Clone Settings of Existing Product Class'),
  );
  $form['clone_class']['clone_from'] = array(
      '#type'          => 'radios',
      '#default_value' => 0,
      '#options'       => _get_available_clone_classes($prodclass),
  );
  $form['clone_class']['clone_submit'] = array(
      '#type'     => 'submit',
      '#value'    => t('Clone'),
  );

  // ------------

  $form['product_class'] = array(
    '#type'   => 'hidden',
    '#value'  => $prodclass,
  );
  $form['power_tools'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('@class "Power Tools" Settings', array('@class' => $class['name'])),
    '#collapsible' => FALSE,
  );
  $form['power_tools']['autosku'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Auto SKU Generation'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,
  );
  $form['power_tools']['autosku']['asku'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['asku']) ? $data['asku'] : 0,
    '#options'       => array(
      0 => t('Show SKU field on product entry form'),
      2 => t('Show SKU field, but for new products give the option to Automatically Generate from replacement pattern below:'),
      3 => t('Disable (show but do not allow editing) SKU field and Automatically Generate from replacement pattern below:'),
      1 => t('Hide SKU field and Automatically Generate from replacement pattern below:'),
    )
  );

  $description = t('This string will be used as SKU.');
  $description .= ' ' . t('Use the syntax [token] if you want to insert a replacement pattern.<br />Note: CCK Fields that are not assigned to products in this product class will return no value. If using the Auto Node Title module, [title] and [title-raw] will return "ant".');
  $form['power_tools']['autosku']['asku_pattern'] = array(
    '#type'          => 'textarea',
    '#title'         => t('Pattern for the SKU'),
    '#description'   => $description,
    '#default_value' => isset($data['asku_settings']) ? $data['asku_settings'] : '',
  );

  if (module_exists('token')) {
    $form['power_tools']['autosku']['token_help'] = array(
      '#title'       => t('Replacement patterns'),
      '#type'        => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed'   => TRUE,
      '#description' => t('Prefer raw-text replacements for text to avoid problems with HTML entities!'),
    );
    $form['power_tools']['autosku']['token_help']['help'] = array(
      '#markup' => theme('token_tree', array('token_types' => 'all', 'click_insert' => FALSE, 'show_restricted' => TRUE)),
    );
  }
  $form['power_tools']['pricing'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Prices'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['pricing']['lp'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('List Price'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['pricing']['lp']['lponoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['lp']) ? $data['lp'] : 0,
    '#options'       => array(
      t('Show List Price field on product entry form'),
      t('Hide List Price field & Use Sell Price Value entered on product form'),
      t('Hide List Price field & Use default value below:'),
      t('Show List Price field & Use default value below:'),
    )
  );
  $form['power_tools']['pricing']['lp']['lp_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('List price'),
    '#required'      => FALSE,
    '#default_value' => isset($data['lp_settings']) ? $data['lp_settings'] : 0,
    '#size'          => 20,
    '#maxlength'     => 35,
    '#field_prefix'  => $sign_flag ? '' : $currency_sign,
    '#field_suffix'  => $sign_flag ? $currency_sign : '',
  );
  $form['power_tools']['pricing']['cost'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Cost'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['pricing']['cost']['costonoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['cost']) ? $data['cost'] : 0,
    '#options'       => array(
      t('Show Cost field on product entry form'),
      t('Hide Cost field & Use default value below:'),
      t('Show Cost field & Use default value below:'),
    )
  );
  $form['power_tools']['pricing']['cost']['cost_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('Cost'),
    '#required'      => FALSE,
    '#default_value' => isset($data['cost_settings']) ? $data['cost_settings'] : 0,
    '#size'          => 20,
    '#maxlength'     => 35,
    '#field_prefix'  => $sign_flag ? '' : $currency_sign,
    '#field_suffix'  => $sign_flag ? $currency_sign : '',
  );
  $form['power_tools']['pricing']['sp'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Sell Price'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['pricing']['sp']['sponoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['sp']) ? $data['sp'] : 0,
    '#options'       => array(
      t('Show Sell Price field on product entry form'),
      t('Hide Sell Price field & Use default value below:'),
      t('Show Sell Price field & Use default value below:'),
    )
  );
  $form['power_tools']['pricing']['sp']['sp_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('Sell price'),
    '#required'      => FALSE,
    '#default_value' => isset($data['sp_settings']) ? $data['sp_settings'] : 0,
    '#size'          => 20,
    '#maxlength'     => 35,
    '#field_prefix'  => $sign_flag ? '' : $currency_sign,
    '#field_suffix'  => $sign_flag ? $currency_sign : '',
  );
  $form['power_tools']['qtys'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Quantities'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['qtys']['dq'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Default Quantity'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['qtys']['dq']['dqonoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['dq']) ? $data['dq'] : 0,
    '#options'       => array(
      t('Show Default Quantity field on product entry form'),
      t('Hide Default Quantity field & Use default value below:'),
      t('Show Default Quantity field & Use default value below:'),
    )
  );
  $form['power_tools']['qtys']['dq']['dq_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('Default Quantity'),
    '#description'   => t('Default quantity to add to cart'),
    '#required'      => FALSE,
    '#default_value' => isset($data['dq_settings']) ? $data['dq_settings'] : 1,
    '#size' => 20,
  );
  $form['power_tools']['qtys']['pq'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Package Quantity'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['qtys']['pq']['pqonoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['pq']) ? $data['pq'] : 0,
    '#options'       => array(
      t('Show Default Package Quantity field on product entry form'),
      t('Hide Default Package Quantity field & Use default value below:'),
      t('Show Default Package Quantity field & Use default value below:'),
    )
  );
  $form['power_tools']['qtys']['pq']['pq_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('Package Quantity'),
    '#description'   => t('For a package containing only this product, how many are in it?'),
    '#required'      => FALSE,
    '#default_value' => isset($data['pq_settings']) ? $data['pq_settings'] : 1,
    '#size'          => 20,
  );
  $form['power_tools']['lpos'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('List Position'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['lpos']['lposonoff'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['lpos']) ? $data['lpos'] : 0,
    '#options'       => array(
      t('Show List Position field on product entry form'),
      t('Hide List Position field & Use default value below:'),
    )
  );
  $form['power_tools']['lpos']['lpos_settings'] = array(
    '#type'          => 'textfield',
    '#size'          => 32,
    '#title'         => t('List Position'),
    '#description'   => t('List Position weight of products of this class.'),
    '#required'      => FALSE,
    '#default_value' => isset($data['lpos_settings']) ? $data['lpos_settings'] : 0,
    '#size'          => 20,
  );
  $form['power_tools']['ship'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Shipping'),
    '#description'  => t('The last option will only hide weight and size fields.'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['ship']['ship_settings'] = array(
    '#type'          => 'radios',
    '#default_value' => isset($data['ship']) ? $data['ship'] : 0,
    '#options'       => array(
      t('Product is Shippable. Show checkbox & all shipping fields on product entry form'),
      t('Product is Shippable. Hide checkbox, but show other shipping fields on product entry form'),
      t('Product is NOT Shippable. Hide checkbox, but show other shipping fields on product entry form'),
      t('Product is NOT Shippable. Hide checkbox & all shipping fields on product entry form'),
      t('Show checkbox but hide all shipping fields on product entry form')
    ),
  );
  $form['power_tools']['ship']['weight'] = array(
    '#weight' => 15,
    '#theme' => 'uc_product_form_weight',
  );
  $form['power_tools']['ship']['weight']['weight'] = array('#type' => 'textfield',
    '#title' => t('Weight'),
    '#default_value' => isset($data['weight']) ? $data['weight'] : 0,
    '#size' => 10,
    '#maxlength' => 15,
  );
  $units = array(
    'lb' => t('Pounds'),
    'kg' => t('Kilograms'),
    'oz' => t('Ounces'),
    'g'  => t('Grams'),
  );
  $form['power_tools']['ship']['weight']['weight_units'] = array(
    '#type'          => 'select',
    '#title'         => t('Unit of measurement'),
    '#default_value' => isset($data['weight_units']) ? $data['weight_units'] : variable_get('uc_weight_unit', 'lb'),
    '#options'       => $units,
  );
  $form['power_tools']['ship']['dimensions'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Dimensions'),
    '#description'   => t('Physical dimensions of the packaged product.'),
    '#weight'        => 20,
    '#theme'         => 'uc_product_dimensions_form',
  );
  $form['power_tools']['ship']['dimensions']['length_units'] = array(
    '#type'          => 'select',
    '#title'         => t('Units of measurement'),
    '#options'       => array(
      'in' => t('Inches'),
      'ft' => t('Feet'),
      'cm' => t('Centimeters'),
      'mm' => t('Millimeters'),
    ),
    '#default_value' => isset($data['length_units']) ? $data['length_units'] : variable_get('uc_length_unit', 'in'),
  );
  $form['power_tools']['ship']['dimensions']['length'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Length'),
    '#default_value' => isset($data['length']) ? $data['length'] : 0,
    '#size'          => 10,
  );
  $form['power_tools']['ship']['dimensions']['width'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Width'),
    '#default_value' => isset($data['width']) ? $data['width'] : 0,
    '#size'          => 10,
  );
  $form['power_tools']['ship']['dimensions']['height'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Height'),
    '#default_value' => isset($data['height']) ? $data['height'] : 0,
    '#size'          => 10,
  );

  $stock_disabled = !module_exists('uc_stock');
  $form['power_tools']['stock'] = array(
    '#type'          => 'fieldset',
    '#title'         => t('Stock'),
    '#collapsible'   => TRUE,
    '#collapsed'     => TRUE,
  );
  $form['power_tools']['stock']['stockonoff'] = array(
    '#type'          => 'radios',
    '#options'       => array(
      t('Do not set stock level'),
      t('Set stock level using default value below'),
    ),
    '#default_value' => isset($data['stock']) ? $data['stock'] : 0,
    '#description'   => t('If setting a default stock level, stock will be set to active for products of this type.'),
    '#disabled'      => $stock_disabled,
  );
  $form['power_tools']['stock']['stock_settings'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Default Stock'),
    '#description'   => t('Default Stock level for products of this class.'),
    '#required'      => FALSE,
    '#default_value' => isset($data['stock_settings']) ? $data['stock_settings'] : 0,
    '#size'          => 20,
    '#disabled'      => $stock_disabled,
  );
  $form['power_tools']['stock']['stock_threshold'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Notification Threshold'),
    '#description'   => t('When stock drops below this level you will be notified.'),
    '#required'      => FALSE,
    '#default_value' => isset($data['stock_threshold']) ? $data['stock_threshold'] : 0,
    '#size'          => 20,
    '#disabled'      => $stock_disabled,
  );
  $form['submit'] = array(
    '#type'     => 'submit',
    '#value'    => t('Submit'),
    '#weight'   => 10,
  );

  return $form;
}

/**
 * Submit handler for uc_product_power_tools_class_settings() form.
 *
 * @see uc_product_power_tools_class()
 */
function uc_product_power_tools_class_settings_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Clone')) {
      if ($form_state['values']['clone_from']) {
        uc_product_power_tools_clone_class($form_state['values']['clone_from'], $form_state['values']['product_class']);
        drupal_goto('admin/store/products/power-tools/' . $form_state['values']['product_class']);
        die();
      }
  }
  $pcid           = $form_state['values']['product_class'];
  $enabled        = 1;
  $asku           = isset($form_state['values']['asku']) ? $form_state['values']['asku'] : ' ';
  $askusettings   = $form_state['values']['asku_pattern'];
  $lp             = $form_state['values']['lponoff'];
  $lpsettings     = $form_state['values']['lp_settings'];
  $sp             = $form_state['values']['sponoff'];
  $spsettings     = $form_state['values']['sp_settings'];
  $cost           = $form_state['values']['costonoff'];
  $costsettings   = $form_state['values']['cost_settings'];
  $dq             = $form_state['values']['dqonoff'];
  $dqsettings     = $form_state['values']['dq_settings'];
  $pq             = $form_state['values']['pqonoff'];
  $pqsettings     = $form_state['values']['pq_settings'];
  $ship           = $form_state['values']['ship_settings'];
  $lpos           = $form_state['values']['lposonoff'];
  $lpossettings   = $form_state['values']['lpos_settings'];
  $stock          = $form_state['values']['stockonoff'];
  $stocksettings  = $form_state['values']['stock_settings'];
  $stockthreshold = $form_state['values']['stock_threshold'];
  $weight         = $form_state['values']['weight'];
  $weight_units   = $form_state['values']['weight_units'];
  $length_units   = $form_state['values']['length_units'];
  $length         = $form_state['values']['length'];
  $width          = $form_state['values']['width'];
  $height         = $form_state['values']['height'];

  db_query('REPLACE INTO {uc_power_tools} (pcid, enabled, asku, asku_settings, lp, lp_settings, cost, cost_settings, sp, sp_settings, dq, dq_settings, pq, pq_settings, ship, weight, weight_units, length_units, length, width, height, lpos, lpos_settings, stock, stock_settings, stock_threshold) VALUES (:settings)',
    array(':settings' => array($pcid, $enabled, $asku, $askusettings, $lp, $lpsettings, $cost, $costsettings, $sp, $spsettings, $dq, $dqsettings, $pq, $pqsettings, $ship, $weight, $weight_units, $length_units, $length, $width, $height, $lpos, $lpossettings, $stock, $stocksettings, $stockthreshold)));

  drupal_set_message(t('Power Tools settings saved.'));
  drupal_goto('admin/store/products/power-tools');
}

/**
 * Copies settings from an existing product class.
 */
function uc_product_power_tools_clone_class($clone_from, $clone_to) {
  $query = db_query('SELECT * FROM {uc_power_tools} WHERE pcid = :pcid', array(':pcid' => $clone_from));
  $existing = $query->fetchAssoc();

  $pcid           = $clone_to;
  $enabled        = 1;
  $asku           = $existing['asku'];
  $askusettings   = $existing['asku_settings'];
  $lp             = $existing['lp'];
  $lpsettings     = $existing['lp_settings'];
  $sp             = $existing['sp'];
  $spsettings     = $existing['sp_settings'];
  $cost           = $existing['cost'];
  $costsettings   = $existing['cost_settings'];
  $dq             = $existing['dq'];
  $dqsettings     = $existing['dq_settings'];
  $pq             = $existing['pq'];
  $pqsettings     = $existing['pq_settings'];
  $ship           = $existing['ship'];
  $lpos           = $existing['lpos'];
  $lpossettings   = $existing['lpos_settings'];
  $stock          = $existing['stock'];
  $stocksettings  = $existing['stock_settings'];
  $stockthreshold = $existing['stock_threshold'];
  $weight         = $existing['weight'];
  $weight_units   = $existing['weight_units'];
  $length_units   = $existing['length_units'];
  $length         = $existing['length'];
  $width          = $existing['width'];
  $height         = $existing['height'];

  db_query('REPLACE INTO {uc_power_tools} (pcid, enabled, asku, asku_settings, lp, lp_settings, cost, cost_settings, sp, sp_settings, dq, dq_settings, pq, pq_settings, ship, weight, weight_units, length_units, length, width, height, lpos, lpos_settings, stock, stock_settings, stock_threshold) VALUES (:settings)',
    array(':settings' => array($pcid, $enabled, $asku, $askusettings, $lp, $lpsettings, $cost, $costsettings, $sp, $spsettings, $dq, $dqsettings, $pq, $pqsettings, $ship, $weight, $weight_units, $length_units, $length, $width, $height, $lpos, $lpossettings, $stock, $stocksettings, $stockthreshold)));

  drupal_set_message(t('Power Tools settings have been cloned.'));
}

/**
 * Helper function to get available product classes for cloning of settings.
 */
function _get_available_clone_classes($product_class) {
  $options = array();

  $query = db_query('SELECT pt.pcid as pcid, nt.name as name FROM {uc_power_tools} as pt, {node_type} as nt WHERE pt.pcid = nt.type AND pt.pcid <> :pcid ORDER BY nt.name', array(':pcid' => $product_class));
  while ($result = $query->fetchAssoc()) {
    if ($result['pcid'] <> 'product') {
      $options[$result['pcid']] = $result['name'];
    }
    else {
      $options[$result['pcid']] = t('Default Ubercart Product Content Type');
    }
  }

  return $options;
}
