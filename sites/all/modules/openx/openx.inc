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
 * Insert the JavaScript into the page's header.
 */
function _openx_javascript() {
  global $user;
  static $spc_code;

  // No need to add the header more than once
  if (!empty($spc_code)) {
    return;
  }

  if (!empty($_SERVER['HTTPS'])) {
    $protocol = 'https';
    $server = variable_get('openx_delivery_url_https', 'd.openx.org');
  }
  else {
    $protocol = 'http';
    $server = variable_get('openx_delivery_url', 'd.openx.org');
  }
  $url = $protocol . '://' . trim($server, '/') . '/spcjs.php';
  
  if ($site_vars = variable_get('openx_site_vars', FALSE)) {
    $vars = array();
    if (module_exists('token')) {
      $data = array('global' => NULL, 'user' => $user);
      
      $node = menu_get_object('node');
      
      if (isset($node)) {
        $objects['node'] = $node;
      }
    }
    foreach ($site_vars as $var) {
      if (!empty($var['key'])) {
        if (module_exists('token')) {
          $vars[$var['key']] = token_replace($var['value'], $data);
        }
        else {
          $vars[$var['key']] = $var['value'];
        }
      }
    }
    
    if (!empty($vars)) {
      $url .= '?' . drupal_http_build_query($vars);
    }
  }

  $zones = variable_get('openx_zones', array());
  $script = "  var OA_zones = {\n";

  $js_zones = array();
  foreach ($zones as $idx => $zone) {
    if (!empty($zone['id'])) {
      $js_zones[] = "    '" . $zone['name'] . "' : " . $zone['id'];
    }
  }
  $script .= implode(",\n", $js_zones);
  $script .= "\n  }";

  $spc_code = array();
  drupal_add_html_head(array(
    '#type' => 'html_tag',
    '#tag' => 'script',
    '#attributes' => array(
      'type' => 'text/javascript',
    ),
    '#value' => $script,
    '#weight' => 1000,
  ), 'openx_zones');
  drupal_add_html_head(array(
    '#type' => 'html_tag',
    '#tag' => 'script',
    '#attributes' => array(
      'type' => 'text/javascript',
      'src' => $url,
    ),
    '#value' => '',
    '#weight' => 1001,
  ), 'openx_script');
}

/**
 * Return zone with given index
 */
function _openx_get_zone($index_or_key) {
  $zones = variable_get('openx_zones', array());
  if (isset($zones[$index_or_key])) {
    return $zones[$index_or_key];
  }
  else {
    foreach ($zones as $zone) {
      if ($zone['name'] == $index_or_key) {
        return $zone;
      }
    }
  }

  // There is no zone with such ID or name
  return FALSE;
}
