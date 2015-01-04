<?php
/**
 * @file
 * Implements views_plugin_style for views_xml
 */

/**
 * Implements views_plugin_style
 */
class views_plugin_style_dotgo_cmrl extends views_plugin_style {
  /**
   * Implementation of views_plugin_style::option_definition
   */
  function option_definition() {
    $options = parent::option_definition();
    $author_default = isset($GLOBALS['user']->name) ? $GLOBALS['user']->name : variable_get('anonymous', t('Anonymous'));
    $options['nid'] = array('default' => NULL, 'translatable' => FALSE);
    $options['type'] = array('default' => NULL, 'translatable' => FALSE);
    $options['parent'] = array('default' => NULL, 'translatable' => FALSE);
    $options['weight'] = array('default' => NULL, 'translatable' => FALSE);
    $options['pattern'] = array('default' => NULL, 'translatable' => FALSE);
    $options['message'] = array('default' => NULL, 'translatable' => FALSE);
    $options['engine'] = array('default' => NULL, 'translatable' => FALSE);
    $options['using_views_api_mode'] = array(
      'default' => FALSE,
      'translatable' => FALSE
    );
    return $options;
  }

  /**
   * Provide a form for setting options.
   */
  function options_form(&$form, &$form_state) {

    $fields = $this->view->display_handler->display->display_options['fields'];
    // If no sorts avaialble for current display, use sorts from default display.
    if (empty($fields)) {
      $fields = $this->view->display['default']->display_options['fields'];
    }
    //dpm($fields);
    $field_options['none'] = '- None -';
    foreach ($fields as $field_option) {
      $field_name = $field_option['field'];

      if ($field_info = field_info_field($field_name)) {
        $views_field_data = field_views_field_default_views_data($field_info);
        $field_options[$field_option['table'] . ':' . $field_option['field']] = filter_xss($views_field_data[$field_option['table']][$field_option['field']]['title']);
      }
      else {
        $field_options[$field_option['table'] . ':' . $field_option['field']] = $field_option['label'];
      }
    }
    $form['nid'] = array(
      '#type' => 'select',
      '#title' => t('ID'),
      '#description' => t('For each row in the view, there needs to be a nid.'),
      '#options' => $field_options,
      '#default_value' => $this->options['nid'],
    );
    $form['type'] = array(
      '#type' => 'select',
      '#title' => t('Match Type Field'),
      '#description' => t('For each row in the view, there needs to be a pattern.'),
      '#options' => $field_options,
      '#default_value' => $this->options['type'],
    );
    $form['parent'] = array(
      '#type' => 'select',
      '#title' => t('Match Parent Field'),
      '#description' => t('For each row in the view, there can to be a parent.'),
      '#options' => $field_options,
      '#default_value' => $this->options['parent'],
    );
    $form['weight'] = array(
      '#type' => 'select',
      '#title' => t('Match Weight Field'),
      '#description' => t('For each row in the view, there can be a weight.'),
      '#options' => $field_options,
      '#default_value' => $this->options['weight'],
    );
    $form['using_views_api_mode'] = array(
      '#type' => 'checkbox',
      '#title' => t('Views API mode'),
      '#default_value' => $this->options['using_views_api_mode'],
      '#description' => t('Not using View API mode means the XML gets output directly and the server ceases normal page processing.  Using it means the server does not cease processing after outputting the JSON.  This allows the Views API to be used with the view without having to prematurely terminate page processing.'),
    );
  }

  /**
   * Implementation of views_style_plugin::theme_functions(). Returns an array of theme functions to use.
   * for the current style plugin
   * @return array
   */
  function theme_functions() {
    $options = $this->options;
    $hook = 'views_dotgo_cmrl_style';
    return views_theme_functions($hook, $this->view, $this->display);
  }

  /**
   * Implements views_style_plugin::additional_theme_functions(). Returns empty array.
   * @return array
   */
  function additional_theme_functions() {
    return array();
  }

  /**
   * Implementation of view_style_plugin::render()
   */
  function render() {
    $view = $this->view;
    $options = $this->options;
    $field = $view->field;

    $rows = _dotgo_cmrl_render_rows($view, $options);

    return theme($this->theme_functions(), array(
      'view' => $view,
      'options' => $options,
      'rows' => $rows
    ));
  }
}
