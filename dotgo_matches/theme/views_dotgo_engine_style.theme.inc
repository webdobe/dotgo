<?php

/**
 * Theme the output of the DOTGO CMRL View Style.
 * @param $vars
 */
function template_preprocess_views_dotgo_engine_style(&$vars) {
  $view = $vars["view"];
  $rows = $vars["rows"];
  $options = $vars["options"];
  $base = $view->base_table;
  $vars["content_type"] = 'text/xml';

  $message = '';
  foreach($rows as $row){
    $message .= $row;
  }

  // NEEDS DONE: Adding pager ability to engine.
  if ($view->display_handler->render_pager()) {
    $exposed_input = isset($view->exposed_raw_input) ? $view->exposed_raw_input : NULL;
  }

  $vars["xml"] = _dotgo_message($message);
}
