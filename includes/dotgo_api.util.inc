<?php

/**
 * Implements _dotgo_message.
 *
 * @param $content.
 * @return return a message to the texter.
 */
function _dotgo_message($content) {
  $dom = new DOMDocument();
  $message = _dotgo_make_message($dom);
  $content = _dotgo_make_content($dom, $message, $content);
  return $dom->saveXML($message);
}

/**
 * Implements _dotgo_set_session.
 * Set a DOTGO session variable.
 * It persists forever unless you set it to empty string by using this call again
 *
 * @param array $set
 * @param string $confirmation
 * @return xml <block>
 */
function _dotgo_set_session($set = array(), $confirmation = 'OK') {
  $dom = new DOMDocument();
  $block = _dotgo_make_block($dom);
  foreach ($set as $name => $value) {
    $set = _dotgo_make_set($block, $name, $value);
  }
  $message = _dotgo_make_message($block);
  $content = _dotgo_make_content($dom, $message, $confirmation);
  return $dom->saveXML($block);
}

/**
 * Implements _dotgo_input_session.
 * Input a DOTGO session variable, it persists forever unless you set it to empty string.
 *
 * @param $input
 * @param array $set
 * @param string $question
 * @param $href
 * @param $options
 * @return xml <block>
 */
function _dotgo_input_session($input, $set = array(), $question = 'OK', $href, $options) {
  $dom = new DOMDocument();
  $block = _dotgo_make_block($dom);
  foreach ($set as $name => $value) {
    $set = _dotgo_make_set($block, $name, $value);
  }
  $message = _dotgo_make_message($block);
  $content = _dotgo_make_content($dom, $message, $question);
  $input = _dotgo_make_input($message, $input, $href, $options);
  return $dom->saveXML($block);
}

/**
 * Implements _dotgo_engine_session.
 * @param $href
 * @param $options
 * @return session <block>
 */
function _dotgo_engine_session($href, $options) {
  $dom = new DOMDocument();
  $block = _dotgo_make_block($dom);
  $engine = _dotgo_make_engine($block, $href, $options);
  return $dom->saveXML($block);
}

/**
 * Implements _dotgo_make_engine.
 * @param $parent
 * @param $href
 * @param array $options
 * @return <engine>
 */
function _dotgo_make_engine(&$parent, $href, $options = array()) {
  // allow someone to give us their own fully-built URL instead of a drupal path
  if (strpos($href, '://') == 0) {
    $options['absolute'] = TRUE;
    $href = url('dotgo/' . $href, $options);
    if (isset($_SERVER['HTTPS'])) {
      $href = str_replace('http://', 'https://', $href);
    }
  }

  $engine = new DOMElement('engine');
  $parent->appendChild($engine);
  $engine->setAttribute('href', $href);
  return $parent->appendChild($engine);
}

/**
 * Implements _dotgo_make_match.
 * @param $parent
 * @param $pattern
 * @return <match pattern="$pattern"><message><content>
 */
function _dotgo_make_match(&$parent, $pattern) {
  $match = new DOMElement('match');
  $parent->appendChild($match);
  $match->setAttribute('pattern', $pattern);
  return $parent->appendChild($match);
}

/**
 * Implements _dotgo_make_message.
 * @param $parent
 * @return <message><content>
 */
function _dotgo_make_message(&$parent) {
  $message = new DOMElement('message');
  return $parent->appendChild($message);
}

/**
 * Implements _dotgo_make_content.
 *
 * @param $dom
 * @param $parent
 * @param $text
 * @return xml <content>
 */
function _dotgo_make_content(&$dom, &$parent, $text) {
  $content = new DOMElement('content');
  $parent->appendChild($content);
  $string = $dom->createDocumentFragment();
  $string->appendXML($text);
  return $content->appendChild($string);
}

/**
 * Implements _dotgo_make_block.
 *
 * @param $parent
 * @return xml <block>
 */
function _dotgo_make_block(&$parent) {
  $block = new DOMElement('block');
  return $parent->appendChild($block);
}

/**
 * Implements _dotgo_make_set.
 * @param $parent
 * @param $name
 * @param $value
 * @return xml <set>
 */
function _dotgo_make_set(&$parent, $name, $value) {
  $set = new DOMElement('set', $value);
  $set = $parent->appendChild($set);
  $set->setAttribute('name', $name);
  return $parent->appendChild($set);
}

/**
 * @param $parent
 * @param $name
 * @param $href
 * @param $options
 * @return <input>
 */
function _dotgo_make_input(&$parent, $name, $href, $options) {
  $input = new DOMElement('input');
  $input = $parent->appendChild($input);
  $input->setAttribute('name', $name);
  $engine = _dotgo_make_engine($input, $href, $options);
  return $parent->appendChild($input);
}

/**
 * @param $parent
 * @param null $reg
 * @param null $password
 * @return follow <follow allow="$reg">
 */
function _dotgo_make_follow(&$parent, $reg = NULL, $password = NULL) {
  $follow = new DOMElement('follow');
  $follow = $parent->appendChild($follow);
  if ($reg) {
    $follow->setAttribute('allow', $reg);
  }
  if ($password) {
    $follow->setAttribute('password', $password);
  }
  return $parent->appendChild($follow);
}

/**
 * @param $parent
 * @param null $reg
 * @param null $password
 * @return mixed
 */
function _dotgo_make_register(&$parent, $reg = NULL, $password = NULL) {
  $register = new DOMElement('register');
  $register = $parent->appendChild($register);
  if ($reg) {
    $register->setAttribute('allow', $reg);
  }
  if ($password) {
    $register->setAttribute('password', $password);
  }
  return $parent->appendChild($register);
}

/**
 * @param $parent
 * @param null $reg
 * @return mixed
 */
function _dotgo_make_forward(&$parent, $reg = NULL) {
  $forward = new DOMElement('forward');
  $forward = $parent->appendChild($forward);
  if ($reg) {
    $forward->setAttribute('allow', $reg);
  }
  return $parent->appendChild($forward);
}

/**
 * @param $parent
 * @param $text
 * @return mixed
 */
function _dotgo_make_query(&$parent, $text) {
  $query = new DOMElement('query', $text);
  return $parent->appendChild($query);
}

/**
 * @param $query
 * @return string
 */
function _dotgo_make_anchor_query($query) {
  return '<a query="' . $query . '"/>';
}

/**
 * @param $content
 * @return string
 */
function _dotgo_make_anchor($content) {
  return '<message><content><anchor>' . $content . '</anchor></message></content>';
}

/**
 * @return string
 */
function _dotgo_designator() {
  $pieces = explode('.', $_SERVER['HTTP_HOST']);
  array_pop($pieces);
  return implode('.', $pieces);
}

/**
 * @return mixed
 */
function _dotgo_tld() {
  $pieces = explode('.', $_SERVER['HTTP_HOST']);
  return $pieces[count($pieces) - 1];
}

/**
 * @param $key
 * @return mixed
 */
function _dotgo_tld_lookup($key) {
  // DOTCOM (368266)—or to one of the phone numbers DOTEDU (368338), DOTGOV (368468), DOTNET (368638), or DOTORG (368674)
  $tlds = array(
    'com' => '368266',
    'edu' => '368338',
    'gov' => '368468',
    'net' => '368638',
    'org' => '368674',
  );

  return $tlds[$key];
}

/**
 * Implements _dotgo_get_parameters.
 */
function _dotgo_get_parameters($query = TRUE, $sys = TRUE, $session = FALSE){
  if($query){
    $parameters = drupal_get_query_parameters();
  }
  if($sys){
    $sys_parameters = array(
      'sys_channel',
      'sys_query',
      'sys_designator',
      'sys_path',
      'sys_argument',
      'sys_location',
      'sys_corrected_path',
      'sys_num_corrected_path',
      'sys_corrected_argument',
      'sys_num_corrected_argument_known',
      'sys_num_corrected_argument_unknown',
      'sys_num_corrected_argument'
    );
    foreach($sys_parameters as $parameter) {
      if(isset($_REQUEST[$parameter])){
        $parameters[$parameter] = $_REQUEST[$parameter];
      }
    }
  }
  return $parameters;
}

/**
 * Impliments _dotgo_build_menu.
 *
 * @param array $menus
 * @param null $type
 * @param array $items
 * @param null $account
 * @return string
 */
function _dotgo_build_menu($menus = array(), $type = NULL, $items = array(), $account = NULL) {
  $menus += module_invoke_all('dotgo_menu', $account);
  if ($menus) {
    foreach ($menus as $menu) {
      if ($type == NULL || $menu['type'] == NULL || $menu['type'] == $type) {
        if ($items) {
          foreach ($items as $item) {
            if ($menu['name'] == NULL || $menu['name'] != $item) {
              if (!$menu['perm'] || user_access($menu['perm'], $account, FALSE)) {
                $query .= _dotgo_make_anchor_query($menu['query']) . $menu['text'] . '<br />';
              }
            }
          }
        }
        else {
          if (!$menu['perm'] || user_access($menu['perm'], $account, FALSE)) {
            $query .= _dotgo_make_anchor_query($menu['query']) . $menu['text'] . '<br />';
          }
        }
      }
    }
    if ($query) {
      $msg .= '<br />REPLY MENU #<br />';
      $msg .= $query;
    }
    return $msg;
  }
}

/**
 * Implements _dotgo_follow_widget.
 *
 * NOTE: The widget is only useable for domains that are verified and added to a DOTGO account.
 *
 * @param array $query
 * @param array $options
 * @return iframe
 */
function _dotgo_follow_widget($query = array(), $options = array()) {

  // Widget specific Parameters
  // Change the line of text below your domain
  $query = array(
    'p' => ($query['p'] == NULL) ? '' : $query['p'],
    't' => ($query['t'] == NULL) ? 'Enter your phone number to follow the DOTGO Newsletter' : $query['t'],
    'c' => ($query['c'] == NULL) ? '000000' : $query['c'],
    'bg' => ($query['bg'] == NULL) ? 'transparent' : $query['bg'],
    'channel' => ($query['channel'] == NULL) ? _dotgo_tld_lookup('com') : $query['channel'],
    'query' => ($query['query'] == NULL) ? 'dotgo.com newsletter follow' : $query['query'],
  );

  // Iframe specific Attributes
  //$options['name'] = ($options['name'] == NULL) ? 'dotgo_signup_widget' : $options['name'];
  $options = array(
    'width' => ($options['width'] == NULL) ? '300px' : $options['width'],
    'height' => ($options['height'] == NULL) ? '200px' : $options['height'],
    'frameborder' => ($options['frameborder'] == NULL) ? 0 : $options['frameborder'],
  );

  $url = url('https://dotgo.com/api/webform/1.0/', array(
    'query' => $query,
    'external' => TRUE,
  ));

  $output = '<iframe width="' . $options['width'] . '" height="' . $options['height'] . '" frameborder="' . $options['frameborder'] . '" src="' . $url . '"></iframe>';

  return $output;

}