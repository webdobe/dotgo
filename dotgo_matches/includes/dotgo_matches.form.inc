<?php
/**
 * @file
 * Code for DOTGO Matches form helpers
 */

/**
 * Implements hook_form_FORM_ID_alter().
 */
function dotgo_matches_form_dotgo_match_node_form_alter(&$form, &$form_state, $form_id) {

  $langcode = $form['language']['#value'];

  if (empty($form['nid']['#value'])) {
    // Set the default values for 'Priority', 'Status' and 'Type'.
    foreach (array('type', 'set_type') as $name) {
      $field_name = 'field_dotgo_match_' . $name;
      $variable_name = 'dotgo_matches_allowed_values_' . $name;
      $value = variable_get_value($variable_name);
      $form[$field_name][$langcode]['#default_value'] = $value['default_value'];
    }
  }

  if(isset($form['comment_settings'])){
    $form['comment_settings']['#title'] = 'Message settings';
    $form['comment_settings']['comment'][0]['#description'] = 'Messages are unavailable.';
    $form['comment_settings']['comment'][1]['#description'] = 'Users cannot send messages.';
    $form['comment_settings']['comment'][2]['#description'] = 'Users with the "Send match message" permission can send messages.';
  }

  // Show DOTGO Form Fields when Type is Form.
  $form['field_dotgo_match_webform']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und' => array('value' => 'form'),
    ),
  );


  // Show Pattern when Parent no Parent.
  $form['field_dotgo_match_pattern']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-parent-und' => array('value' => ''),
    ),
  );

  // Show message when type is default.
  $form['field_dotgo_match_message']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und' => array(
        array('value' => 'default'),
        array('value' => 'form'),
      ),
    ),
  );

  // Show Engine when Type is Engine.
  $form['field_dotgo_match_engine']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und' => array('value' => 'engine'),
    ),
  );

  // Show Set Name when Type is form.
  $form['field_dotgo_match_set_name']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und' => array('value' => 'form'),
    ),
  );

  // Show Set type when Type is form.
  $form['field_dotgo_match_set_type']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und' => array('value' => 'form'),
    ),
  );
}

/**
 * Form callback for the apps configuration form.
 */
function dotgo_matches_apps_configure_form($form, &$form_state) {
  return variable_group_form($form, $form_state, 'dotgo_matches');
}