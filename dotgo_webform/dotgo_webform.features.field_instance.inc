<?php
/**
 * @file
 * dotgo_webform.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function dotgo_webform_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-dotgo_match-field_dotgo_match_webform'
  $field_instances['node-dotgo_match-field_dotgo_match_webform'] = array(
    'bundle' => 'dotgo_match',
    'comment_alter' => 0,
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'module' => 'dotgo_webform',
        'settings' => array(),
        'type' => 'dotgo_webform_input',
        'weight' => 15,
      ),
      'dotgo' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 3,
      ),
      'featured' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_dotgo_match_webform',
    'label' => 'Elements',
    'required' => 0,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'dotgo_webform',
      'settings' => array(),
      'type' => 'dotgo_webform_component',
      'weight' => 8,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('Elements');

  return $field_instances;
}
