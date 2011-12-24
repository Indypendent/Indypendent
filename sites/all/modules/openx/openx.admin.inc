<?php

/**
 * @author Gerd Riesselmann
 * @author Jeff Warrington (jaydub) is new maintainer March 2008
 * @author Chris Nutting <Chris.Nutting@openx.org>
 * @author Bruno Massa
 *
 * @file
 * Module settings.
 */

/**
 * Module settings page form.
 */
function _openx_settings($form, &$form_state) {
  $form['blocks'] = array(
    '#description'  => t('<ul>
        <li>' . t('The zone ID can be found in the OpenX administration interface.') . '</li>
        <li>' . t('The name can be any alpha-numeric string, this will be used for the block name.') . '</li>
    </ul>'),
    '#title'        => t('OpenX blocks'),
    '#type'         => 'fieldset',
    '#prefix' => '<div id="zones-wrapper">',
    '#suffix' => '</div>'
  );

  $form['blocks']['openx_zones'] = array(
    '#tree'         => TRUE,
    '#theme'        => 'openx_settings_zones',
  );

  if (empty($form_state['zones'])) {
    $zones = variable_get('openx_zones', array());
    if (empty($zones)) {
      $form_state['zones'] = array_fill(0, 3, array('id' => '', 'name' => ''));
    }
    else {
      $form_state['zones'] = $zones;
    }
  }

  foreach ($form_state['zones'] as $index => $zone) {
    $form['blocks']['openx_zones'][$index]['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('_openx_settings_delete_zone'),
      '#ajax' => array(
        'callback' => '_openx_settings_add_zone_callback',
        'wrapper' => 'zones-wrapper',
      ),
      '#zone_id' => $index,
      '#name' => 'delete-zone-' . $index,
    );
    $form['blocks']['openx_zones'][$index]['id'] = array(
      '#type'           => 'textfield',
      '#default_value'  => isset($form_state['zones'][$index]['id']) ? $form_state['zones'][$index]['id'] : '',
      '#size'           => 6,
    );
    $form['blocks']['openx_zones'][$index]['name'] = array(
      '#type'           => 'textfield',
      '#default_value'  => isset($form_state['zones'][$index]['name']) ? $form_state['zones'][$index]['name'] : '',
    );
  }

  $form['blocks']['add_zone'] = array(
    '#type' => 'submit',
    '#value' => t('Add zone'),
    '#submit' => array('_openx_settings_add_zone'),
    '#ajax' => array(
      'callback' => '_openx_settings_add_zone_callback',
      'wrapper' => 'zones-wrapper'
    ),
  );

  $form['targeting'] = array(
    '#type' => 'fieldset',
    '#title' => t('Site Variables'),
    '#description' => t('Specify site variables to allow OpenX to target ads to particular pages.'),
  );
  
  $form_state['site_vars'] = isset($form_state['site_vars']) ? $form_state['site_vars'] : variable_get('openx_site_vars', array_fill(0, 3, array('key' => '', 'value' => '')));
  if (empty($form_state['site_vars'])) {
    $form_state['site_vars'] = array_fill(0, 3, array('key' => '', 'value' => ''));
  }
  $form['targeting']['openx_site_vars'] = array(
     '#tree' => TRUE,
     '#element_validate' => array('_openx_settings_site_vars_clean'),
     '#theme' => 'openx_site_vars',
     '#prefix' => '<div id="openx-site-vars-wrapper">',
     '#suffix' => '</div>',
  );
  
  foreach ($form_state['site_vars'] as $key => $vars) {
    $form['targeting']['openx_site_vars'][$key]['key'] = array(
      '#type' => 'textfield',
      '#default_value' => $vars['key'],
      '#size' => 16,
    );
    $form['targeting']['openx_site_vars'][$key]['value'] = array(
      '#type' => 'textfield',
      '#default_value' => $vars['value'],
      '#size' => 64,
    );
    $form['targeting']['openx_site_vars'][$key]['delete'] = array(
      '#type' => 'submit',
      '#value' => t('delete'),
      '#submit' => array('_openx_settings_del_variable'),
      '#name' => 'openx-site-vars-delete-' . $key,
      '#key' => $key,
      '#disabled' => count($form_state['site_vars']) == 1,
      '#ajax' => array(
        'callback' => '_openx_settings_add_variable_callback',
        'wrapper' => 'openx-site-vars-wrapper'
      ),
    );
  }
  
  $form['targeting']['add_row'] = array(
    '#type' => 'submit',
    '#value' => t('Add variable'),
    '#submit' => array('_openx_settings_add_variable'),
    '#ajax' => array(
      'callback' => '_openx_settings_add_variable_callback',
      'wrapper' => 'openx-site-vars-wrapper'
    ),
  );
  $form['targeting']['token_vars_tree'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array('node', 'user'),
  );

  $form['adserver'] = array(
    '#collapsible'    => TRUE,
    '#collapsed'      => variable_get('openx_delivery_url', FALSE),
    '#type'           => 'fieldset',
    '#title'          => t('OpenX server'),
    '#description'    => t('These settings are located on your OpenX server at <strong>Settings -> Main Settings -> Delivery Settings</strong><br />'),
  );
  $form['adserver']['openx_delivery_url'] = array(
    '#type'           => 'textfield',
    '#default_value'  => isset($form_state['values']['openx_delivery_url']) ? $form_state['values']['openx_delivery_url'] : variable_get('openx_delivery_url', 'd.openx.org'),
    '#title'          => t('OpenX delivery url'),
    '#description'    => t('For example "ads.example.org/delivery". "http://" is automatically prefixed.'),
    '#required'       => TRUE,
  );
  $form['adserver']['openx_delivery_url_https'] = array(
    '#type'           => 'textfield',
    '#default_value'  => isset($form_state['values']['openx_delivery_url_https']) ? $form_state['values']['openx_delivery_url_https'] : variable_get('openx_delivery_url_https', 'd.openx.org'),
    '#title'          => t('OpenX https delivery url'),
    '#description'    => t('For example "ads.example.org/delivery". "https://" is automatically prefixed.'),
    '#required'       => TRUE,
  );

  $form['#submit'] = array('_openx_settings_submit');
  return system_settings_form($form);
}

function _openx_settings_submit(&$form, &$form_state) {
  unset($form_state['storage']);
}

function _openx_settings_add_zone_callback($form, &$form_state) {
  return $form['blocks'];
}

function _openx_settings_add_zone($form, &$form_state) {
  $form_state['zones'][] = array('id' => '', 'name' => '');
  $form_state['rebuild'] = TRUE;
}

function _openx_settings_delete_zone($form, &$form_state) {
  unset($form_state['zones'][$form_state['clicked_button']['#zone_id']]);
  if (count($form_state['zones']) < 3) {
    $form_state['zones'][] = array('id' => '', 'name' => '');
  }
  $form_state['rebuild'] = TRUE;
}

function _openx_settings_site_vars_clean(&$form, &$form_state) {
  $errors = FALSE;

  $site_vars = $form_state['values']['openx_site_vars'];
  foreach ($site_vars as $key => $var) {
    if (empty($var['key']) && empty($var['value'])) {
      unset($site_vars[$key]);
    }
    elseif (empty($var['key']) && !empty($var['value'])) {
      $errors = TRUE;
      form_error($form[$key]['key'], t('Key field is required when there is a value present'));
    }
  }
  
  if (!$errors) {
    form_set_value($form, $site_vars, $form_state);
  }
}

function _openx_settings_del_variable(&$form, &$form_state) {
  unset($form_state['site_vars'][$form_state['clicked_button']['#key']]);
  $form_state['rebuild'] = 1;
}

function _openx_settings_add_variable(&$form, &$form_state) {
  $form_state['site_vars'][] = array('key' => '', 'value' => '');
  $form_state['rebuild'] = 1;
}

function _openx_settings_add_variable_callback($form, &$form_state) {
  return $form['targeting']['openx_site_vars'];
}

/**
 * Theme the OpenX blocks list as a table
 */
function theme_openx_settings_zones($variables) {
  $form =& $variables['form'];
  $header = array(t('Zone ID'), t('Block name'), '');
  foreach (element_children($form) as $zone) {
    $rows[] = array(
      drupal_render($form[$zone]['id']),
      drupal_render($form[$zone]['name']),
      drupal_render($form[$zone]['delete'])
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

function theme_openx_site_vars($variables) {
  $form =& $variables['form'];
  $header = array(t('Variable'), t('Value'), '');
  $rows = array();
  
  foreach (element_children($form) as $key) {
    $rows[] = array(
      drupal_render($form[$key]['key']),
      drupal_render($form[$key]['value']),
      drupal_render($form[$key]['delete']),
    );
  }
  
  return theme('table', array('header' => $header, 'rows' => $rows));
}
