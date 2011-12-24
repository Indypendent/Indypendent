<?php

/**
 * @file
 * Support for profile2 destinations.
 */

// TODO:
// Make sure this works with updates, explicit destination keys

/**
 * Destination class implementing migration into nodes.
 */
class MigrateDestinationProfile2 extends MigrateDestinationEntity {

  var $entity_type = 'profile2';
  var $entity_info = NULL;
  var $entity_key = NULL;

  static public function getKeySchema() {
    $key = 'pid'; // Hard coded since $this->entity_key not in object context.
    return array(
      $key => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Return an options array for profile2 destinations.
   *
   * @param string $language
   *  Default language for profiles created via this destination class.
   * @param string $text_format
   *  Default text format for profiles created via this destination class.
   */
  static public function options($language, $text_format) {
    return compact('language', 'text_format');
  }

  /**
   * Basic initialization
   *
   * @param string $bundle
   *  A.k.a. the profile type.
   * @param array $options
   *  Options applied to profiles.
   */
  public function __construct($bundle, array $options = array()) {
    parent::__construct($this->entity_type, $bundle, $options);
    $this->entity_info = entity_get_info('profile2');
    $this->entity_key = $this->entity_info['entity keys']['id'];
  }

  /**
   * Returns a list of fields available to be mapped for the node type (bundle)
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields = array();
    $type = ucfirst($this->entity_type) . ': ';
    // First the core (node table) properties
    $fields[$this->entity_key] = $type . t('Existing profile ID');
    $fields['uid'] = $type . t('Authored by (uid)');
    $fields['revision_uid'] = $type . t('Modified (uid)');
    //$fields['created'] = $type . t('Created timestamp');
    //$fields['changed'] = $type . t('Modified timestamp');
    //$fields['status'] = $type . t('Published');
    //$fields['revision'] = $type . t('Create new revision');
    $fields['language'] = $type . t('Language (fr, en, ...)');

    // Then add in anything provided by handlers
    $fields += migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle);
    $fields += migrate_handler_invoke_all('Profile2', 'fields', $this->entityType, $this->bundle);

    return $fields;
  }

  /**
   * Delete a batch of profiles at once.
   *
   * @param $ids
   *  Array of profile IDs to be deleted.
   */
  public function bulkRollback(array $ids) {
    migrate_instrument_start('profile2_delete_multiple');
    $this->prepareRollback($ids);
    entity_delete_multiple($this->entity_type, $ids);
    $this->completeRollback($ids);
    migrate_instrument_stop('profile2_delete_multiple');
  }

  /**
   * Import a single entity.
   *
   * @param $entity
   *  Entity object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields of the entity that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $entity, stdClass $row) {
    $migration = Migration::currentMigration();
    $type = $this->entity_info['entity keys']['bundle'];
    $entity->$type = $this->bundle;
    list($id, $vid, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // Updating previously-migrated content?
    if (isset($row->migrate_map_destid1)) {
      // Make sure is_new is off
      $entity->is_new = FALSE;
      if (!empty($id)) {
        if ($id != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming id !id and map destination id !destid1 don't match",
            array('!id' => $id, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $entity->$this->entity_key = $row->migrate_map_destid1;
      }
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (empty($id)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination id provided'));
      }
      $old_entity = entity_load_single($this->entitytype, $id);
      if (!isset($entity->created)) {
        $entity->created = $old_entity->created;
      }
      if (empty($vid)) {
        $entity->vid = $old_entity->vid;
      }
      if (!isset($entity->status)) {
        $entity->status = $old_entity->status;
      }
      if (!isset($entity->uid)) {
        $entity->uid = $old_entity->uid;
      }
    }

    if ($migration->getSystemOfRecord() == Migration::SOURCE) {
      if (!isset($entity->language)) {
        $entity->language = $this->language;
      }

      // Apply defaults, allow standard prepare hooks to fire.
      // node_object_prepare() will blow these away, so save them here and
      // stuff them in later if need be.
      //// TODO: stub.
    }

    // Invoke migration prepare handlers
    $this->prepare($entity, $row);
    if (!isset($entity->revision)) {
      $entity->revision = 0; // Saves disk space and writes. Can be overridden.
    }

    // Trying to update an existing entity
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      // Incoming data overrides existing data, so only copy non-existent fields
      foreach ($old_entity as $field => $value) {
        // An explicit NULL in the source data means to wipe to old value (i.e.,
        // don't copy it over from $old_entity)
        if (property_exists($entity, $field) && $entity->$field === NULL) {
          // Ignore this field
        }
        elseif (!isset($entity->$field)) {
          $entity->$field = $old_entity->$field;
        }
      }
    }

    if (empty($id) && !(isset($entity->is_new) && $entity->is_new)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    migrate_instrument_start('entity_save');
    entity_save($this->entity_type, $entity);
    migrate_instrument_stop('entity_save');

    list($id, $vid, $bundle) = entity_extract_ids($this->entity_type, $entity);

    if (isset($id)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }

      $return = array($id);
    }
    else {
      $return = FALSE;
    }

    $this->complete($entity, $row);
    return $return;
  }
}
