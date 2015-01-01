<?php
/**
 * @file
 * dotgo_matches.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function dotgo_matches_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function dotgo_matches_node_info() {
  $items = array(
    'dotgo_match' => array(
      'name' => t('Match'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}
