<?php

/**
 * Define Dotgo types.
 */
function dotgo_info_types($type = NULL) {
  $info = _dotgo_info_collate_types();
  $types = $info['types'];
  if ($type) {
    if (isset($types[$type])) {
      return $types[$type];
    }
  }
  else {
    return $types;
  }
}

/**
 *
 */
function _dotgo_info_collate_types($reset = NULL){
  global $language;
  static $info;

  // The _types_info() hooks invoked below include translated strings, so each
  // language is cached separately.
  $langcode = $language->language;

  if ($reset) {
    $info = NULL;
    // Clear all languages.
    cache_clear_all('dotgo_info_types:', 'cache_dotgo', TRUE);
    return;
  }

  if (!isset($info)) {
    if ($cached = cache_get("dotgo_info_types:$langcode", 'cache_dotgo')) {
      $info = $cached->data;
    }
    else {
      $info = array(
        'types' => array(),
      );

      // Populate types.
      foreach (module_implements('dotgo_type_info') as $module) {
        $types = (array) module_invoke($module, 'dotgo_type_info');
        foreach ($types as $name => $type_info) {
          // Provide defaults.
          $type_info += array(
            'settings' => array(),
          );
          $info['types'][$name] = $type_info;
          $info['types'][$name]['module'] = $module;
        }
      }
      drupal_alter('dotgo_type_info', $info['types']);

      // Set the cache if we can acquire a lock.
      if (lock_acquire("dotgo_info_types:$langcode")) {
        cache_set("dotgo_info_types:$langcode", $info, 'cache_dotgo');
        lock_release("dotgo_info_types:$langcode");
      }
    }
  }

  return $info;
}

/**
 * Implements template_preprocess_dotgo_default(&$vars);
 */
function template_preprocess_dotgo_default(&$vars){
  $node = isset($vars['node']) ? $vars['node'] : NULL;
  if($node){
    $node_view = node_view($node, $view_mode = 'dotgo', $node->language);
    $message = drupal_render($node_view);
    $message = _dotgo_filter_process($message);
    $vars['message'] = _dotgo_message($message);
  } else {
    $vars['message'] = _dotgo_message('No matching node.');
  }
}