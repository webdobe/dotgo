<?php
/**
 * @file
 * Views style plugins to render nodes in the XML data format.
 *
 * @see views_plugin_style_xml.inc
 * @ingroup views_plugins
 */

/**
 * Implements hook_views_plugins().
 */
function dotgo_matches_views_plugins() {
  $path = drupal_get_path('module', 'dotgo_matches');
  return array(
    'module' => 'dotgo_matches',
    'style' => array(
      'dotgo_cmrl_style' => array(
        'title' => t('DOTGO CMRL'),
        'path' => $path . '/plugins',
        'help' => t('Display a view as an CMRL document.'),
        'handler' => 'views_plugin_style_dotgo_cmrl',
        'theme' => 'views_dotgo_cmrl_style',
        'theme file' => 'views_dotgo_cmrl_style.theme.inc',
        'theme path' => $path . '/theme',
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help_topic' => 'style-cmrl',
        'even empty' => TRUE,
      ),
      'dotgo_engine_style' => array(
        'title' => t('DOTGO Engine'),
        'path' => $path . '/plugins',
        'help' => t('Display a view as a DOTGO Engine.'),
        'handler' => 'views_plugin_style_dotgo_engine',
        'theme' => 'views_dotgo_engine_style',
        'theme file' => 'views_dotgo_engine_style.theme.inc',
        'theme path' => $path . '/theme',
        'uses row plugin' => TRUE,
        'uses fields' => FALSE,
        'uses options' => TRUE,
        'type' => 'normal',
        'help_topic' => 'style-engine',
        'even empty' => TRUE,
      ),
    ),
  );
}
