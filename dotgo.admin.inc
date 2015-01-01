<?php

/**
 * @file
 * Administrative page callbacks for dotgo module.
 */

/**
 * Menu callback which shows an overview page of all the custom dotgos and their descriptions.
 */
function dotgo_overview_page() {
  $result = db_query("SELECT * FROM {dotgo_custom} ORDER BY title", array(), array('fetch' => PDO::FETCH_ASSOC));
  $header = array(
    t('Title'),
    array('data' => t('Operations'), 'colspan' => '3')
  );
  $rows = array();
  foreach ($result as $dotgo) {
    $row = array(
      theme('dotgo_admin_overview', array(
        'title' => $dotgo['title'],
        'name' => $dotgo['dotgo_name'],
        'description' => $dotgo['description']
      ))
    );
    $row[] = array('data' => l(t('list links'), 'admin/structure/dotgo/manage/' . $dotgo['dotgo_name']));
    $row[] = array('data' => l(t('edit dotgo'), 'admin/structure/dotgo/manage/' . $dotgo['dotgo_name'] . '/edit'));
    $row[] = array('data' => l(t('add link'), 'admin/structure/dotgo/manage/' . $dotgo['dotgo_name'] . '/add'));
    $rows[] = $row;
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}

/**
 * Returns HTML for a dotgo title and description for the dotgo overview page.
 *
 * @param $variables
 *   An associative array containing:
 *   - title: The dotgo's title.
 *   - description: The dotgo's description.
 *
 * @ingroup themeable
 */
function theme_dotgo_admin_overview($variables) {
  $output = check_plain($variables['title']);
  $output .= '<div class="description">' . filter_xss_admin($variables['description']) . '</div>';

  return $output;
}

/**
 * Form for editing an entire dotgo tree at once.
 *
 * Shows for one dotgo the dotgo links accessible to the current user and
 * relevant operations.
 */
function dotgo_overview_form($form, &$form_state, $dotgo) {
  global $dotgo_admin;
  $form['#attached']['css'] = array(drupal_get_path('module', 'dotgo') . '/dotgo.css');
  $sql = "
    SELECT m.load_functions, m.to_arg_functions, m.access_callback, m.access_arguments, m.page_callback, m.page_arguments, m.delivery_callback, m.title, m.title_callback, m.title_arguments, m.type, m.description, ml.*
    FROM {dotgo_links} ml LEFT JOIN {dotgo_router} m ON m.path = ml.router_path
    WHERE ml.dotgo_name = :dotgo
    ORDER BY p1 ASC, p2 ASC, p3 ASC, p4 ASC, p5 ASC, p6 ASC, p7 ASC, p8 ASC, p9 ASC";
  $result = db_query($sql, array(':dotgo' => $dotgo['dotgo_name']), array('fetch' => PDO::FETCH_ASSOC));
  $links = array();
  foreach ($result as $item) {
    $links[] = $item;
  }
  $tree = dotgo_tree_data($links);
  $node_links = array();
  dotgo_tree_collect_node_links($tree, $node_links);
  // We indicate that a dotgo administrator is running the dotgo access check.
  $dotgo_admin = TRUE;
  dotgo_tree_check_access($tree, $node_links);
  $dotgo_admin = FALSE;

  $form = array_merge($form, _dotgo_overview_tree_form($tree));
  $form['#dotgo'] = $dotgo;

  if (element_children($form)) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );
  }
  else {
    $form['#empty_text'] = t('There are no dotgo links yet. <a href="@link">Add link</a>.', array('@link' => url('admin/structure/dotgo/manage/' . $form['#dotgo']['dotgo_name'] . '/add')));
  }
  return $form;
}

/**
 * Recursive helper function for dotgo_overview_form().
 *
 * @param $tree
 *   The dotgo_tree retrieved by dotgo_tree_data.
 */
function _dotgo_overview_tree_form($tree) {
  $form = & drupal_static(__FUNCTION__, array('#tree' => TRUE));
  foreach ($tree as $data) {
    $title = '';
    $item = $data['link'];
    // Don't show callbacks; these have $item['hidden'] < 0.
    if ($item && $item['hidden'] >= 0) {
      $mlid = 'mlid:' . $item['mlid'];
      $form[$mlid]['#item'] = $item;
      $form[$mlid]['#attributes'] = $item['hidden'] ? array('class' => array('dotgo-disabled')) : array('class' => array('dotgo-enabled'));
      $form[$mlid]['title']['#markup'] = l($item['title'], $item['href'], $item['localized_options']);
      if ($item['hidden']) {
        $form[$mlid]['title']['#markup'] .= ' (' . t('disabled') . ')';
      }
      elseif ($item['link_path'] == 'user' && $item['module'] == 'system') {
        $form[$mlid]['title']['#markup'] .= ' (' . t('logged in users only') . ')';
      }

      $form[$mlid]['hidden'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable @title dotgo link', array('@title' => $item['title'])),
        '#title_display' => 'invisible',
        '#default_value' => !$item['hidden'],
      );
      $form[$mlid]['weight'] = array(
        '#type' => 'weight',
        '#delta' => 50,
        '#default_value' => $item['weight'],
        '#title_display' => 'invisible',
        '#title' => t('Weight for @title', array('@title' => $item['title'])),
      );
      $form[$mlid]['mlid'] = array(
        '#type' => 'hidden',
        '#value' => $item['mlid'],
      );
      $form[$mlid]['plid'] = array(
        '#type' => 'hidden',
        '#default_value' => $item['plid'],
      );
      // Build a list of operations.
      $operations = array();
      $operations['edit'] = array(
        '#type' => 'link',
        '#title' => t('edit'),
        '#href' => 'admin/structure/dotgo/item/' . $item['mlid'] . '/edit'
      );
      // Only items created by the dotgo module can be deleted.
      if ($item['module'] == 'dotgo' || $item['updated'] == 1) {
        $operations['delete'] = array(
          '#type' => 'link',
          '#title' => t('delete'),
          '#href' => 'admin/structure/dotgo/item/' . $item['mlid'] . '/delete'
        );
      }
      // Set the reset column.
      elseif ($item['module'] == 'system' && $item['customized']) {
        $operations['reset'] = array(
          '#type' => 'link',
          '#title' => t('reset'),
          '#href' => 'admin/structure/dotgo/item/' . $item['mlid'] . '/reset'
        );
      }
      $form[$mlid]['operations'] = $operations;
    }

    if ($data['below']) {
      _dotgo_overview_tree_form($data['below']);
    }
  }
  return $form;
}

/**
 * Submit handler for the dotgo overview form.
 *
 * This function takes great care in saving parent items first, then items
 * underneath them. Saving items in the incorrect order can break the dotgo tree.
 *
 * @see dotgo_overview_form()
 */
function dotgo_overview_form_submit($form, &$form_state) {
  // When dealing with saving dotgo items, the order in which these items are
  // saved is critical. If a changed child item is saved before its parent,
  // the child item could be saved with an invalid path past its immediate
  // parent. To prevent this, save items in the form in the same order they
  // are sent by $_POST, ensuring parents are saved first, then their children.
  // See http://drupal.org/node/181126#comment-632270
  $order = array_flip(array_keys($form_state['input'])); // Get the $_POST order.
  $form = array_merge($order, $form); // Update our original form with the new order.

  $updated_items = array();
  $fields = array('weight', 'plid');
  foreach (element_children($form) as $mlid) {
    if (isset($form[$mlid]['#item'])) {
      $element = $form[$mlid];
      // Update any fields that have changed in this dotgo item.
      foreach ($fields as $field) {
        if ($element[$field]['#value'] != $element[$field]['#default_value']) {
          $element['#item'][$field] = $element[$field]['#value'];
          $updated_items[$mlid] = $element['#item'];
        }
      }
      // Hidden is a special case, the value needs to be reversed.
      if ($element['hidden']['#value'] != $element['hidden']['#default_value']) {
        // Convert to integer rather than boolean due to PDO cast to string.
        $element['#item']['hidden'] = $element['hidden']['#value'] ? 0 : 1;
        $updated_items[$mlid] = $element['#item'];
      }
    }
  }

  // Save all our changed items to the database.
  foreach ($updated_items as $item) {
    $item['customized'] = 1;
    dotgo_link_save($item);
  }
  drupal_set_message(t('Your configuration has been saved.'));
}

/**
 * Returns HTML for the dotgo overview form into a table.
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_dotgo_overview_form($variables) {
  $form = $variables['form'];

  drupal_add_tabledrag('dotgo-overview', 'match', 'parent', 'dotgo-plid', 'dotgo-plid', 'dotgo-mlid', TRUE, MENU_MAX_DEPTH - 1);
  drupal_add_tabledrag('dotgo-overview', 'order', 'sibling', 'dotgo-weight');

  $header = array(
    t('Menu link'),
    array('data' => t('Enabled'), 'class' => array('checkbox')),
    t('Weight'),
    array('data' => t('Operations'), 'colspan' => '3'),
  );

  $rows = array();
  foreach (element_children($form) as $mlid) {
    if (isset($form[$mlid]['hidden'])) {
      $element = & $form[$mlid];
      // Build a list of operations.
      $operations = array();
      foreach (element_children($element['operations']) as $op) {
        $operations[] = array(
          'data' => drupal_render($element['operations'][$op]),
          'class' => array('dotgo-operations')
        );
      }
      while (count($operations) < 2) {
        $operations[] = '';
      }

      // Add special classes to be used for tabledrag.js.
      $element['plid']['#attributes']['class'] = array('dotgo-plid');
      $element['mlid']['#attributes']['class'] = array('dotgo-mlid');
      $element['weight']['#attributes']['class'] = array('dotgo-weight');

      // Change the parent field to a hidden. This allows any value but hides the field.
      $element['plid']['#type'] = 'hidden';

      $row = array();
      $row[] = theme('indentation', array('size' => $element['#item']['depth'] - 1)) . drupal_render($element['title']);
      $row[] = array(
        'data' => drupal_render($element['hidden']),
        'class' => array('checkbox', 'dotgo-enabled')
      );
      $row[] = drupal_render($element['weight']) . drupal_render($element['plid']) . drupal_render($element['mlid']);
      $row = array_merge($row, $operations);

      $row = array_merge(array('data' => $row), $element['#attributes']);
      $row['class'][] = 'draggable';
      $rows[] = $row;
    }
  }
  $output = '';
  if (empty($rows)) {
    $rows[] = array(array('data' => $form['#empty_text'], 'colspan' => '7'));
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'dotgo-overview')
  ));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Menu callback; Build the dotgo link editing form.
 */
function dotgo_edit_item($form, &$form_state, $type, $item, $dotgo) {
  if ($type == 'add' || empty($item)) {
    // This is an add form, initialize the dotgo link.
    $item = array(
      'link_title' => '',
      'mlid' => 0,
      'plid' => 0,
      'dotgo_name' => $dotgo['dotgo_name'],
      'weight' => 0,
      'link_path' => '',
      'options' => array(),
      'module' => 'dotgo',
      'expanded' => 0,
      'hidden' => 0,
      'has_children' => 0
    );
  }
  else {
    // Get the human-readable dotgo title from the given dotgo name.
    $titles = dotgo_get_dotgos();
    $current_title = $titles[$item['dotgo_name']];

    // Get the current breadcrumb and add a link to that dotgo's overview page.
    $breadcrumb = dotgo_get_active_breadcrumb();
    $breadcrumb[] = l($current_title, 'admin/structure/dotgo/manage/' . $item['dotgo_name']);
    drupal_set_breadcrumb($breadcrumb);
  }
  $form['actions'] = array('#type' => 'actions');
  $form['link_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu link title'),
    '#default_value' => $item['link_title'],
    '#description' => t('The text to be used for this link in the dotgo.'),
    '#required' => TRUE,
  );
  foreach (array(
             'link_path',
             'mlid',
             'module',
             'has_children',
             'options'
           ) as $key) {
    $form[$key] = array('#type' => 'value', '#value' => $item[$key]);
  }
  // Any item created or edited via this interface is considered "customized".
  $form['customized'] = array('#type' => 'value', '#value' => 1);
  $form['original_item'] = array('#type' => 'value', '#value' => $item);

  $path = $item['link_path'];
  if (isset($item['options']['query'])) {
    $path .= '?' . drupal_http_build_query($item['options']['query']);
  }
  if (isset($item['options']['fragment'])) {
    $path .= '#' . $item['options']['fragment'];
  }
  if ($item['module'] == 'dotgo') {
    $form['link_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#maxlength' => 255,
      '#default_value' => $path,
      '#description' => t('The path for this dotgo link. This can be an internal Drupal path such as %add-node or an external URL such as %drupal. Enter %front to link to the front page.', array(
        '%front' => '<front>',
        '%add-node' => 'node/add',
        '%drupal' => 'http://drupal.org'
      )),
      '#required' => TRUE,
    );
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#access' => $item['mlid'],
      '#submit' => array('dotgo_item_delete_submit'),
      '#weight' => 10,
    );
  }
  else {
    $form['_path'] = array(
      '#type' => 'item',
      '#title' => t('Path'),
      '#description' => l($item['link_title'], $item['href'], $item['options']),
    );
  }
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => isset($item['options']['attributes']['title']) ? $item['options']['attributes']['title'] : '',
    '#rows' => 1,
    '#description' => t('Shown when hovering over the dotgo link.'),
  );
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => !$item['hidden'],
    '#description' => t('Menu links that are not enabled will not be listed in any dotgo.'),
  );
  $form['expanded'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show as expanded'),
    '#default_value' => $item['expanded'],
    '#description' => t('If selected and this dotgo link has children, the dotgo will always appear expanded.'),
  );

  // Generate a list of possible parents (not including this link or descendants).
  $options = dotgo_parent_options(dotgo_get_dotgos(), $item);
  $default = $item['dotgo_name'] . ':' . $item['plid'];
  if (!isset($options[$default])) {
    $default = 'navigation:0';
  }
  $form['parent'] = array(
    '#type' => 'select',
    '#title' => t('Parent link'),
    '#default_value' => $default,
    '#options' => $options,
    '#description' => t('The maximum depth for a link and all its children is fixed at !maxdepth. Some dotgo links may not be available as parents if selecting them would exceed this limit.', array('!maxdepth' => MENU_MAX_DEPTH)),
    '#attributes' => array('class' => array('dotgo-title-select')),
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#delta' => 50,
    '#default_value' => $item['weight'],
    '#description' => t('Optional. In the dotgo, the heavier links will sink and the lighter links will be positioned nearer the top.'),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );

  return $form;
}

/**
 * Validate form values for a dotgo link being added or edited.
 */
function dotgo_edit_item_validate($form, &$form_state) {
  $item = & $form_state['values'];
  $normal_path = drupal_get_normal_path($item['link_path']);
  if ($item['link_path'] != $normal_path) {
    drupal_set_message(t('The dotgo system stores system paths only, but will use the URL alias for display. %link_path has been stored as %normal_path', array(
      '%link_path' => $item['link_path'],
      '%normal_path' => $normal_path
    )));
    $item['link_path'] = $normal_path;
  }
  if (!url_is_external($item['link_path'])) {
    $parsed_link = parse_url($item['link_path']);
    if (isset($parsed_link['query'])) {
      $item['options']['query'] = drupal_get_query_array($parsed_link['query']);
    }
    else {
      // Use unset() rather than setting to empty string
      // to avoid redundant serialized data being stored.
      unset($item['options']['query']);
    }
    if (isset($parsed_link['fragment'])) {
      $item['options']['fragment'] = $parsed_link['fragment'];
    }
    else {
      unset($item['options']['fragment']);
    }
    if (isset($parsed_link['path']) && $item['link_path'] != $parsed_link['path']) {
      $item['link_path'] = $parsed_link['path'];
    }
  }
  if (!trim($item['link_path']) || !drupal_valid_path($item['link_path'], TRUE)) {
    form_set_error('link_path', t("The path '@link_path' is either invalid or you do not have access to it.", array('@link_path' => $item['link_path'])));
  }
}

/**
 * Submit function for the delete button on the dotgo item editing form.
 */
function dotgo_item_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/dotgo/item/' . $form_state['values']['mlid'] . '/delete';
}

/**
 * Process dotgo and dotgo item add/edit form submissions.
 */
function dotgo_edit_item_submit($form, &$form_state) {
  $item = & $form_state['values'];

  // The value of "hidden" is the opposite of the value
  // supplied by the "enabled" checkbox.
  $item['hidden'] = (int) !$item['enabled'];
  unset($item['enabled']);

  $item['options']['attributes']['title'] = $item['description'];
  list($item['dotgo_name'], $item['plid']) = explode(':', $item['parent']);
  if (!dotgo_link_save($item)) {
    drupal_set_message(t('There was an error saving the dotgo link.'), 'error');
  }
  else {
    drupal_set_message(t('Your configuration has been saved.'));
  }
  $form_state['redirect'] = 'admin/structure/dotgo/manage/' . $item['dotgo_name'];
}

/**
 * Menu callback; Build the form that handles the adding/editing of a custom dotgo.
 */
function dotgo_edit_dotgo($form, &$form_state, $type, $dotgo = array()) {
  $system_dotgos = dotgo_list_system_dotgos();
  $dotgo += array(
    'dotgo_name' => '',
    'old_name' => !empty($dotgo['dotgo_name']) ? $dotgo['dotgo_name'] : '',
    'title' => '',
    'description' => '',
  );
  // Allow dotgo_edit_dotgo_submit() and other form submit handlers to determine
  // whether the dotgo already exists.
  $form['#insert'] = empty($dotgo['old_name']);
  $form['old_name'] = array(
    '#type' => 'value',
    '#value' => $dotgo['old_name'],
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#default_value' => $dotgo['title'],
    '#required' => TRUE,
    // The title of a system dotgo cannot be altered.
    '#access' => !isset($system_dotgos[$dotgo['dotgo_name']]),
  );

  $form['dotgo_name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Menu name'),
    '#default_value' => $dotgo['dotgo_name'],
    '#maxlength' => MENU_MAX_MENU_NAME_LENGTH_UI,
    '#description' => t('A unique name to construct the URL for the dotgo. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'exists' => 'dotgo_edit_dotgo_name_exists',
      'source' => array('title'),
      'replace_pattern' => '[^a-z0-9-]+',
      'replace' => '-',
    ),
    // A dotgo's machine name cannot be changed.
    '#disabled' => !empty($dotgo['old_name']) || isset($system_dotgos[$dotgo['dotgo_name']]),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $dotgo['description'],
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  // Only custom dotgos may be deleted.
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#access' => $type == 'edit' && !isset($system_dotgos[$dotgo['dotgo_name']]),
    '#submit' => array('dotgo_custom_delete_submit'),
  );

  return $form;
}

/**
 * Submit function for the 'Delete' button on the dotgo editing form.
 */
function dotgo_custom_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/dotgo/manage/' . $form_state['values']['dotgo_name'] . '/delete';
}

/**
 * Menu callback; check access and get a confirm form for deletion of a custom dotgo.
 */
function dotgo_delete_dotgo_page($dotgo) {
  // System-defined dotgos may not be deleted.
  $system_dotgos = dotgo_list_system_dotgos();
  if (isset($system_dotgos[$dotgo['dotgo_name']])) {
    drupal_access_denied();
    return;
  }
  return drupal_get_form('dotgo_delete_dotgo_confirm', $dotgo);
}

/**
 * Build a confirm form for deletion of a custom dotgo.
 */
function dotgo_delete_dotgo_confirm($form, &$form_state, $dotgo) {
  $form['#dotgo'] = $dotgo;
  $caption = '';
  $num_links = db_query("SELECT COUNT(*) FROM {dotgo_links} WHERE dotgo_name = :dotgo", array(':dotgo' => $dotgo['dotgo_name']))->fetchField();
  if ($num_links) {
    $caption .= '<p>' . format_plural($num_links, '<strong>Warning:</strong> There is currently 1 dotgo link in %title. It will be deleted (system-defined items will be reset).', '<strong>Warning:</strong> There are currently @count dotgo links in %title. They will be deleted (system-defined links will be reset).', array('%title' => $dotgo['title'])) . '</p>';
  }
  $caption .= '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, t('Are you sure you want to delete the custom dotgo %title?', array('%title' => $dotgo['title'])), 'admin/structure/dotgo/manage/' . $dotgo['dotgo_name'], $caption, t('Delete'));
}

/**
 * Delete a custom dotgo and all links in it.
 */
function dotgo_delete_dotgo_confirm_submit($form, &$form_state) {
  $dotgo = $form['#dotgo'];
  $form_state['redirect'] = 'admin/structure/dotgo';

  // System-defined dotgos may not be deleted - only dotgos defined by this module.
  $system_dotgos = dotgo_list_system_dotgos();
  if (isset($system_dotgos[$dotgo['dotgo_name']]) || !(db_query("SELECT 1 FROM {dotgo_custom} WHERE dotgo_name = :dotgo", array(':dotgo' => $dotgo['dotgo_name']))->fetchField())) {
    return;
  }

  // Reset all the dotgo links defined by the system via hook_dotgo().
  $result = db_query("SELECT * FROM {dotgo_links} ml INNER JOIN {dotgo_router} m ON ml.router_path = m.path WHERE ml.dotgo_name = :dotgo AND ml.module = 'system' ORDER BY m.number_parts ASC", array(':dotgo' => $dotgo['dotgo_name']), array('fetch' => PDO::FETCH_ASSOC));
  foreach ($result as $link) {
    dotgo_reset_item($link);
  }

  // Delete all links to the overview page for this dotgo.
  $result = db_query("SELECT mlid FROM {dotgo_links} ml WHERE ml.link_path = :link", array(':link' => 'admin/structure/dotgo/manage/' . $dotgo['dotgo_name']), array('fetch' => PDO::FETCH_ASSOC));
  foreach ($result as $link) {
    dotgo_link_delete($link['mlid']);
  }

  // Delete the custom dotgo and all its dotgo links.
  dotgo_delete($dotgo);

  $t_args = array('%title' => $dotgo['title']);
  drupal_set_message(t('The custom dotgo %title has been deleted.', $t_args));
  watchdog('dotgo', 'Deleted custom dotgo %title and all its dotgo links.', $t_args, WATCHDOG_NOTICE);
}

/**
 * Returns whether a dotgo name already exists.
 *
 * @see dotgo_edit_dotgo()
 * @see form_validate_machine_name()
 */
function dotgo_edit_dotgo_name_exists($value) {
  // 'dotgo-' is added to the dotgo name to avoid name-space conflicts.
  $value = 'dotgo-' . $value;
  $custom_exists = db_query_range('SELECT 1 FROM {dotgo_custom} WHERE dotgo_name = :dotgo', 0, 1, array(':dotgo' => $value))->fetchField();
  $link_exists = db_query_range("SELECT 1 FROM {dotgo_links} WHERE dotgo_name = :dotgo", 0, 1, array(':dotgo' => $value))->fetchField();

  return $custom_exists || $link_exists;
}

/**
 * Submit function for adding or editing a custom dotgo.
 */
function dotgo_edit_dotgo_submit($form, &$form_state) {
  $dotgo = $form_state['values'];
  $path = 'admin/structure/dotgo/manage/';
  if ($form['#insert']) {
    // Add 'dotgo-' to the dotgo name to help avoid name-space conflicts.
    $dotgo['dotgo_name'] = 'dotgo-' . $dotgo['dotgo_name'];
    $link['link_title'] = $dotgo['title'];
    $link['link_path'] = $path . $dotgo['dotgo_name'];
    $link['router_path'] = $path . '%';
    $link['module'] = 'dotgo';
    $link['plid'] = db_query("SELECT mlid FROM {dotgo_links} WHERE link_path = :link AND module = :module", array(
      ':link' => 'admin/structure/dotgo',
      ':module' => 'system'
    ))
      ->fetchField();

    dotgo_link_save($link);
    dotgo_save($dotgo);
  }
  else {
    dotgo_save($dotgo);
    $result = db_query("SELECT mlid FROM {dotgo_links} WHERE link_path = :path", array(':path' => $path . $dotgo['dotgo_name']), array('fetch' => PDO::FETCH_ASSOC));
    foreach ($result as $m) {
      $link = dotgo_link_load($m['mlid']);
      $link['link_title'] = $dotgo['title'];
      dotgo_link_save($link);
    }
  }
  drupal_set_message(t('Your configuration has been saved.'));
  $form_state['redirect'] = $path . $dotgo['dotgo_name'];
}

/**
 * Menu callback; Check access and present a confirm form for deleting a dotgo link.
 */
function dotgo_item_delete_page($item) {
  // Links defined via hook_dotgo may not be deleted. Updated items are an
  // exception, as they can be broken.
  if ($item['module'] == 'system' && !$item['updated']) {
    drupal_access_denied();
    return;
  }
  return drupal_get_form('dotgo_item_delete_form', $item);
}

/**
 * Build a confirm form for deletion of a single dotgo link.
 */
function dotgo_item_delete_form($form, &$form_state, $item) {
  $form['#item'] = $item;
  return confirm_form($form, t('Are you sure you want to delete the custom dotgo link %item?', array('%item' => $item['link_title'])), 'admin/structure/dotgo/manage/' . $item['dotgo_name']);
}

/**
 * Process dotgo delete form submissions.
 */
function dotgo_item_delete_form_submit($form, &$form_state) {
  $item = $form['#item'];
  dotgo_link_delete($item['mlid']);
  $t_args = array('%title' => $item['link_title']);
  drupal_set_message(t('The dotgo link %title has been deleted.', $t_args));
  watchdog('dotgo', 'Deleted dotgo link %title.', $t_args, WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/structure/dotgo/manage/' . $item['dotgo_name'];
}

/**
 * Menu callback; reset a single modified dotgo link.
 */
function dotgo_reset_item_confirm($form, &$form_state, $item) {
  $form['item'] = array('#type' => 'value', '#value' => $item);
  return confirm_form($form, t('Are you sure you want to reset the link %item to its default values?', array('%item' => $item['link_title'])), 'admin/structure/dotgo/manage/' . $item['dotgo_name'], t('Any customizations will be lost. This action cannot be undone.'), t('Reset'));
}

/**
 * Process dotgo reset item form submissions.
 */
function dotgo_reset_item_confirm_submit($form, &$form_state) {
  $item = $form_state['values']['item'];
  $new_item = dotgo_reset_item($item);
  drupal_set_message(t('The dotgo link was reset to its default settings.'));
  $form_state['redirect'] = 'admin/structure/dotgo/manage/' . $new_item['dotgo_name'];
}

/**
 * Menu callback; Build the form presenting dotgo configuration options.
 */
function dotgo_configure() {
  $form['intro'] = array(
    '#type' => 'item',
    '#markup' => t('The dotgo module allows on-the-fly creation of dotgo links in the content authoring forms. To configure these settings for a particular content type, visit the <a href="@content-types">Content types</a> page, click the <em>edit</em> link for the content type, and go to the <em>Menu settings</em> section.', array('@content-types' => url('admin/structure/types'))),
  );

  $dotgo_options = dotgo_get_dotgos();

  $main = variable_get('dotgo_main_links_source', 'main-dotgo');
  $form['dotgo_main_links_source'] = array(
    '#type' => 'select',
    '#title' => t('Source for the Main links'),
    '#default_value' => variable_get('dotgo_main_links_source', 'main-dotgo'),
    '#empty_option' => t('No Main links'),
    '#options' => $dotgo_options,
    '#tree' => FALSE,
    '#description' => t('Select what should be displayed as the Main links (typically at the top of the page).'),
  );

  $form['dotgo_secondary_links_source'] = array(
    '#type' => 'select',
    '#title' => t('Source for the Secondary links'),
    '#default_value' => variable_get('dotgo_secondary_links_source', 'user-dotgo'),
    '#empty_option' => t('No Secondary links'),
    '#options' => $dotgo_options,
    '#tree' => FALSE,
    '#description' => t('Select the source for the Secondary links. An advanced option allows you to use the same source for both Main links (currently %main) and Secondary links: if your source dotgo has two levels of hierarchy, the top level dotgo links will appear in the Main links, and the children of the active link will appear in the Secondary links.', array('%main' => $main ? $dotgo_options[$main] : t('none'))),
  );

  return system_settings_form($form);
}
