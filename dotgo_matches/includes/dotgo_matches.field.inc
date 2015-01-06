<?php
/**
 * @file
 * An example field using the Field Types API.
 */

/**
 * Implements hook_field_info().
 *
 * Provides the description of the field.
 */
function dotgo_matches_field_info() {
  return array(
    // We name our field as the associative name of the array.
    'dotgo_type' => array(
      'label' => t('DOTGO Type'),
      'description' => t('Provides field to select a match type.'),
      'default_widget' => 'options_select',
      'default_formatter' => 'dotgo_type_format',
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
 * @see dotgo_matches_field_widget_error()
 */
function dotgo_matches_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  /*foreach ($items as $delta => $item) {
    if (!empty($item['rgb'])) {
      if (! preg_match('@^#[0-9a-f]{6}$@', $item['rgb'])) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'dotgo_matches_invalid',
          'message' => t('Color must be in the HTML format #abcdef.'),
        );
      }
    }
  }*/
}


/**
 * Implements hook_field_is_empty().
 *
 * hook_field_is_emtpy() is where Drupal asks us if this field is empty.
 * Return TRUE if it does not contain cdata, FALSE if it does. This lets
 * the form API flag an error when required fields are empty.
 */
function dotgo_matches_field_is_empty($item, $field) {
  if ($field['type'] == 'dotgo_type') {
    if (!isset($item['value'])) {
      return TRUE;
    }
  }
  return FALSE;
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
 * @see dotgo_matches_field_widget_form()
 */
function dotgo_matches_field_widget_info() {
  return array(
    'dotgo_type_format' => array(
      'label' => t('Select list'),
      'field types' => array('dotgo_type'),
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
 * The 'dotgo_matches_colorpicker' and 'dotgo_matches_text' are essentially
 * the same, but dotgo_matches_colorpicker adds a javascript colorpicker
 * helper.
 *
 * dotgo_matches_component displays three text fields, one each for red, green,
 * and blue. However, the field type defines a single text column,
 * rgb, which needs an HTML color spec. Define an element validate
 * handler that converts our r, g, and b fields into a simulated single
 * 'rgb' form element.
 */
function dotgo_matches_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $value = isset($items[$delta]['value']) ? $items[$delta]['value'] : 'default';

  $widget = $element;
  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {

    case 'dotgo_type_format':
      if(isset($element['#entity'])){
        $entity = $element['#entity'];
        if(isset($entity->{$element['#field_name']}[$element['#language']][$element['#delta']])){
          $default = $entity->{$element['#field_name']}[$element['#language']][$element['#delta']];
        }
      }

      $options = array();
      $types = dotgo_info_types();
      foreach($types as $type => $info){
        $options[$type] = $info['label'];
      }

      // Make this a fieldset with the three text fields.
      $widget += array(
        '#type' => 'markup',
        '#element_validate' => array('dotgo_matches_format_validate'),

        // #delta is set so that the validation function will be able
        // to access external value information which otherwise would be
        // unavailable.
        '#delta' => $delta,

        '#prefix' => '<div id="dotgo-type">',
        '#suffix' => '</div>',
      );

      $widget['value'] = array(
        '#type' => 'select',
        '#title' => 'Type',
        '#options' => $options,
        '#default_value' => isset($default['value']) ? $default['value'] : 'default',
      );

      break;
  }

  $element = $widget;
  return $element;
}

/**
 * Validate the individual fields and then convert them into a single HTML RGB
 * value as text.
 */
function dotgo_matches_format_validate($element, &$form_state) {
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
function dotgo_matches_field_attach_validate($entity_type, $entity, &$errors) {
  if ($entity_type == 'node' && $entity->type == 'dotgo_match') {

  }
}

/**
 * Implementation of hook_field_attach_submit().
 */
function dotgo_matches_field_attach_submit($entity_type, $entity, $form, &$form_state) {;
  if ($entity_type == 'node' && $entity->type == 'dotgo_match') {

  }
}

/**
 * Implements hook_field_widget_error().
 *
 * hook_field_widget_error() lets us figure out what to do with errors
 * we might have generated in hook_field_validate(). Generally, we'll just
 * call form_error().
 *
 * @see dotgo_matches_field_validate()
 * @see form_error()
 */
function dotgo_matches_field_widget_error($element, $error, $form, &$form_state) {
  switch ($error['error']) {
    case 'dotgo_matches_invalid':
      form_error($element, $error['message']);
      break;
  }
}