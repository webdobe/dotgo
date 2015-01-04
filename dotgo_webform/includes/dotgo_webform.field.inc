<?php
/**
 * @file
 * An example field using the Field Types API.
 */

/**
 * @defgroup dotgo_webform Example: Field Types API
 * @ingroup examples
 * @{
 * Examples using Field Types API.
 *
 * This is updated from Barry Jaspan's presentation at Drupalcon Paris,
 * @link http://acquia.com/community/resources/acquia-tv/intro-field-api-module-developers Video Presentation @endlink
 *
 * Providing a field requires:
 * - Defining a field:
 *   - hook_field_info()
 *   - hook_field_schema()
 *   - hook_field_validate()
 *   - hook_field_is_empty()
 *
 * - Defining a formatter for the field (the portion that outputs the field for
 *   display):
 *   - hook_field_formatter_info()
 *   - hook_field_formatter_view()
 *
 * - Defining a widget for the edit form:
 *   - hook_field_widget_info()
 *   - hook_field_widget_form()
 *
 * Our module defines the field in dotgo_webform_field_info(),
 * dotgo_webform_field_validate() and dotgo_webform_field_is_empty().
 * dotgo_webform_field_schema() is implemented in dotgo_webform.install.
 *
 * Our module sets up a formatter in dotgo_webform_field_formatter_info() and
 * dotgo_webform_field_formatter_view(). These are the API hooks that present
 * formatted and themed output to the user.

 * And finally, our module defines the widet in
 * dotgo_webform_field_widget_info() and dotgo_webform_field_widget_form().
 * The widget is the form element used to receive input from the user
 * when the field is being populated.
 *
 * @see field_types
 * @see field
 */

/***************************************************************
 * Field Type API hooks
 ***************************************************************/

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
  /*foreach ($items as $delta => $item) {
    if (!empty($item['rgb'])) {
      if (! preg_match('@^#[0-9a-f]{6}$@', $item['rgb'])) {
        $errors[$field['field_name']][$langcode][$delta][] = array(
          'error' => 'dotgo_webform_invalid',
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
    /*
    // This formatter changes the background color of the content region.
    'dotgo_webform_color_background' => array(
      'label' => t('Change the background of the output text'),
      'field types' => array('dotgo_webform'),
    ),*/
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
          '#tag' => 'p',
          '#attributes' => array(
            'style' => 'color: ' . $item['rgb'],
          ),
          '#value' => t('The color code in this field is @code', array('@code' => $item['rgb'])),
        );
      }
      break;
/*
    // This formatter adds css to the page changing the '.region-content' area's
    // background color. If there are many fields, the last one will win.
    case 'dotgo_webform_color_background':
      foreach ($items as $delta => $item) {
        $element[$delta] = array(
          '#type' => 'html_tag',
          '#tag' => 'p',
          '#value' => t('The content area color has been changed to @code', array('@code' => $item['rgb'])),
          '#attached' => array(
            'css' => array(
              array(
                'data' => 'div.region-content { background-color:' . $item['rgb'] . ';}',
                'type' => 'inline',
              ),
            ),
          ),
        );
      }
      break;*/
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
    /*
    'dotgo_webform_text' => array(
      'label' => t('RGB value as #ffffff'),
      'field types' => array('dotgo_webform'),
    ),*/
    'dotgo_webform_component' => array(
      'label' => t('DOTGO Webform Component'),
      'field types' => array('dotgo_webform'),
    ),
    /*
    'dotgo_webform_colorpicker' => array(
      'label' => t('Color Picker'),
      'field types' => array('dotgo_webform'),
    ),*/
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
/*
    case 'dotgo_webform_colorpicker':
      $widget += array(
        '#suffix' => '<div class="field-example-colorpicker"></div>',
        '#attributes' => array('class' => array('edit-field-example-colorpicker')),
        '#attached' => array(
          // Add Farbtastic color picker.
          'library' => array(
            array('system', 'farbtastic'),
          ),
          // Add javascript to trigger the colorpicker.
          'js' => array(drupal_get_path('module', 'dotgo_webform') . '/dotgo_webform.js'),
        ),
      );

    // DELIBERATE fall-through: From here on the dotgo_webform_text and
    // dotgo_webform_colorpicker are exactly the same.
    case 'dotgo_webform_text':
      $widget += array(
        '#type' => 'textfield',
        '#default_value' => $value,
        // Allow a slightly larger size that the field length to allow for some
        // configurations where all characters won't fit in input field.
        '#size' => 7,
        '#maxlength' => 7,
      );
      break;
*/
    case 'dotgo_webform_component':
      $options = array('_none' => '- Create new component -');
      $default_component = array();
      if(isset($element['#entity'])){
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
    $entity->match = dotgo_matches_load_match($entity);
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
    foreach($field_info as $field_name => $field){
      if(isset($field['widget']['type']) && $field['widget']['type'] == 'dotgo_webform_component'){
        foreach($form_state['input'][$field_name][$entity->language] as $delta => $values){
          // When component is set to none we create one.
          if ($values['cid'] == '_none' && $values['name'] && $values['form_key'] && $values['type']) {
            webform_ensure_record($entity->match);

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


/**
 * Implements hook_menu().
 *
 * Provides a simple user interface that tells the developer where to go.

function dotgo_webform_menu() {
  $items['examples/dotgo_webform'] = array(
    'title' => 'Field Example',
    'page callback' => '_dotgo_webform_page',
    'access callback' => TRUE,
  );
  return $items;
}
 */
/**
 * A simple page to explain to the developer what to do.

function _dotgo_webform_page() {
  return t("The Field Example provides a field composed of an HTML RGB value, like #ff00ff. To use it, add the field to a content type.");
}
/**
 * @} End of "defgroup dotgo_webform".
 */