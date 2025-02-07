<?php
// $Id: path.inc,v 1.3 2006/01/24 08:18:26 dries Exp $

/**
 * @file
 * Functions to handle paths in Drupal, including path aliasing.
 *
 * These functions are not loaded for cached pages, but modules that need
 * to use them in hook_init() or hook exit() can make them available, by
 * executing "drupal_bootstrap(DRUPAL_BOOTSTRAP_PATH);".
 */

/**
 * Initialize the $_GET['q'] variable to the proper normal path.
 */
function drupal_init_path() {
  if (!empty($_GET['q'])) {
    $_GET['q'] = drupal_get_normal_path(trim($_GET['q'], '/'));
  }
  else {
    $_GET['q'] = drupal_get_normal_path(variable_get('site_frontpage', 'node'));
  }
}

/**
 * Given an alias, return its Drupal system URL if one exists. Given a Drupal
 * system URL return its alias if one exists.
 *
 * @param $action
 *   One of the following values:
 *   - wipe: delete the alias cache.
 *   - alias: return an alias for a given Drupal system path (if one exists).
 *   - source: return the Drupal system URL for a path alias (if one exists).
 * @param $path
 *   The path to investigate for corresponding aliases or system URLs.
 *
 * @return
 *   Either a Drupal system path, an aliased path, or FALSE if no path was
 *   found.
 */
function drupal_lookup_path($action, $path = '') {
  static $map = array();
  static $count = NULL;

  if ($count === NULL) {
    $count = db_result(db_query('SELECT COUNT(pid) FROM {url_alias}'));
  }

  if ($action == 'wipe') {
    $map = array();
  }
  elseif ($count > 0 && $path != '') {
    if ($action == 'alias') {
      if (isset($map[$path])) {
        return $map[$path];
      }
      if ($alias = db_result(db_query("SELECT dst FROM {url_alias} WHERE src = '%s'", $path))) {
        $map[$path] = $alias;
        return $alias;
      }
      else {
        $map[$path] = $path;
      }
    }
    elseif ($action == 'source') {
      if ($alias = array_search($path, $map)) {
        return $alias;
      }
      if (!isset($map[$path])) {
        if ($src = db_result(db_query("SELECT src FROM {url_alias} WHERE dst = '%s'", $path))) {
          $map[$src] = $path;
          return $src;
        }
      }
    }
  }

  return FALSE;
}

/**
 * Given an internal Drupal path, return the alias set by the administrator.
 *
 * @param $path
 *   An internal Drupal path.
 *
 * @return
 *   An aliased path if one was found, or the original path if no alias was
 *   found.
 */
function drupal_get_path_alias($path) {
  $result = $path;
  if ($alias = drupal_lookup_path('alias', $path)) {
    $result = $alias;
  }
  if (function_exists('custom_url_rewrite')) {
    $result = custom_url_rewrite('alias', $result, $path);
  }
  return $result;
}

/**
 * Given a path alias, return the internal path it represents.
 *
 * @param $path
 *   A Drupal path alias.
 *
 * @return
 *   The internal path represented by the alias, or the original alias if no
 *   internal path was found.
 */
function drupal_get_normal_path($path) {
  $result = $path;
  if ($src = drupal_lookup_path('source', $path)) {
    $result = $src;
  }
  if (function_exists('custom_url_rewrite')) {
    $result = custom_url_rewrite('source', $result, $path);
  }
  return $result;
}

/**
 * Return a component of the current Drupal path.
 *
 * When viewing a page at the path "admin/node/configure", for example, arg(0)
 * would return "admin", arg(1) would return "node", and arg(2) would return
 * "configure".
 *
 * Avoid use of this function where possible, as resulting code is hard to read.
 * Instead, attempt to use named arguments in menu callback functions. See the
 * explanation in menu.inc for how to construct callbacks that take arguments.
 *
 * @param $index
 *   The index of the component, where each component is separated by a '/'
 *   (forward-slash), and where the first component has an index of 0 (zero).
 *
 * @return
 *   The component specified by $index, or FALSE if the specified component was
 *   not found.
 */
function arg($index) {
  static $arguments, $q;

  if (empty($arguments) || $q != $_GET['q']) {
    $arguments = explode('/', $_GET['q']);
    $q = $_GET['q'];
  }

  if (isset($arguments[$index])) {
    return $arguments[$index];
  }
}

/**
 * Get the title of the current page, for display on the page and in the title bar.
 *
 * @return
 *   The current page's title.
 */
function drupal_get_title() {
  $title = drupal_set_title();

  // during a bootstrap, menu.inc is not included and thus we cannot provide a title
  if (!isset($title) && function_exists('menu_get_active_title')) {
    $title = check_plain(menu_get_active_title());
  }

  return $title;
}

/**
 * Set the title of the current page, for display on the page and in the title bar.
 *
 * @param $title
 *   Optional string value to assign to the page title; or if set to NULL
 *   (default), leaves the current title unchanged.
 *
 * @return
 *   The updated title of the current page.
 */
function drupal_set_title($title = NULL) {
  static $stored_title;

  if (isset($title)) {
    $stored_title = $title;
  }
  return $stored_title;
}
