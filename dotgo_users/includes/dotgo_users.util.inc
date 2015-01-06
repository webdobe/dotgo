<?php

/**
 * @param $uid
 * @return null
 */
function _dotgo_users_get($uid) {
  if($uid){
    $result = db_query('SELECT * FROM {dotgo_users} u WHERE uid = :uid', array(':uid' => $uid));
    $record = $result->fetchObject();
    if (!$record->token) {
      $record->token = user_password(6);
      db_query('INSERT INTO {dotgo_users} (uid, token) VALUES (:uid, :token)', array(':uid' => $uid, ':token' => $record->token));
    }
    return $record;
  }
  return NULL;
}

/**
 * @param $reg
 * @param $href
 * @param $options
 * @param $text
 * @return string
 */
function dotgo_users_follow_block($reg, $href, $options, $text) {
  $dom = new DOMDocument();
  $block = _dotgo_make_block($dom);
  $follow = _dotgo_make_follow($block, $reg);
  $query = _dotgo_make_query($block, $text);
  $engine = _dotgo_make_engine($follow, $href, $options);
  return $dom->saveXML($block);
}

/**
 * @param $reg
 * @param $href
 * @param $options
 * @return string
 */
function dotgo_users_register_block($reg, $href, $options) {
  $dom = new DOMDocument();
  $block = _dotgo_make_block($dom);
  $register = _dotgo_make_register($block);
  $engine = _dotgo_make_engine($register, $href, $options);
  $forward = _dotgo_make_forward($block, $reg);
  return $dom->saveXML($block);
}

/**
 * @param $sys_user
 * @param $email_address
 * @return mixed
 */
function dotgo_users_add($sys_user, $email_address) {
  $newUser = array(
    'name' => $sys_user,
    'pass' => user_password(), // note: do not md5 the password
    'mail' => $email_address,
    'status' => 1,
    'init' => $email_address,
  );

  $account = user_save(NULL, $newUser);

  dotgo_users_update('update', $account->uid, $sys_user);

  $account->password = $newUser['pass'];
  $mail = _user_mail_notify('register_no_approval_required', $account);
  return $account;
}

/**
 * @param $sys_user
 * @return null
 */
function dotgo_users_load($sys_user) {
  $result = db_query('SELECT u.uid FROM {dotgo_users} u WHERE u.sys_user = :sys_user', array(':sys_user' => $sys_user));
  $record = $result->fetchObject();

  if($record){
    return user_load($record->uid);
  }

  return NULL;
}

/**
 * @param string $op
 * @param $uid
 * @param $sys_user
 */
function dotgo_users_update($op = 'update', $uid, $sys_user) {
  if ($op == 'delete') {
    db_query('DELETE FROM {dotgo_users} WHERE uid = :uid', array(':uid' => $uid));
  }
  else {
    $result = db_query('SELECT uid FROM {dotgo_users} user WHERE uid = :uid', array(':uid' => $uid));
    $record = $result->fetchObject();
    $data = array(
      'uid' => $uid,
      'sys_user' => $sys_user,
    );
    if ($record) {
      drupal_write_record('dotgo_users', $data, array('uid'));
    }
    else {
      drupal_write_record('dotgo_users', $data);
    }
  }
}