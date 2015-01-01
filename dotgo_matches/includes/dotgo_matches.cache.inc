<?php
/**
 * @file
 * Code for DOTGO Matches caching helpers
 */

/**
 * Name of various caches.
 */
define('DOTGO_CACHE_MATCH_STATE', 'dotgo_match_state');
define('DOTGO_CACHE_MATCHES_BY_GROUP', 'dotgo_matches_by_group');

/**
 * Helper function to get a value from static or db cache
 * @param $cid : the cache id (see dotgo_matches_get_cache_id)
 * @param $value : the value returned
 * @return bool : TRUE if cache value is returned
 */
function dotgo_matches_get_cache($name, $cid, &$value) {
  $static_cache = &drupal_static($name);
  if (isset($static_cache[$cid])) {
    $value = $static_cache[$cid];
    return TRUE;
  }

  if ($cache = cache_get($cid)) {
    $static_cache[$cid] = $cache->data;
    $value = $cache->data;
    return TRUE;
  }
  return FALSE;
}

/**
 * Helper function to set a value to the static and db cache
 * @param $cid: the cache id (see dotgo_matches_get_cache_id)
 * @param $value : the value to save in cache
 */
function dotgo_matches_set_cache($name, $cid, $value) {
  $static_cache = &drupal_static($name);
  $static_cache[$cid] = $value;
  cache_set($cid, $value, 'cache_matches');
}

/**
 * Helper function to clear the static and db cache
 * @param $cid: the cache id (see dotgo_matches_get_cache_id)
 * @param $value : the value to save in cache
 */
function dotgo_matches_clear_cache($name, $cid) {
  cache_clear_all($cid, 'cache_matches', TRUE);
  drupal_static_reset($name);
}

/**
 * Helper function to return a caching ids.
 * @param $cache_name : The name of the cache table.
 * @param null $id : if not-null, adds the id to the cache id.  Typically passes the $user->uid
 * @param bool $include_archived : if TRUE, adds _archived to the cache id
 * @param array $args : appends each argument to the cache id
 * @return string : the cache_id for cache_get/set
 */
function dotgo_matches_get_cache_id($cache_name, $id = NULL, $include_archived = FALSE, $args = array()) {

  $cid = $cache_name . '_';

  if (!empty($id)) {
    $cid .= $id;

    if ($include_archived) {
      $cid .= '_archived';
    }

    foreach ($args as $arg) {
      $cid .= '_' . (!empty($arg) ? $arg : '');
    }
  }

  return $cid;
}
