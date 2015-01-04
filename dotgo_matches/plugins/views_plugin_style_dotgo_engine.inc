<?php
/**
 * @file
 * Implements views_plugin_style for views_xml
 */

/**
 * Implements views_plugin_style
 */
class views_plugin_style_dotgo_engine extends views_plugin_style {

  /**
   * Implementation of views_style_plugin::theme_functions(). Returns an array of theme functions to use.
   * for the current style plugin
   * @return array
   */
  function theme_functions() {
    $options = $this->options;
    $hook = 'views_dotgo_engine_style';
    return views_theme_functions($hook, $this->view, $this->display);
  }

  /**
   * Implements views_style_plugin::additional_theme_functions(). Returns empty array.
   * @return array

  function additional_theme_functions() {
    return array();
  }*/

  /**
   * Implementation of view_style_plugin::render()
   */
  function render() {
    if ($this->uses_row_plugin() && empty($this->row_plugin)) {
      vpr('views_plugin_style_default: Missing row plugin');
      return;
    }

    $view = $this->view;
    $options = $this->options;

    // Group the rows according to the grouping field, if specified.
    $sets = $this->render_grouping($view->result, $options['grouping']);

    // Render each group separately and concatenate.  Plugins may override this
    // method if they wish some other way of handling grouping.
    $output = '';
    foreach ($sets as $title => $records) {
      if ($this->uses_row_plugin()) {
        $rows = array();
        foreach ($records as $row_index => $row) {
          $this->view->row_index = $row_index;
          $rows[] = $this->row_plugin->render($row);
        }
      }
      else {
        $rows = $records;
      }

      $output .= theme($this->theme_functions(), array(
        'view' => $view,
        'options' => $options,
        'rows' => $rows,
      ));
    }
    unset($this->view->row_index);
    return $output;
  }
}
