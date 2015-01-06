<?php

/**
 * Impliments dotgo_menu_build.
 *
 * @param array $menus
 * @param null $type
 * @param array $items
 * @param null $account
 * @return string
 */
function dotgo_menu_build($menus = array(), $current_path = NULL, $account = NULL) {
  $items = _dotgo_menu_collate_items();

  $return = array();
  foreach($items as $name => $item){
    $function = isset($item['access callback']) ? $item['access callback'] : 'dotgo_menu_access';
    if (!function_exists($function)) {
      continue;
    } else {
      if($function($item, $account)){
        if(empty($menus) || in_array($item['menu_name'], $menus)){
          if($current_path != $item['query']){
            $return[$name] = $item;
          }
        }
      }
    }
  }

  // Let others modules alter the returned menu.
  drupal_alter('dotgo_menu', $return, $current_path, $account);

  return theme('dotgo_menu', array('items' => $return, 'account' => $account));

}

/**
 *
 */
function _dotgo_menu_collate_items($reset = NULL){
  global $language;
  static $info;

  // The _types_info() hooks invoked below include translated strings, so each
  // language is cached separately.
  $langcode = $language->language;

  if ($reset) {
    $info = NULL;
    // Clear all languages.
    cache_clear_all('dotgo_menu_items:', 'cache_dotgo', TRUE);
    return;
  }

  if (!isset($info)) {
    if ($cached = cache_get("dotgo_menu_items:$langcode", 'cache_dotgo')) {
      $info = $cached->data;
    }
    else {
      $info = array(
        'types' => array(),
      );

      // Populate types.
      foreach (module_implements('dotgo_menu') as $module) {
        $types = (array) module_invoke($module, 'dotgo_menu');
        foreach ($types as $name => $type_info) {
          // Provide defaults.
          $type_info += array(
            'settings' => array(),
          );
          $info['types'][$name] = $type_info;
          $info['types'][$name]['module'] = $module;
        }
      }
      drupal_alter('dotgo_menu', $info['types']);

      // Set the cache if we can acquire a lock.
      if (lock_acquire("dotgo_menu_items:$langcode")) {
        cache_set("dotgo_menu_items:$langcode", $info, 'cache_dotgo');
        lock_release("dotgo_menu_items:$langcode");
      }
    }
  }

  return $info;
}

function dotgo_menu_access($item, $account = NULL){
  if(empty($item['access arguments'])){
    return TRUE;
  } else {
    foreach($item['access arguments'] as $permission){
      if(user_access($permission, $account)){
        continue;
      } else {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Implements template_preprocess_dotgo_default(&$vars);
 */
function template_preprocess_dotgo_menu(&$vars){
  ddl($vars);
  /*
  $menus += module_invoke_all('dotgo_menu', $account);
  if(!empty($menus)) {
    foreach ($menus as $menu) {
      if ($type == NULL || $menu['type'] == NULL || $menu['type'] == $type) {
        if ($items) {
          foreach ($items as $item) {
            if ($menu['name'] == NULL || $menu['name'] != $item) {
              if (!$menu['perm'] || user_access($menu['perm'], $account, FALSE)) {
                $query .= _dotgo_make_anchor_query($menu['query']) . $menu['text'] . '<br />';
              }
            }
          }
        }
        else {
          if (!$menu['perm'] || user_access($menu['perm'], $account, FALSE)) {
            $query .= _dotgo_make_anchor_query($menu['query']) . $menu['text'] . '<br />';
          }
        }
      }
    }
    if ($query) {
      $msg .= '<br />REPLY MENU #<br />';
      $msg .= $query;
    }
  }*/
}