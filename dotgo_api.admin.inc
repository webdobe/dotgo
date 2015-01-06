<?php
/**
 * Menu callback for admin settings.
 */

function dotgo_api_settings() {
  $form = array();
  $form['dotgo'] = array(
    '#type' => 'fieldset',
    '#title' => t('DOTGO SETTINGS'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['dotgo']['dotgo_account'] = array(
    '#type' => 'textfield',
    '#title' => t('DOTGO Account'),
    '#description' => t('DOTGO Account example:7k5gt-r425f-gxpoo-lgl04'),
    '#default_value' => variable_get('dotgo_account', '7k5gt-r425f-gxpoo-lgl04'),
  );
  $form['dotgo']['dotgo_subdomain_designator'] = array(
    '#type' => 'checkbox',
    '#title' => t('Subdomain Designator'),
    '#description' => t('Allow subdomain in the designator.'),
    '#default_value' => variable_get('dotgo_subdomain_designator', 0),
  );
  $form['debug'] = array(
    '#type' => 'fieldset',
    '#title' => t('DEBUG SETTINGS'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['debug']['dotgo_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug Mode'),
    '#description' => t('Turns on Debug Mode which allows you to view index.cmrl and engines as anonymous for development purposes'),
    // Defaults to 1 until we save index.cmrl to DOTGO server.
    '#default_value' => variable_get('dotgo_debug', 1),
  );
  $form = system_settings_form($form);
  return $form;
}
