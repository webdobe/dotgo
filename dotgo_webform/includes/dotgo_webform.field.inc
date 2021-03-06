<?php
/**
 * @file
 * File Implements Field API.
 */

/**
 * Implements hook_field_info().
 *
 * Provides the description of the field.
 */
function dotgo_webform_field_info() {
  return array(
    // We name our field as the associative name of the array.
    'dotgo_webform' => array(
      'label' => t('DOTGO Webform'),
      'description' => t('Provides field to select/create a match webform component.'),
      'default_widget' => 'dotgo_webform_component',
      'default_formatter' => 'dotgo_webform_input',
    ),
  );
}

/**
 * Implements hook_field_validate().
 *
 * This hook gives us a chance to validate content that's in our
 * field. We're really only interested in the $items parameter, since
 * it holds arrays representing content in the field we've defined.
 * We want to verify that the items only contain RGB hex values like
 * this: #RRGGBB. If the item validates, we do nothing. If it doesn't
 * validate, we add our own error notification to the $errors parameter.
 *
 * @see dotgo_webform_field_widget_error()
 */
function dotgo_webform_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {

}


/**
 * Implements hook_field_is_empty().
 *
 * hook_field_is_emtpy() is where Drupal asks us if this field is empty.
 * Return TRUE if it does not contain cdata, FALSE if it does. This lets
 * the form API flag an error when required fields are empty.
 */
function dotgo_webform_field_is_empty($item, $field) {
  if ($field['type'] == 'dotgo_webform') {
    if (!is_numeric($item['cid'])) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Implements hook_field_formatter_info().
 *
 * We need to tell Drupal that we have two different types of formatters
 * for this field. One will change the text color, and the other will
 * change the background color.
 *
 * @see dotgo_webform_field_formatter_view()
 */
function dotgo_webform_field_formatter_info() {
  return array(
    // This formatter just displays the hex value in the color indicated.
    'dotgo_webform_input' => array(
      'label' => t('Dotgo webform input'),
      'field types' => array('dotgo_webform'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 *
 * Two formatters are implemented.
 * - dotgo_webform_input just outputs markup indicating the color that
 *   was entered and uses an inline style to set the text color to that value.
 * - dotgo_webform_color_background does the same but also changes the
 *   background color of div.region-content.
 *
 * @see dotgo_webform_field_formatter_info()
 */
function dotgo_webform_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    // This formatter simply outputs the field as text and with a color.
    case 'dotgo_webform_input':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          // We create a render array to produce the desired markup,
          // "<p style="color: #hexcolor">The color code ... #hexcolor</p>".
          // See theme_html_tag().
          '#type' => 'html_tag',
          '#tag' => 'div',
          '#value' => t($item['name']),
        );
      }
      break;

  }

  return $element;
}

/**
 * Implements hook_field_widget_info().
 *
 * Three widgets are provided.
 * - A simple text-only widget where the user enters the '#ffffff'.
 * - A 3-textfield widget that gathers the red, green, and blue values
 *   separately.
 * - A farbtastic colorpicker widget that chooses the value graphically.
 *
 * These widget types will eventually show up in hook_field_widget_form,
 * where we will have to flesh them out.
 *
 * @see dotgo_webform_field_widget_form()
 */
function dotgo_webform_field_widget_info() {
  return array(
    'dotgo_webform_component' => array(
      'label' => t('Webform Component'),
      'field types' => array('dotgo_webform'),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 *
 * hook_widget_form() is where Drupal tells us to create form elements for
 * our field's widget.
 *
 * We provide one of three different forms, depending on the widget type of
 * the Form API item provided.
 *
 * The 'dotgo_webform_colorpicker' and 'dotgo_webform_text' are essentially
 * the same, but dotgo_webform_colorpicker adds a javascript colorpicker
 * helper.
 *
 * dotgo_webform_component displays three text fields, one each for red, green,
 * and blue. However, the field type defines a single text column,
 * rgb, which needs an HTML color spec. Define an element validate
 * handler that converts our r, g, and b fields into a simulated single
 * 'rgb' form element.
 */
function dotgo_webform_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $value = isset($items[$delta]['component']) ? $items[$delta]['component'] : '';

  $widget = $element;
  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {
    case 'dotgo_webform_component':
      $options = array('_none' => '- Create new component -');
      $default_component = array();
      if(isset($element['#entity']->nid)){
        $entity = $element['#entity'];
        $components = dotgo_webform_get_match_components($entity);
        if(!empty($components)){
          if(isset($form_state['values'])){
            unset($form_state['input'][$element['#field_name']]);
            if(isset($form_state['values'][$element['#field_name']])){
              $cid = $form_state['values'][$element['#field_name']][$element['#language']][$element['#delta']]['cid'];
              $entity->{$element['#field_name']}[$element['#language']][$element['#delta']]['cid'] = $cid;
            }
          }

          foreach ($components as $cid => $component) {
            $options[$cid] = check_plain($component['name']);

            if(isset($entity->{$element['#field_name']}[$element['#language']][$element['#delta']])){
              if($component['cid'] == $entity->{$element['#field_name']}[$element['#language']][$element['#delta']]['cid']){
                $default_component = $component;
              }
            }
          }
        }
      }

      $weight = 0;

      // Make this a fieldset with the three text fields.
      $widget += array(
        '#type' => 'markup',
        '#element_validate' => array('dotgo_webform_component_validate'),

        // #delta is set so that the validation function will be able
        // to access external value information which otherwise would be
        // unavailable.
        '#delta' => $delta,

        '#attached' => array(
          'css' => array(drupal_get_path('module', 'dotgo_webform') . '/dotgo_webform.css'),
        ),
        '#prefix' => '<div id="dotgo-webform-'.$delta.'" class="dotgo-webform">',
        '#suffix' => '</div>',
      );

      if(isset($default_component['cid'])){
        $widget['edit'] = array(
          '#prefix' => '<div id="dotgo-webform-cid-'.$delta.'-wrapper" class="dotgo-webform-item"><div class="form-item"><label for="edit-field-dotgo-match-webform-und-0-cid">Component </label>',
          '#suffix' => '</div></div>',
          '#type' => 'submit',
          '#title' => 'Component',
          '#value' => 'Edit '.$default_component['name'],
          '#submit' => array('_dotgo_webform_cid_submit'),
        );
        $widget['cid'] = array(
          '#type' => 'hidden',
          '#value' => isset($default_component['cid']) ? $default_component['cid'] : '_none',
        );
      } else {
        $widget['cid'] = array(
          '#prefix' => '<div id="dotgo-webform-cid-'.$delta.'-wrapper" class="dotgo-webform-item">',
          '#suffix' => '</div>',
          '#type' => 'select',
          '#title' => 'Component',
          '#options' => $options,
          '#weight' => $weight++,
          '#default_value' => isset($default_component['cid']) ? $default_component['cid'] : '_none',
          '#ajax' => array(
            'callback' => '_dotgo_webform_elements_callback',
            'wrapper' => 'dotgo-webform-'.$delta,
          ),
        );
      }

      $widget['name'] = array(
        '#prefix' => '<div id="dotgo-webform-name-'.$delta.'-wrapper" class="dotgo-webform-item">',
        '#suffix' => '</div>',
        '#type' => 'textfield',
        '#title' => 'Name',
        '#weight' => $weight++,
        '#default_value' => isset($default_component['name']) ? check_plain($default_component['name']) : '',
      );

      $widget['form_key'] = array(
        '#type' => 'machine_name',
        '#title' => 'Key',
        '#weight' => $weight++,
        '#default_value' => isset($default_component['form_key']) ? $default_component['form_key'] : '',
        '#required' => FALSE,
        '#machine_name' => array(
          'exists' => '_dotgo_webform_element_form_key_exists',
          'source' => array($field['field_name'], $langcode, $delta, 'name'),
          'label' => t('Key'),
          'replace_pattern' => '[^a-z0-9_]+',
          'replace' => '_',
        ),
      );

      $widget['type'] = array(
        '#prefix' => '<div id="dotgo-webform-type-'.$delta.'-wrapper" class="dotgo-webform-item">',
        '#suffix' => '</div>',
        '#type' => 'select',
        '#title' => 'Type',
        '#default_value' => isset($default_component['type']) ? $default_component['type'] : 'textfield',
        '#options' => dotgo_matches_get_form_types(),
        '#weight' => $weight++,
      );

      if(!empty($default_component)){
        $widget['name']['#disabled'] = TRUE;
        $widget['type']['#disabled'] = TRUE;
      }

      break;
  }

  $element = $widget;
  return $element;
}

/**
 * Form callback elements.
 */
function _dotgo_webform_elements_callback($form, $form_state) {
  $parents = $form_state['triggering_element']['#parents'];
  array_pop($parents);
  $commands = array();
  $wrapper = drupal_array_get_nested_value($form, $parents);

  $name_render = render($wrapper['name']);
  $commands[] = ajax_command_replace('#dotgo-webform-name-'.$wrapper['#delta'].'-wrapper', $name_render);

  $type_render = render($wrapper['type']);
  $commands[] = ajax_command_replace('#dotgo-webform-type-'.$wrapper['#delta'].'-wrapper', $type_render);

  return array('#type' => 'ajax', '#commands' => $commands);
}

/**
 * Form callback to check if form element exists.
 * Currently always returns FALSE.
 */
function _dotgo_webform_cid_submit($form, &$form_state) {
  $node = $form['#node'];
  $cid = $form_state['values']['field_dotgo_match_webform'][$node->language][0]['cid'];

  $url = "node/" . $node->match->nid . "/webform/components/" . $cid;

  $form_state['redirect'] = array(
    $url, array(
      'query' => drupal_get_destination()
    )
  );

  unset($_GET['destination']);
  drupal_static_reset('drupal_get_destination');
  drupal_get_destination();
}

/**
 * Form callback to check if form element exists.
 *
 *
 * Currently always returns FALSE.
 */
function _dotgo_webform_element_form_key_exists() {
  return FALSE;
}

/**
 * Validate the individual fields and then convert them into a single HTML RGB
 * value as text.
 */
function dotgo_webform_component_validate($element, &$form_state) {
  $delta = $element['#delta']; // TODO: Isn't there a better way to find out which element?
  $field = $form_state['field'][$element['#field_name']][$element['#language']]['field'];
  $field_name = $field['field_name'];
  if (isset($form_state['values'][$field_name][$element['#language']][$delta])) {
    $value = $form_state['values'][$field_name][$element['#language']][$delta];
  }
  form_set_value($element, $value, $form_state);
}

/*
 * Implementation of hook_field_attach_validate
 */
function dotgo_webform_field_attach_validate($entity_type, $entity, &$errors) {
  if ($entity_type == 'node' && $entity->type == 'dotgo_match') {
    $field_info = field_info_instances($entity_type, $entity->type);
    foreach($field_info as $field_name => $field){
      if(isset($field['widget']['type']) && $field['widget']['type'] == 'dotgo_webform'){
        $form_key = $entity->{$field_name}[$entity->language][0]['form_key'];
        if($form_key){
          if (!preg_match('/^[a-z0-9_]+$/i', $form_key)) {
            $errors[$field_name][$entity->language][0][] = array(
              'error' => 'form_key',
              'message' => t('The name %field_key is invalid. Please include only lowercase alphanumeric characters and underscores.', array('%field_key' => $form_key)),
            );
          }
          foreach ($entity->match->webform['components'] as $cid => $component) {
            if ((strcasecmp($component['form_key'], $form_key) == 0)) {
              $errors[$field_name][$entity->language][0][] = array(
                'error' => 'form_key',
                'message' => t('The name %field_key is already in use by the field labeled %existing_field. Please use a unique key.', array('%field_key' => $form_key, '%existing_field' => $component['name'])),
              );
            }
          }
        }
      }
    }
  }
}

/**
 * Implementation of hook_field_attach_submit().
 */
function dotgo_webform_field_attach_submit($entity_type, $entity, $form, &$form_state) {;
  if ($entity_type == 'node' && $entity->type == 'dotgo_match') {
    $field_info = field_info_instances($entity_type, $entity->type);
    $entity->match = dotgo_matches_load_match($entity);
    if(!$entity->match && isset($entity->field_dotgo_match[$entity->language][0]['target_id'])) {
      $entity->match = node_load($entity->field_dotgo_match[$entity->language][0]['target_id']);
    }
    foreach($field_info as $field_name => $field){
      if(isset($field['widget']['type']) && $field['widget']['type'] == 'dotgo_webform_component'){
        foreach($form_state['input'][$field_name][$entity->language] as $delta => $values){
          // When component is set to none we create one.
          if ($values['cid'] == '_none' && $values['name'] && $values['form_key'] && $values['type']) {

            $component = array(
              'nid' => $entity->match->nid,
              'pid' => 0,
              'form_key' => $values['form_key'],
              'name' => $values['name'],
              'type' => $values['type'],
              'extra' => array(),
              'weight' => 0,
            );

            $values['cid'] = webform_component_insert($component);
            drupal_set_message(t('New component %name added.', array('%name' => $values['name'])));

            // We make sure the field is getting the new cid
            $entity->{$field_name}[$entity->language][$delta] = $values;

            $url = "node/" . $entity->match->nid . "/webform/components/" . $values['cid'];

            $form_state['redirect'] = array(
              $url, array(
                'query' => drupal_get_destination()
              )
            );

            unset($_GET['destination']);
            drupal_static_reset('drupal_get_destination');
            drupal_get_destination();
          }
        }
      }
    }
  }
}

/**
 * Implements hook_field_widget_error().
 *
 * hook_field_widget_error() lets us figure out what to do with errors
 * we might have generated in hook_field_validate(). Generally, we'll just
 * call form_error().
 *
 * @see dotgo_webform_field_validate()
 * @see form_error()
 */
function dotgo_webform_field_widget_error($element, $error, $form, &$form_state) {
  switch ($error['error']) {
    case 'dotgo_webform_invalid':
      form_error($element, $error['message']);
      break;
  }
}