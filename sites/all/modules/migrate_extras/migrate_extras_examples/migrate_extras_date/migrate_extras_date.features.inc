<?php

/**
 * Implementation of hook_node_info().
 */
function migrate_extras_date_node_info() {
  $items = array(
    'migrate_example_date' => array(
      'name' => t('Migrate example - dates'),
      'base' => 'node_content',
      'description' => t('This content type is used for demonstrating and testing Migrate Example support for the Date module.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
