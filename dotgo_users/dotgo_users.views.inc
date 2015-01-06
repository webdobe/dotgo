<?php
/**
 * @file
 * Views style plugins to render nodes in the XML data format.
 *
 * @see views_plugin_style_xml.inc
 * @ingroup views_plugins
 */

/**
 * Implements hook_views_data_alter().
 */
function dotgo_usersviews_data_alter(&$data) {
  // Dotgo User of user.
  $data['users']['dotgo_users'] = array(
    'title' => t('DOTGO User'),
    'help' => t('DOTGO User of the given user.'),
    'real field' => 'uid',
    'relationship' => array(
      'base' => 'dotgo_users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('DOTGO User'),
    ),
  );
}
