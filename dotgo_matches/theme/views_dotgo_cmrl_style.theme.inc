<?php

/**
 * Theme the output of the DOTGO CMRL View Style.
 * @param $vars
 */
function template_preprocess_views_dotgo_cmrl_style(&$vars) {
  $view = $vars["view"];
  $rows = $vars["rows"];
  $options = $vars["options"];
  $base = $view->base_table;
  $vars["content_type"] = 'text/xml';

  $xml = new DOMDocument();
  $xml->preserveWhiteSpace = TRUE;
  $xml->formatOutput = TRUE;
  //$xml->xmlEncoding = 'UTF-8';
  $cmrl = $xml->createElement('cmrl');
  $cmrl = $xml->appendChild($cmrl);
  $cmrl->setAttribute('xmlns:dotgo', DOTGO_XMLNS);
  $cmrl->setAttribute('account', variable_get('dotgo_account', 'dotgo_account'));

  foreach ($rows as $id => $row) {

    $node = $row['node'];
    $type = $node->field_dotgo_match_type[$node->language][0]['value'];
    $pattern = empty($node->field_dotgo_match_pattern) ? '' : $node->field_dotgo_match_pattern[$node->language][0]['value'];
    if($row['tree'] == $id){
      $make_match[$id] = _dotgo_make_match($cmrl, $pattern);
    }

    if(isset($make_match[$id])){
      switch ($type) {
        case "engine":
          $engine = $node->field_dotgo_match_engine[$node->language][0]['url'];
          if(empty($pattern)){
            $make_engine[$id] = _dotgo_make_engine($make_match[$id], $engine);
          } else {
            $sub_match[$id] = _dotgo_make_match($make_match[$id], '');
            $make_engine[$id] = _dotgo_make_engine($sub_match[$id], $engine);
          }
          break;
        case "default":
          $message = empty($node->field_dotgo_match_message) ? ' ' : $node->field_dotgo_match_message[$node->language][0]['value'];
          if (strpos($message, "<block>") === 0) {
            $fragment = $xml->createDocumentFragment();
            $fragment->appendXML($message);
            $make_match[$id]->appendChild($fragment);
          }
          else {
            $engine = 'matches/'.$node->nid;
            if(empty($pattern)){
              $make_engine[$id] = _dotgo_make_engine($make_match[$id], $engine);
            } else {
              $sub_match[$id] = _dotgo_make_match($make_match[$id], '');
              $make_engine[$id] = _dotgo_make_engine($sub_match[$id], $engine);
            }

          }
          break;
      }
    }
  }

  dotgo_api_render_engines($cmrl);

  $vars["xml"] = $xml->saveXML();
}
