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
    '#description' => t('DOTGO Account example:456rc-o51oj-o5w3z-hpcr0'),
    '#default_value' => variable_get('dotgo_account', $dotgo_account),
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
    '#default_value' => variable_get('dotgo_debug', 0),
  );
  $form['debug']['dotgo_admin_ip'] = array(
    '#type' => 'textfield',
    '#title' => t('Administrator IP Mode'),
    '#description' => t('Turns on Debug Mode based on your IP address: ' . $_SERVER['REMOTE_ADDR']),
    '#default_value' => variable_get('dotgo_admin_ip', t('127.0.0.1')),
  );
  $form = system_settings_form($form);
  return $form;
}
