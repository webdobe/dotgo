<?php
/**
 * @file
 * dotgo_webform.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function dotgo_webform_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_dotgo_match_webform'
  $field_bases['field_dotgo_match_webform'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_dotgo_match_webform',
    'foreign keys' => array(),
    'indexes' => array(
      'cid' => array(
        0 => 'cid',
      ),
    ),
    'locked' => 0,
    'module' => 'dotgo_webform',
    'settings' => array(),
    'translatable' => 0,
    'type' => 'dotgo_webform',
  );

  return $field_bases;
}
