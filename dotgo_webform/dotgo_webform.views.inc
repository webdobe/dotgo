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
function dotgo_webform_views_data_alter(&$data) {
  // sys_user.
  $data['webform_submissions']['sys_user'] = array(
    'title' => t('DOTGO User'),
    'help' => t('The dotgo user who sent the webform submission.'),
    'relationship' => array(
      'base' => 'dotgo_users',
      'field' => 'sys_user',
      'handler' => 'views_handler_relationship',
      'label' => t('Webform Submission Dotgo User'),
    ),
  );
}
