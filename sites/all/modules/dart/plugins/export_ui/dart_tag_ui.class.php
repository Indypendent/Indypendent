<?php

/**
 * @file
 * A custom Ctools Export UI class for DART Tags.
 */

/**
 * Customizations of the DART Tags UI.
 */
class dart_tag_ui extends ctools_export_ui {

  /**
   * Prepare the item object before the edit form is rendered.
   */
  function edit_form(&$form, &$form_state) {
    $form_state['item']->settings = unserialize($form_state['item']->settings);

    parent::edit_form($form, $form_state);
  }

  /**
   * Prepare the tag values before they are added to the database.
   */
  function edit_form_submit(&$form, &$form_state) {
    $settings = $form_state['item']->settings;
    $structure = _dart_tag_settings_data_structure();

    foreach ($structure as $group => $fields) {
      // Values will be stored at the root of settings['group'].
      if (empty($fields)) {
        $settings[$group] = $form_state['values'][$group];
      }
      else {
        foreach ($fields as $field) {
          $settings[$group][$field] = $form_state['values'][$field];
        }
      }
    }

    $form_state['values']['settings'] = serialize($settings);
    parent::edit_form_submit($form, $form_state);
  }

  /**
   * Build a row based on the item.
   *
   * By default all of the rows are placed into a table by the render
   * method, so this is building up a row suitable for theme('table').
   * This doesn't have to be true if you override both.
   */
  function list_build_row($item, &$form_state, $operations) {
    // Set up sorting
    $name = $item->{$this->plugin['export']['key']};

    // Note: $item->type should have already been set up by export.inc so
    // we can use it safely.
    switch ($form_state['values']['order']) {
      case 'disabled':
        $this->sorts[$name] = empty($item->disabled) . $name;
        break;
      case 'title':
        $this->sorts[$name] = $item->{$this->plugin['export']['admin_title']};
        break;
      case 'name':
        $this->sorts[$name] = $name;
        break;
      case 'storage':
        $this->sorts[$name] = $item->type . $name;
        break;
    }

    $this->rows[$name]['data'] = array();
    $this->rows[$name]['class'] = !empty($item->disabled) ? array('ctools-export-ui-disabled') : array('ctools-export-ui-enabled');

    // If we have an admin title, make it the first row.
    if (!empty($this->plugin['export']['admin_title'])) {
      $this->rows[$name]['data'][] = array('data' => check_plain($item->{$this->plugin['export']['admin_title']}), 'class' => array('ctools-export-ui-title'));
    }
    $this->rows[$name]['data'][] = array('data' => check_plain($name), 'class' => array('ctools-export-ui-name'));
    $this->rows[$name]['data'][] = array('data' => $item->active ? t('Yes') : t('No'), 'class' => array('ctools-export-ui-active'));
    $this->rows[$name]['data'][] = array('data' => check_plain($item->pos), 'class' => array('ctools-export-ui-pos'));
    $this->rows[$name]['data'][] = array('data' => check_plain($item->sz), 'class' => array('ctools-export-ui-sz'));
    $this->rows[$name]['data'][] = array('data' => check_plain($item->type), 'class' => array('ctools-export-ui-storage'));
    $this->rows[$name]['data'][] = array('data' => theme('links', array('links' => $operations)), 'class' => array('ctools-export-ui-operations'));

    // Add an automatic mouseover of the description if one exists.
    if (!empty($this->plugin['export']['admin_description'])) {
      $this->rows[$name]['title'] = $item->{$this->plugin['export']['admin_description']};
    }
  }

  /**
   * Provide the table header.
   *
   * If you've added columns via list_build_row() but are still using a
   * table, override this method to set up the table header.
   */
  function list_table_header() {
    $header = array();
    if (!empty($this->plugin['export']['admin_title'])) {
      $header[] = array('data' => t('Title'), 'class' => array('ctools-export-ui-title'));
    }

    $header[] = array('data' => t('Name'), 'class' => array('ctools-export-ui-name'));
    $header[] = array('data' => t('Active'), 'class' => array('ctools-export-ui-active'));
    $header[] = array('data' => t('Pos'), 'class' => array('ctools-export-ui-pos'));
    $header[] = array('data' => t('Size'), 'class' => array('ctools-export-ui-sz'));
    $header[] = array('data' => t('Storage'), 'class' => array('ctools-export-ui-storage'));
    $header[] = array('data' => t('Operations'), 'class' => array('ctools-export-ui-operations'));

    return $header;
  }

}