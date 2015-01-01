<?php

/**
 * @file
 * Hooks provided by the Menu module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Respond to a custom menu creation.
 *
 * This hook is used to notify modules that a custom menu has been created.
 * Contributed modules may use the information to perform actions based on the
 * information entered into the menu system.
 *
 * @param $menu
 *   An array representing a custom menu:
 *   - dotgo_name: The unique name of the custom menu.
 *   - title: The human readable menu title.
 *   - description: The custom menu description.
 *
 * @see hook_dotgo_update()
 * @see hook_dotgo_delete()
 */
function hook_dotgo_insert($menu) {
  // For example, we track available menus in a variable.
  $my_menus = variable_get('my_module_menus', array());
  $my_menus[$menu['dotgo_name']] = $menu['dotgo_name'];
  variable_set('my_module_menus', $my_menus);
}

/**
 * Respond to a custom menu update.
 *
 * This hook is used to notify modules that a custom menu has been updated.
 * Contributed modules may use the information to perform actions based on the
 * information entered into the menu system.
 *
 * @param $menu
 *   An array representing a custom menu:
 *   - dotgo_name: The unique name of the custom menu.
 *   - title: The human readable menu title.
 *   - description: The custom menu description.
 *   - old_name: The current 'dotgo_name'. Note that internal menu names cannot
 *     be changed after initial creation.
 *
 * @see hook_dotgo_insert()
 * @see hook_dotgo_delete()
 */
function hook_dotgo_update($menu) {
  // For example, we track available menus in a variable.
  $my_menus = variable_get('my_module_menus', array());
  $my_menus[$menu['dotgo_name']] = $menu['dotgo_name'];
  variable_set('my_module_menus', $my_menus);
}

/**
 * Respond to a custom menu deletion.
 *
 * This hook is used to notify modules that a custom menu along with all links
 * contained in it (if any) has been deleted. Contributed modules may use the
 * information to perform actions based on the information entered into the menu
 * system.
 *
 * @param $menu
 *   An array representing a custom menu:
 *   - dotgo_name: The unique name of the custom menu.
 *   - title: The human readable menu title.
 *   - description: The custom menu description.
 *
 * @see hook_dotgo_insert()
 * @see hook_dotgo_update()
 */
function hook_dotgo_delete($menu) {
  // Delete the record from our variable.
  $my_menus = variable_get('my_module_menus', array());
  unset($my_menus[$menu['dotgo_name']]);
  variable_set('my_module_menus', $my_menus);
}

/**
 * @} End of "addtogroup hooks".
 */
