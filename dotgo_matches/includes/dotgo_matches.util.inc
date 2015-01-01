<?php
/**
 * @file
 * Code for DOTGO Matches util helpers
 */

/**
 * Get child Matches.
 *
 * @param int $nid
 *   The NID of the Space or Group to check.
 * @param string|NULL $bundle
 *   (optional) The node type (default: DOTGO_MATCH_TYPE). If NULL is passed, it
 *   will include all node types.
 * @param int|NULL $status
 *   (optional) If specified, the node status (ex. NODE_PUBLISHED or
 *   NODE_NOT_PUBLISHED) to look for. If not specified, it return
 *   nodes of either status.
 * @param bool $include_archived
 *   (optional) Whether to include archived nodes or not. By default, archived
 *   items aren't included.
 *
 * @return array
 *   Array of children NIDs.
 */
function dotgo_matches_get_by_parent($nid, $bundle = DOTGO_MATCH_TYPE, $status = NULL, $include_archived = FALSE, $account = NULL) {

  if (!isset($account)) {
    global $user;
    $uid = $user->uid;
  }
  elseif (is_numeric($account)) {
    $uid = $account;
  }
  else {
    $uid = $account->uid;
  }

  $query = db_select('node', 'n');
  $query->leftJoin('field_data_field_dotgo_match_parent', 'p', "n.nid = p.entity_id");
  $query
    ->fields('n', array('nid'))
    ->condition('p.field_dotgo_match_parent_target_id', $nid)
    ->condition('p.entity_type', 'node')
    ->orderBy('n.title')
    ->addTag('node_access');

  if (isset($bundle)) {
    $query->condition('n.type', $bundle);
  }
  if (isset($status)) {
    $query->condition('n.status', $status);
  }

  if (module_exists('flag') && !$include_archived) {
    if ($flag = flag_get_flag('trash')) {
      $query->leftJoin('flag_content', 'a', "a.fid = :fid AND a.content_id = n.nid", array(':fid' => $flag->fid));
      // This makes sure that archived content isn't included, because 'uid'
      // will be NULL if the join didn't connect anything.
      $query->isNull('a.uid');
    }
  }

  $result = $query->execute()->fetchCol(0);

  return $result;
}

/**
 * _dotgo_engine_render_rows($rows).
 */
function _dotgo_cmrl_render_rows($view, $options){
  if(empty($view->result)){
    return array();
  }
  $tree_array = _dotgo_cmrl_tree_array($view, $options);

  return $tree_array;
}

/**
 * Helper function to build cmrl tree.
 */
function _dotgo_cmrl_tree_array($view, $options){
  if(empty($view->result)){
    return array();
  }
  $items = array();
  $return = array();

  $parent = explode(':',$options['parent']);
  $weight = explode(':',$options['weight']);

  $parent_field = 'field_'.$parent[1];
  $weight_field = 'field_'.$weight[1];

  foreach($view->result as $key => $row) {
    $fields = array_keys($row->_field_data);
    $nid_field = $fields[0];
    $nid = $row->{$nid_field};
    $parent_value = isset($row->{$parent_field}[0]['raw']['target_id']) ? $row->{$parent_field}[0]['raw']['target_id']: NULL;
    $weight_value = isset($row->{$weight_field}[0]['raw']['value']) ? $row->{$weight_field}[0]['raw']['value'] : 0;
    $items[$nid]['parent'] = $parent_value;
    $items[$nid]['weight'] = $weight_value;
    if (isset($row->_field_data['nid']['entity'])) {
      $items[$nid]['node'] = $row->_field_data['nid']['entity'];
    }
    if(empty($parent_value)) {
      $items['trees'][] = $nid;
    } else {
      $items[$parent_value]['children'][] = $nid;
    }
  }

  $trees = $items['trees'];
  unset($items['trees']);
  $count = 0;

  foreach($trees as $key => $tree) {
    $return[$tree]['tree'] = $tree;
    $return[$tree]['node'] = $items[$tree]['node'];
    $return[$tree]['parent'] = $items[$tree]['parent'];
    $return[$tree]['weight'] = $count;
    if(isset($items[$tree]['children'])) {
      $return[$tree]['children'] = $items[$tree]['children'];
      $children = _dotgo_cmrl_tree_children($items[$tree]['children'], $items, $tree);

      if($children) {
        foreach ($children as $id => $row) {
          $return[$id] = $row;
        }
      }
    } else {
      $return[$tree]['children'] = NULL;
    }
    $count++;
  }

  return $return;
}

/**
 * Helper function to build tree children
 */
function _dotgo_cmrl_tree_children($nids, $list, $tree) {
  $return = array();
  $count = 0;

  foreach($nids as $weight => $nid) {
    $return[$nid]['tree'] = $tree;
    $return[$nid]['parent'] = $list[$nid]['parent'];
    $return[$nid]['node'] = $list[$nid]['node'];
    $return[$nid]['weight'] = $count;
    if(isset($list[$nid]['children'])) {
      $return[$nid]['children'] = $list[$nid]['children'];
      $children = _dotgo_cmrl_tree_children($list[$nid]['children'], $list, $tree);

      if($children) {
        foreach ($children as $key => $row) {
          $return[$key] = $row;
        }
      }
    } else {
      $return[$nid]['children'] = NULL;
    }
    $count++;
  }
  return $return;
}
