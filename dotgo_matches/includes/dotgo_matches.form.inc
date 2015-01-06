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

  if(isset($form['comment_settings'])){
    $form['comment_settings']['#title'] = 'Message settings';
    $form['comment_settings']['comment'][0]['#description'] = 'Messages are unavailable.';
    $form['comment_settings']['comment'][1]['#description'] = 'Users cannot send messages.';
    $form['comment_settings']['comment'][2]['#description'] = 'Users with the "Send match message" permission can send messages.';
  }

  // Show Pattern when Parent no Parent.
  $form['field_dotgo_match_pattern']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-parent-und' => array('value' => ''),
    ),
  );

  // Show message when type is default.
  $form['field_dotgo_match_message']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und-0-value' => array(
        array('value' => 'default'),
      ),
    ),
  );

  // Show Engine when Type is Engine.
  $form['field_dotgo_match_engine']['#states'] = array(
    'visible' => array(
      '#edit-field-dotgo-match-type-und-0-value' => array('value' => 'engine'),
    ),
  );
}

/**
 * Form callback for the apps configuration form.
 */
function dotgo_matches_apps_configure_form($form, &$form_state) {
  return variable_group_form($form, $form_state, 'dotgo_matches');
}