<?php

/**
 * @file views-dotgo-engine-style.tpl.php
 * views_plugin_style_dotgo_engine->theme_functions()
 *
 * - $view: The View object.
 * - $rows: Array of row objects rendered
 *   $options: Array of plugin style options
 *
 * @ingroup views_templates
 * @see views_plugin_style_dotgo_engine.inc
 * @see views_dotgo_engine_style.theme.inc
 */
if (isset($view->override_path)) { // inside live preview
  print htmlspecialchars($xml);
}
elseif ($options['using_views_api_mode']) { // We're in Views API mode.
  print $xml;
}
else {
  drupal_add_http_header("Content-Type", "$content_type; charset=utf-8");
  print $xml;
  drupal_page_footer();
  exit;
}