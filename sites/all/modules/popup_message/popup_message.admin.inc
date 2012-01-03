<?php

/**
 * @file
 * Settings.
 */

/**
 * Settings form.
 *
 * @return
 *   Form array.
 */
function popup_message_settings() {
  $form = array();
  $form['popup_message_enable'] = array(
    '#type' => 'radios',
    '#title' => t('Enable Popup'),
    '#default_value' => variable_get('popup_message_enable', 1),
    '#options' => array(
      1 => t('Enabled'),
      0 => t('Disabled'),
    ),
  );
  $form['popup_message_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Popup message settings'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $form['popup_message_fieldset']['popup_message_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Message title'),
    '#required' => TRUE,
    '#default_value' => variable_get('popup_message_title'),
  );
  $popup_message_body = variable_get('popup_message_body');
  $form['popup_message_fieldset']['popup_message_body'] = array(
      '#type' => 'text_format',
      '#base_type' => 'textarea',
      '#title' => t('Message body'),
      '#default_value' => $popup_message_body['value'],
      '#format' => isset($popup_message_body['format']) ? $popup_message_body['format'] : NULL,
    );
  $form['popup_message_fieldset']['popup_message_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Window width'),
    '#required' => TRUE,
    '#default_value' => variable_get('popup_message_width', 300),
  );
  $form['popup_message_fieldset']['popup_message_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Window height'),
    '#required' => TRUE,
    '#default_value' => variable_get('popup_message_height', 300),
  );
  $form['popup_message_fieldset']['popup_message_check_cookie'] = array(
    '#type' => 'radios',
    '#title' => t('Check cookie'),
    '#description' => t('If enabled message will be displayed only once per browser session'),
    '#default_value' => variable_get('popup_message_check_cookie', 1),
    '#options' => array(
      1 => t('Enabled'),
      0 => t('Disabled'),
    ),
  );

  // Styles.
  // Find styles in module directory.
  $directory = drupal_get_path('module', 'popup_message') . '/styles';
  $subdirectories = scandir($directory);
  $styles = array();
  foreach ($subdirectories as $subdir) {
    if (is_dir($directory . '/' . $subdir)) {
      if (file_exists($directory . '/' . $subdir . '/' . POPUP_MESSAGE_CSS_NAME)) {
        $path = $directory . '/' . $subdir . '/' . POPUP_MESSAGE_CSS_NAME;
        $styles[$path] = $path;
      }
    }
  }
  // Find styles in default theme. directory.
  $directory = drupal_get_path('theme', variable_get('theme_default', '')) . '/' . POPUP_MESSAGE_THEME_STYLE_DIR;
  if (file_exists($directory)) {
    $subdirectories = scandir($directory);
    foreach ($subdirectories as $subdir) {
      if (is_dir($directory . '/' . $subdir)) {
        if (file_exists($directory . '/' . $subdir . '/' . POPUP_MESSAGE_CSS_NAME)) {
          $path = $directory . '/' . $subdir . '/' . POPUP_MESSAGE_CSS_NAME;
          $styles[$path] = $path;
        }
      }
    }
  }
  $form['popup_message_fieldset']['popup_message_style'] = array(
    '#type' => 'select',
    '#title' => t('Popup style'),
    '#default_value' => variable_get('popup_message_style', drupal_get_path('module', 'popup_message') . '/styles/'. POPUP_MESSAGE_CSS_NAME ),
    '#options' => $styles,
    '#description' => t('To add custom styles create directory and file "[my_default_theme]/popup_message_styles/mystyle/popup.css" and set in this file custom CSS code.'),
  );

  return system_settings_form($form);
}
