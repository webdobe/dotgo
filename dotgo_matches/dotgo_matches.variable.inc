<?php

/**
 * @file
 * Hook implementations for the Variable module.
 */

/**
 * Implements hook_variable_info().
 */
function dotgo_matches_variable_info($options) {
  $description  = t('The possible values this field can contain.');

  $variable = array();
  $variable['dotgo_matches_allowed_values_type'] = array(
    'type' => 'dotgo_matches_allowed_values',
    'title' => t('Type values'),
    'default' => array(
      'options' => array(
        'default' => 'Default',
        'engine' => 'Engine',
        'form' => 'Form',
      ),
      'default_value' => 'default',
    ),
    'description' => $description,
    'group' => 'dotgo_matches',
  );
  $variable['dotgo_matches_allowed_values_set_type'] = array(
    'type' => 'dotgo_matches_allowed_values',
    'title' => t('Set Type values'),
    'default' => array(
      'options' => array(
        'date' => 'Date',
        'email' => 'Email',
        'number' => 'Number',
        'select' => 'Select',
        'textfield' => 'Textfield',
        'time' => 'Time',
      ),
      'default_value' => 'textfield',
    ),

    'description' => $description,
    'group' => 'dotgo_matches',
  );
  return $variable;
}

/**
 * Implements hook_variable_group_info().
 */
function dotgo_matches_variable_group_info() {
  $groups = array();
  $groups['dotgo_matches'] = array(
    'title' => t('DOTGO Match settings'),
    'description' => t('Settings for DOTGO Matches'),
    'access' => 'administer dotgo_matches',
  );
  return $groups;
}

/**
 * Implements hook_variable_type_info().
 */
function dotgo_matches_variable_type_info() {
  $types = array();
  $types['dotgo_matches_allowed_values'] = array(
    'title' => t('Allowed values'),
    'element' => array(
      '#type' => 'options',
      '#key_type' => 'custom',
      '#key_type_toggled' => TRUE,
    ),
    'default' => array('options' => array(), 'default_value' => ''),
    'element callback' => 'dotgo_matches_form_element_allowed_values',
    'validate callback' => 'dotgo_matches_form_element_validate_allowed_values',
  );
  return $types;
}

/**
 * Build $element array for 'dotgo_matches_allowed_values' variables.
 */
function dotgo_matches_form_element_allowed_values($variable, $options = array()) {
  $element = variable_form_element_default($variable, $options);
  $element['#options'] = $element['#default_value']['options'];
  $element['#default_value'] = $element['#default_value']['default_value'];

  // Add some text attempting to explain the key/value thing
  $element['#description'] .= ' ' . t('The key is what gets stored in the database and <em>sholdn\'t</em> be changed when there is already data using it. However, the value can be changed as much as you like!');

  // If this is a numeric field, be sure to tell the user!
  $numeric = isset($variable['numeric']) ? (bool)$variable['numeric'] : FALSE;
  if ($numeric) {
    $element['#description'] .= ' <strong>' . t('The keys for this field must be whole numbers.') . '</strong>';
  }

  return $element;
}

/**
 * Validate that the allowed values data is valid.
 */
function dotgo_matches_form_element_validate_allowed_values($variable) {
  $numeric = isset($variable['numeric']) ? (bool)$variable['numeric'] : FALSE;
  foreach ($variable['value']['options'] as $key => $value) {
    if ($numeric && !is_numeric($key)) {
      return t('The key %key is not a number.', array('%key' => $key));
    }
    if (strpos($value, '|') !== FALSE) {
      return t("Values cannot contain the pipe character '|'.");
    }
  }
  if ($numeric && !is_numeric($variable['value']['default_value'])) {
    return t('The default value must be a number.');
  }
}

