<?php
/**
 * @file
 * dotgo_webform.features.defaultconfig.inc
 */

/**
 * Implements hook_defaultconfig_features().
 */
function dotgo_webform_defaultconfig_features() {
  return array(
    'dotgo_webform' => array(
      'strongarm' => 'strongarm',
    ),
  );
}

/**
 * Implements hook_defaultconfig_strongarm().
 */
function dotgo_webform_defaultconfig_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'webform_node_dotgo_match';
  $strongarm->value = 1;
  $export['webform_node_dotgo_match'] = $strongarm;

  return $export;
}
