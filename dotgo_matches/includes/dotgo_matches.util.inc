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
 * Helper function to get single field values without having to do a node_load.
 */
function dotgo_matches_get_field($node, $field_name, $field_value) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', $node->type)
    ->propertyCondition('status', 1)
    ->fieldCondition($field_name, $field_value, 'NULL', '!=');
  $results = $query->execute();
  if (isset($results['node'])) {
    $entities = $results['node'];
    $fields = field_info_instances('node', $node->type);
    $field_id = $fields[$field_name]['field_id'];
    field_attach_load('node', $entities, FIELD_LOAD_CURRENT, array('field_id' => $field_id));
    if (isset($entities[$node->nid])) {
      $field = field_get_items('node', $entities[$node->nid], $field_name);
      return $field;
    }
  }
  return NULL;
}

/**
 * Helper function to load match.
 *
 * @param $node
 * @return nid
 */
function dotgo_matches_load_match($entity) {
  $match = dotgo_matches_get_match($entity);
  return node_load($match);
}

/**
 * Helper function to grab the top level match.
 *
 * @param $node
 * @return nid
 */
function dotgo_matches_get_match($node) {
  $field_name = 'field_dotgo_match_parent';
  $field_value = 'target_id';
  $parent = dotgo_matches_get_field($node, $field_name, $field_value);
  if (!$parent) {
    return $node->nid;
  }
  else {
    // create a blank node
    $node = new stdClass();
    $node->type = 'dotgo_match';
    $node->language = 'und';
    $node->nid = $parent[0][$field_value];
    return dotgo_matches_get_match($node);
  }
}

/**
 * Helper to get match form components.
 *
 * @param $entity
 * @return $components
 */
function dotgo_matches_get_match_form_elements($entity){
  $elements = array();

  if(!isset($entity->match)){
    $entity->match = dotgo_matches_load_match($entity);
  }

  drupal_alter('get_match_form_elements', $elements, $entity);

  return $elements;
}

function dotgo_matches_get_match_form_element($entity){
  $element = array();
  $elements = dotgo_matches_get_match_form_elements($entity);

  drupal_alter('get_match_form_element', $element, $elements, $entity);

  return $element;
}

function dotgo_matches_get_form_types(){
  return array(
    "date" => "Date",
    "email" => "Email",
    "textfield" => "Text",
    "number" => "Number",
    "select" => "Select",
    "time" => "Time",
  );
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

function dotgo_matches_determin_next_match($node, $params){
  $children = dotgo_matches_get_by_parent($node->nid);

  if(empty($children)){
    if(empty($node->field_dotgo_match_parent)){
      return;
    } else {
      $parent = $node->field_dotgo_match_parent[$node->language][0]['target_id'];
      $children = dotgo_matches_get_by_parent($parent);
    }
  }

  foreach($children as $key => $child){
    if($child == $node->nid){
      unset($children[$key]);
      continue;
    }
  }

  if(empty($children)){
    return;
  }

  array_values($children);
  return node_load($children[0]);
}

/**
 * Given an array of submitted values, flatten it into data for a submission.
 *
 * @param $node
 *   The node object containing the current webform.
 * @param $submitted
 *   The submitted user values from the webform.
 * @return
 *   An array suitable for use in the 'data' property of a $submission object.
 */
function dotgo_matches_form_submission_data($node, $submitted) {
  $data = array();

  foreach ($submitted as $cid => $values) {
    // Don't save pseudo-fields or pagebreaks as submitted data.
    if (!isset($node->webform['components'][$cid]) || $node->webform['components'][$cid]['type'] == 'pagebreak') {
      continue;
    }

    if (is_array($values)) {
      $data[$cid] = $values;
    }
    else {
      $data[$cid][0] = $values;
    }
  }

  return $data;
}

/**
 * Given set of $form_state values, prepare a psuedo-submission.
 */
function dotgo_matches_form_submission_create($node, $account, $match_state) {
  $submission = (object) array(
    'nid' => $node->nid,
    'uid' => $account->uid,
    'sys_user' => $account->sys_user,
    'sid' => NULL,
    'submitted' => REQUEST_TIME,
    'is_draft' => TRUE,
    'serial' => NULL,
    'data' => dotgo_matches_form_submission_data($node, $match_state['values']['submitted']),
  );
  drupal_alter('dotgo_matches_form_submission_create', $submission, $node, $account, $match_state);
  return $submission;
}

/**
 * Update a dotgo_matches_form submission entry in the database.
 *
 * @param $node
 *   The node object containing the current dotgo_matches_form.
 * @param $submission
 *   The dotgo_matches_form submission object to be saved into the database.
 * @return
 *   The existing submission SID.
 */
function dotgo_matches_form_submission_update($node, $submission) {
  // Allow other modules to modify the submission before saving.
  foreach (module_implements('dotgo_matches_form_submission_presave') as $module) {
    $function = $module . '_dotgo_matches_form_submission_presave';
    $function($node, $submission);
  }

  // Then re-add submission data to the database.
  $submission->is_new = FALSE;
  dotgo_matches_form_submission_insert($node, $submission);

  module_invoke_all('dotgo_matches_form_submission_update', $node, $submission);

  return $submission->sid;
}

/**
 * Insert a dotgo_matches_form submission entry in the database.
 *
 * @param $node
 *   The node object containing the current dotgo_matches_form.
 * @param $submission
 *   The dotgo_matches_form submission object to be saved into the database.
 * @return
 *   The new submission SID.
 */
function dotgo_matches_form_submission_insert($node, $submission) {
  // The submission ID may already be set if being called as an update.
  if (!isset($submission->sid) && (!isset($submission->is_new) || $submission->is_new == FALSE)) {
    // Allow other modules to modify the submission before saving.
    foreach (module_implements('dotgo_matches_form_submission_presave') as $module) {
      $function = $module . '_dotgo_matches_form_submission_presave';
      $function($node, $submission);
    }

    $is_new = TRUE;
  }

  // Invoke the insert hook after saving all the data.
  if (isset($is_new)) {
    module_invoke_all('dotgo_matches_form_submission_insert', $node, $submission);
  }

  return $submission->sid;
}

/**
 * Delete a single submission.
 *
 * @param $node
 *   The node object containing the current dotgo_matches_form.
 * @param $submission
 *   The dotgo_matches_form submission object to be deleted from the database.
 */
function dotgo_matches_form_submission_delete($node, $submission) {


  module_invoke_all('dotgo_matches_form_submission_delete', $node, $submission);
}
