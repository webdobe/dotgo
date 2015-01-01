<?php
/**
 * Menu callback for admin settings.
 */

function dotgo_reseller_settings() {

  $keyword = _dotgo_designator();

  $describeSubaccounts = _dotgo_api_call($query = array(
    'method' => 'describeSubaccounts',
    'limit' => 1
  ));
  if ($describeSubaccounts['result_attr']['success'] == 1) {
    if (!$describeSubaccounts['result']['subaccounts']['item']) {
      $createSubaccount = _dotgo_api_call($query = array(
        'method' => 'createSubaccount',
        'subaccount' => $keyword
      ));
    }
  }

  $describeURLs = _dotgo_api_call($query = array(
    'method' => 'describeURLs',
    'limit' => 10,
    'subaccounts' => "*"
  ));
  if ($describeURLs['result_attr']['success'] == 1) {
    if (empty($describeURLs['result']['urls'])) {
      $addURL = _dotgo_api_call($query = array(
        'method' => 'addURL',
        'subaccount' => $keyword
      ));
      $reloadURL = _dotgo_api_call($query = array(
        'method' => 'reloadURL',
        'subaccount' => $keyword
      ));
    }
    else {
      $dotgo_account = $describeURLs['result']['urls']['item']['urlKey'];
      variable_set('dotgo_account', $dotgo_account);
    }
  }

  $describeService = _dotgo_api_call($query = array('method' => 'describeServices'));
  if ($describeService['result_attr']['success'] == 1) {
    if ($describeService['result']['services']['item']) {
      $service_level = variable_set('dotgo_message_service_level', $describeService['result']['services']['item']['serviceLevel']);
      $coupon = variable_set('dotgo_message_coupon', $describeService['result']['services']['item']['coupon']);
      $remaining = $describeService['result']['services']['item']['details']['creditsRemaining'];
      $total = $describeService['result']['services']['item']['details']['creditsTotal'];
      $used = $describeService['result']['services']['item']['details']['creditsUsed'];
      $expires = $describeService['result']['services']['item']['details']['creditsExpireTime'];
    }
  }

  $update = variable_get('dotgo_message_update_service', 1);

  if ($update == 1) {
    variable_set('dotgo_message_update_service', 0);
    $level = variable_get('dotgo_message_service_level', 0);
    $reloadURL = _dotgo_api_call($query = array(
      'method' => 'reloadURL',
      'subaccount' => $keyword
    ));
    if ($level != 0) {
      if (empty($describeService['result']['services'])) {
        $addService = _dotgo_api_call($query = array(
          'method' => 'addService',
          'service' => 'messaging',
          'subaccount' => $keyword,
          'serviceLevel' => variable_get('dotgo_message_service_level', 0),
          'coupon' => variable_get('dotgo_message_coupon', $coupon)
        ));
      }
      else {

        $modifyService = _dotgo_api_call($query = array(
          'method' => 'modifyService',
          'service' => 'messaging',
          'subaccount' => $keyword,
          'serviceLevel' => variable_get('dotgo_message_service_level', $service_level),
          'coupon' => variable_get('dotgo_message_coupon', $coupon)
        ));
      }
    }
  }

  $query = array(
    'method' => 'describeFollows',
    'limit' => 1,
  );

  $xmlarray = _dotgo_api_call($query);

  $subcount = $xmlarray['result']['follows_attr']['count'];

  $form = array();
  $form['dotgo'] = array(
    '#type' => 'fieldset',
    '#title' => t('DOTGO RESELLER SETTINGS'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['dotgo']['dotgo_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('DOTGO API Key'),
    '#description' => t('API Key distributed from DOTGO'),
    '#default_value' => variable_get('dotgo_api_key', t('thisisnotarealapikey')),
  );
  $form['dotgo']['dotgo_api_password'] = array(
    '#type' => 'textfield',
    '#title' => t('DOTGO API Password'),
    '#description' => t('API Password distributed from DOTGO'),
    '#default_value' => variable_get('dotgo_api_password', t('thisisnotarealapipassword')),
  );
  $form['messaging'] = array(
    '#type' => 'fieldset',
    '#title' => t('MESSAGING SETTINGS'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['messaging']['dotgo_message_credits'] = array(
    '#type' => 'markup',
    '#value' => '<table><tr><td>Total Subscribers</td><td>Remaining</td><td>Total</td><td>Used</td><td>Expires</td></tr><tr><td>' . $subcount . '</td><td>' . $remaining . '</td><td>' . $total . '</td><td>' . $used . '</td><td>' . date('M jS, Y', $expires) . '</td></tr></table>',
  );
  $form['messaging']['dotgo_message_coupon'] = array(
    '#type' => 'textfield',
    '#title' => t('Message Coupon'),
    '#description' => t('Message Coupon'),
    '#default_value' => variable_get('dotgo_message_coupon', $coupon),
  );
  $form['messaging']['dotgo_message_service_level'] = array(
    '#type' => 'radios',
    '#title' => t('Dotgo Service Level'),
    '#description' => t('Message Level'),
    '#default_value' => variable_get('dotgo_message_service_level', $service_level),
    '#options' => array(
      0 => 'Free: 0 Messages',
      1 => 'Basic: 1,000 Messages',
      2 => 'Standard: 10,000 Messages',
      3 => 'Pro: 100,000 Messages'
    ),
  );
  $form['messaging']['dotgo_message_update_service'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update Service'),
    '#description' => t('Update Messaging Service'),
    '#default_value' => variable_get('dotgo_message_update_service', 1),
  );
  $form = system_settings_form($form);
  return $form;
}