<?php
// $Id: theme.inc,v 1.289 2006/04/07 10:40:55 unconed Exp $

/**
 * @file
 * The theme system, which controls the output of Drupal.
 *
 * The theme system allows for nearly all output of the Drupal system to be
 * customized by user themes.
 *
 * @see <a href="http://drupal.org/node/253">Theme system</a>
 * @see themeable
 */

 /**
 * @name Content markers
 * @{
 * Markers used by theme_mark() and node_mark() to designate content.
 * @see theme_mark(), node_mark()
 */
define('MARK_READ',    0);
define('MARK_NEW',     1);
define('MARK_UPDATED', 2);
/**
 * @} End of "Content markers".
 */

/**
 * Initialize the theme system by loading the theme.
 *
 */
function init_theme() {
  global $theme, $user, $custom_theme, $theme_engine, $theme_key;

  // If $theme is already set, assume the others are set, too, and do nothing
  if (isset($theme)) {
    return;
  }

  drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);
  $themes = list_themes();

  // Only select the user selected theme if it is available in the
  // list of enabled themes.
  $theme = $user->theme && $themes[$user->theme]->status ? $user->theme : variable_get('theme_default', 'bluemarine');

  // Allow modules to override the present theme... only select custom theme
  // if it is available in the list of installed themes.
  $theme = $custom_theme && $themes[$custom_theme] ? $custom_theme : $theme;

  // Store the identifier for retrieving theme settings with.
  $theme_key = $theme;

  // If we're using a style, load its appropriate theme,
  // which is stored in the style's description field.
  // Also load the stylesheet using theme_add_style().
  // Otherwise, load the theme.
  if (strpos($themes[$theme]->filename, '.css')) {
    // File is a style; loads its CSS.
    // Set theme to its template/theme
    theme_add_style($themes[$theme]->filename);
    $theme = basename(dirname($themes[$theme]->description));
  }
  else {
    // File is a template/theme
    // Load its CSS, if it exists
    if (file_exists($stylesheet = dirname($themes[$theme]->filename) .'/style.css')) {
      theme_add_style($stylesheet);
    }
  }

  if (strpos($themes[$theme]->filename, '.theme')) {
   // file is a theme; include it
   include_once './' . $themes[$theme]->filename;
  }
  elseif (strpos($themes[$theme]->description, '.engine')) {
    // file is a template; include its engine
    include_once './' . $themes[$theme]->description;
    $theme_engine = basename($themes[$theme]->description, '.engine');
    if (function_exists($theme_engine .'_init')) {
      call_user_func($theme_engine .'_init', $themes[$theme]);
    }
  }
}

/**
 * Provides a list of currently available themes.
 *
 * @param $refresh
 *   Whether to reload the list of themes from the database.
 * @return
 *   An array of the currently available themes.
 */
function list_themes($refresh = FALSE) {
  static $list;

  if ($refresh) {
    unset($list);
  }

  if (!$list) {
    $list = array();
    $result = db_query("SELECT * FROM {system} WHERE type = 'theme'");
    while ($theme = db_fetch_object($result)) {
      if (file_exists($theme->filename)) {
        $list[$theme->name] = $theme;
      }
    }
  }

  return $list;
}

/**
 * Provides a list of currently available theme engines
 *
 * @param $refresh
 *   Whether to reload the list of themes from the database.
 * @return
 *   An array of the currently available theme engines.
 */
function list_theme_engines($refresh = FALSE) {
  static $list;

  if ($refresh) {
    unset($list);
  }

  if (!$list) {
    $list = array();
    $result = db_query("SELECT * FROM {system} WHERE type = 'theme_engine' AND status = '1' ORDER BY name");
    while ($engine = db_fetch_object($result)) {
      if (file_exists($engine->filename)) {
        $list[$engine->name] = $engine;
      }
    }
  }

  return $list;
}

/**
 * Generate the themed representation of a Drupal object.
 *
 * All requests for themed functions must go through this function. It examines
 * the request and routes it to the appropriate theme function. If the current
 * theme does not implement the requested function, then the current theme
 * engine is checked. If neither the engine nor theme implement the requested
 * function, then the base theme function is called.
 *
 * For example, to retrieve the HTML that is output by theme_page($output), a
 * module should call theme('page', $output).
 *
 * @param $function
 *   The name of the theme function to call.
 * @param ...
 *   Additional arguments to pass along to the theme function.
 * @return
 *   An HTML string that generates the themed output.
 */
function theme() {
  $args = func_get_args();
  $function = array_shift($args);

  if ($func = theme_get_function($function)) {
    return call_user_func_array($func, $args);
  }
}

/**
 * Determine if a theme function exists, and if so return which one was found.
 *
 * @param $function
 *   The name of the theme function to test.
 * @return
 *   The name of the theme function that should be used, or false if no function exists.
 */
function theme_get_function($function) {
  global $theme, $theme_engine;

  // Because theme() is called a lot, calling init_theme() only to have it
  // smartly return is a noticeable performance hit.   Don't do it.
  if (!isset($theme)) {
    init_theme();
  }

  if (($theme != '') && function_exists($theme .'_'. $function)) {
    // call theme function
    return $theme .'_'. $function;
  }
  elseif (($theme != '') && isset($theme_engine) && function_exists($theme_engine .'_'. $function)) {
    // call engine function
    return $theme_engine .'_'. $function;
  }
  elseif (function_exists('theme_'. $function)){
    // call Drupal function
    return 'theme_'. $function;
  }
  return false;
}

/**
 * Return the path to the currently selected theme.
 */
function path_to_theme() {
  global $theme;

  $themes = list_themes();

  return dirname($themes[$theme]->filename);
}

/**
 * Retrieve an associative array containing the settings for a theme.
 *
 * The final settings are arrived at by merging the default settings,
 * the site-wide settings, and the settings defined for the specific theme.
 * If no $key was specified, only the site-wide theme defaults are retrieved.
 *
 * The default values for each of settings are also defined in this function.
 * To add new settings, add their default values here, and then add form elements
 * to system_theme_settings() in system.module.
 *
 * @param $key
 *  The template/style value for a given theme.
 *
 * @return
 *   An associative array containing theme settings.
 */
function theme_get_settings($key = NULL) {
  $defaults = array(
    'mission'                       =>  '',
    'default_logo'                  =>  1,
    'logo_path'                     =>  '',
    'default_favicon'               =>  1,
    'favicon_path'                  =>  '',
    'toggle_logo'                   =>  1,
    'toggle_favicon'                =>  1,
    'toggle_name'                   =>  1,
    'toggle_search'                 =>  1,
    'toggle_slogan'                 =>  0,
    'toggle_mission'                =>  1,
    'toggle_node_user_picture'      =>  0,
    'toggle_comment_user_picture'   =>  0,
  );

  if (module_exist('node')) {
    foreach (node_get_types() as $type) {
      $defaults['toggle_node_info_' . $type] = 1;
    }
  }
  $settings = array_merge($defaults, variable_get('theme_settings', array()));

  if ($key) {
    $settings = array_merge($settings, variable_get(str_replace('/', '_', 'theme_'. $key .'_settings'), array()));
  }

  // Only offer search box if search.module is enabled.
  if (!module_exist('search') || !user_access('search content')) {
    $settings['toggle_search'] = 0;
  }

  return $settings;
}

/**
 * Retrieve a setting for the current theme.
 * This function is designed for use from within themes & engines
 * to determine theme settings made in the admin interface.
 *
 * Caches values for speed (use $refresh = TRUE to refresh cache)
 *
 * @param $setting_name
 *  The name of the setting to be retrieved.
 *
 * @param $refresh
 *  Whether to reload the cache of settings.
 *
 * @return
 *   The value of the requested setting, NULL if the setting does not exist.
 */
function theme_get_setting($setting_name, $refresh = FALSE) {
  global $theme_key;
  static $settings;

  if (empty($settings) || $refresh) {
    $settings = theme_get_settings($theme_key);

    $themes = list_themes();
    $theme_object = $themes[$theme_key];

    if ($settings['mission'] == '') {
      $settings['mission'] = variable_get('site_mission', '');
    }

    if (!$settings['toggle_mission']) {
      $settings['mission'] = '';
    }

    if ($settings['toggle_logo']) {
      if ($settings['default_logo']) {
        $settings['logo'] = base_path() . dirname($theme_object->filename) .'/logo.png';
      }
      elseif ($settings['logo_path']) {
        $settings['logo'] = base_path() . $settings['logo_path'];
      }
    }

    if ($settings['toggle_favicon']) {
      if ($settings['default_favicon']) {
        if (file_exists($favicon = dirname($theme_object->filename) .'/favicon.ico')) {
          $settings['favicon'] = base_path() . $favicon;
        }
        else {
          $settings['favicon'] = base_path() . 'misc/favicon.ico';
        }
      }
      elseif ($settings['favicon_path']) {
        $settings['favicon'] = base_path() . $settings['favicon_path'];
      }
    }
  }

  return isset($settings[$setting_name]) ? $settings[$setting_name] : NULL;
}

/**
 * Add a theme stylesheet to be included later. This is handled separately from
 * drupal_set_html_head() to enforce the correct CSS cascading order.
 */
function theme_add_style($path = '', $media = 'all') {
  static $styles = array();

  if ($path && !isset($styles["$media:$path"])) {
    $style = new stdClass();
    $style->path = base_path() . $path;
    $style->media = $media;
    $styles["$media:$path"] = $style;
  }
  return $styles;
}

/**
 * Return the HTML for a theme's stylesheets.
 */
function theme_get_styles() {
  $output = '';
  foreach (theme_add_style() as $style) {
    $output .= theme('stylesheet_import', $style->path, $style->media);
  }
  return $output;
}

/**
 * @defgroup themeable Themeable functions
 * @{
 * Functions that display HTML, and which can be customized by themes.
 *
 * All functions that produce HTML for display should be themeable. This means
 * that they should be named with the theme_ prefix, and invoked using theme()
 * rather than being called directly. This allows themes to override the display
 * of any Drupal object.
 *
 * The theme system is described and defined in theme.inc.
 */

/**
 * Format a dynamic text string for emphasised display in a placeholder.
 *
 * E.g. t('Added term %term', array('%term' => theme('placeholder', $term)))
 *
 * @param $text
 *   The text to format (plain-text).
 * @return
 *   The formatted text (html).
 */
function theme_placeholder($text) {
  return '<em>'. check_plain($text) .'</em>';
}

/**
 * Return an entire Drupal page displaying the supplied content.
 *
 * @param $content
 *   A string to display in the main content area of the page.
 * @return
 *   A string containing the entire HTML page.
 */
function theme_page($content) {
  $output = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  $output .= '<html xmlns="http://www.w3.org/1999/xhtml">';
  $output .= '<head>';
  $output .= ' <title>'. (drupal_get_title() ? strip_tags(drupal_get_title()) : variable_get('site_name', 'drupal')) .'</title>';
  $output .= drupal_get_html_head();
  $output .= theme_get_styles();

  $output .= ' </head>';
  $output .= ' <body style="background-color: #fff; color: #000;">';
  $output .= '<table border="0" cellspacing="4" cellpadding="4"><tr><td style="vertical-align: top; width: 170px;">';

  $output .= theme('blocks', 'all');
  $output .= '</td><td style="vertical-align: top;">';

  $output .= theme('breadcrumb', drupal_get_breadcrumb());
  $output .= '<h1>' . drupal_get_title() . '</h1>';

  if ($tabs = theme('menu_local_tasks')) {
   $output .= $tabs;
  }

  $output .= theme('help');

  $output .= theme('status_messages');

  $output .= "\n<!-- begin content -->\n";
  $output .= $content;
  $output .= "\n<!-- end content -->\n";

  $output .= '</td></tr></table>';
  $output .= theme_closure();
  $output .= '</body></html>';

  return $output;
}

function theme_maintenance_page($content, $messages = TRUE, $partial = FALSE) {
  drupal_set_header('Content-Type: text/html; charset=utf-8');
  theme('add_style', 'misc/maintenance.css');
  drupal_set_html_head('<link rel="shortcut icon" href="'. base_path() .'misc/favicon.ico" type="image/x-icon" />');

  $output = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  $output .= '<html xmlns="http://www.w3.org/1999/xhtml">';
  $output .= '<head>';
  $output .= ' <title>'. drupal_get_title() .'</title>';
  $output .= drupal_get_html_head();
  $output .= theme_get_styles();
  $output .= '</head>';
  $output .= '<body>';
  $output .= '<h1>' . drupal_get_title() . '</h1>';

  if ($messages) {
    $output .= theme('status_messages');
  }

  $output .= "\n<!-- begin content -->\n";
  $output .= $content;
  $output .= "\n<!-- end content -->\n";

  if (!$partial) {
    $output .= '</body></html>';
  }

  return $output;
}

/**
 * Returns themed set of status and/or error messages.  The messages are grouped
 * by type.
 *
 * @return
 *   A string containing the messages.
 */
function theme_status_messages() {
  if ($data = drupal_get_messages()) {
    $output = '';
    foreach ($data as $type => $messages) {
      $output .= "<div class=\"messages $type\">\n";
      if (count($messages) > 1) {
        $output .= " <ul>\n";
        foreach($messages as $message) {
          $output .= '  <li>'. $message ."</li>\n";
        }
        $output .= " </ul>\n";
      }
      else {
        $output .= $messages[0];
      }
      $output .= "</div>\n";
    }

    return $output;
  }
}

/**
 * Return a themed set of links.
 *
 * @param $links
 *   An array of links to be themed.
 * @param $delimiter
 *   A string used to separate the links.
 * @return
 *   A string containing the themed links.
 */
function theme_links($links, $delimiter = ' | ') {
  if (!is_array($links)) {
    return '';
  }
  return implode($delimiter, $links);
}

/**
 * Return a themed image.
 *
 * @param $path
 *   The path of the image file.
 * @param $alt
 *   The alternative text for text-based browsers.
 * @param $title
 *   The title text is displayed when the image is hovered in some popular browsers.
 * @param $attributes
 *   Associative array of attributes to be placed in the img tag.
 * @param $getsize
 *   If set to true, the image's dimension are fetched and added as width/height attributes.
 * @return
 *   A string containing the image tag.
 */
function theme_image($path, $alt = '', $title = '', $attributes = NULL, $getsize = TRUE) {
  if (!$getsize || (is_file($path) && (list($width, $height, $type, $image_attributes) = @getimagesize($path)))) {
    $attributes = drupal_attributes($attributes);
    return '<img src="'. check_url(base_path() . $path) .'" alt="'. check_plain($alt) .'" title="'. check_plain($title) .'" '. $image_attributes . $attributes .' />';
  }
}

/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return a string containing the breadcrumb output.
 */
function theme_breadcrumb($breadcrumb) {
  if (!empty($breadcrumb)) {
    return '<div class="breadcrumb">'. implode(' » ', $breadcrumb) .'</div>';
  }
}

/**
 * Return a themed help message.
 *
 * @return a string containing the helptext for the current page.
 */
function theme_help() {
  if ($help = menu_get_active_help()) {
    return '<div class="help">'. $help .'</div>';
  }
}

/**
 * Return a themed node.
 *
 * @param $node
 *   An object providing all relevant information for displaying a node:
 *   - $node->nid: The ID of the node.
 *   - $node->type: The content type (story, blog, forum...).
 *   - $node->title: The title of the node.
 *   - $node->created: The creation date, as a UNIX timestamp.
 *   - $node->teaser: A shortened version of the node body.
 *   - $node->body: The entire node contents.
 *   - $node->changed: The last modification date, as a UNIX timestamp.
 *   - $node->uid: The ID of the author.
 *   - $node->username: The username of the author.
 * @param $teaser
 *   Whether to display the teaser only, as on the main page.
 * @param $page
 *   Whether to display the node as a standalone page. If TRUE, do not display
 *   the title because it will be provided by the menu system.
 * @return
 *   A string containing the node output.
 */
function theme_node($node, $teaser = FALSE, $page = FALSE) {
  if (module_exist('taxonomy')) {
    $terms = taxonomy_link('taxonomy terms', $node);
  }

  if ($page == 0) {
    $output = t('%title by %name', array('%title' => '<h2 class="title">'. check_plain($node->title) .'</h2>', '%name' => theme('username', $node)));
  }
  else {
    $output = t('by %name', array('%name' => theme('username', $node)));
  }

  if (count($terms)) {
    $output .= ' <small>('. theme('links', $terms) .')</small><br />';
  }

  if ($teaser && $node->teaser) {
    $output .= $node->teaser;
  }
  else {
    $output .= $node->body;
  }

  if ($node->links) {
    $output .= '<div class="links">'. theme('links', $node->links) .'</div>';
  }

  return $output;
}

/**
 * Return a themed form element.
 *
 * @param $title the form element's title
 * @param $value the form element's data
 * @param $description the form element's description or explanation
 * @param $id the form element's ID used by the &lt;label&gt; tag
 * @param $required a boolean to indicate whether this is a required field or not
 * @param $error a string with an error message filed against this form element
 *
 * @return a string representing the form element
 */
function theme_form_element($title, $value, $description = NULL, $id = NULL, $required = FALSE, $error = FALSE) {

  $output  = '<div class="form-item">'."\n";
  $required = $required ? '<span class="form-required" title="'. t('This field is required.') .'">*</span>' : '';

  if ($title) {
    if ($id) {
      $output .= ' <label for="'. form_clean_id($id) .'">'. $title .":$required</label>\n";
    }
    else {
      $output .= ' <label>'. $title .":$required</label>\n";
    }
  }

  $output .= " $value\n";

  if ($description) {
    $output .= ' <div class="description">'. $description ."</div>\n";
  }

  $output .= "</div>\n";

  return $output;
}

/**
 * Return a themed submenu, typically displayed under the tabs.
 *
 * @param $links
 *   An array of links.
 */
function theme_submenu($links) {
  return '<div class="submenu">'. implode(' | ', $links) .'</div>';
}

/**
 * Return a themed table.
 *
 * @param $header
 *   An array containing the table headers. Each element of the array can be
 *   either a localized string or an associative array with the following keys:
 *   - "data": The localized title of the table column.
 *   - "field": The database field represented in the table column (required if
 *     user is to be able to sort on this column).
 *   - "sort": A default sort order for this column ("asc" or "desc").
 *   - Any HTML attributes, such as "colspan", to apply to the column header cell.
 * @param $rows
 *   An array of table rows. Every row is an array of cells, or an associative
 *   array with the following keys:
 *   - "data": an array of cells
 *   - Any HTML attributes, such as "class", to apply to the table row.
 *
 *   Each cell can be either a string or an associative array with the following keys:
 *   - "data": The string to display in the table cell.
 *   - Any HTML attributes, such as "colspan", to apply to the table cell.
 *
 *   Here's an example for $rows:
 *   @verbatim
 *   $rows = array(
 *     // Simple row
 *     array(
 *       'Cell 1', 'Cell 2', 'Cell 3'
 *     ),
 *     // Row with attributes on the row and some of its cells.
 *     array(
 *       'data' => array('Cell 1', array('data' => 'Cell 2', 'colspan' => 2)), 'class' => 'funky'
 *     )
 *   );
 *   @endverbatim
 *
 * @param $attributes
 *   An array of HTML attributes to apply to the table tag.
 * @param $caption
 *   A localized string to use for the <caption> tag.
 * @return
 *   An HTML string representing the table.
 */
function theme_table($header, $rows, $attributes = array(), $caption = NULL) {
  $output = '<table'. drupal_attributes($attributes) .">\n";

  if (isset($caption)) {
    $output .= '<caption>'. $caption ."</caption>\n";
  }

  // Format the table header:
  if (count($header)) {
    $ts = tablesort_init($header);
    $output .= ' <thead><tr>';
    foreach ($header as $cell) {
      $cell = tablesort_header($cell, $header, $ts);
      $output .= _theme_table_cell($cell, 1);
    }
    $output .= " </tr></thead>\n";
  }

  // Format the table rows:
  $output .= "<tbody>\n";
  if (count($rows)) {
    foreach ($rows as $number => $row) {
      $attributes = array();

      // Check if we're dealing with a simple or complex row
      if (isset($row['data'])) {
        foreach ($row as $key => $value) {
          if ($key == 'data') {
            $cells = $value;
          }
          else {
            $attributes[$key] = $value;
          }
        }
      }
      else {
        $cells = $row;
      }

      // Add odd/even class
      $class = ($number % 2 == 1) ? 'even': 'odd';
      if (isset($attributes['class'])) {
        $attributes['class'] .= ' '. $class;
      }
      else {
        $attributes['class'] = $class;
      }

      // Build row
      $output .= ' <tr'. drupal_attributes($attributes) .'>';
      $i = 0;
      foreach ($cells as $cell) {
        $cell = tablesort_cell($cell, $header, $ts, $i++);
        $output .= _theme_table_cell($cell, 0);
      }
      $output .= " </tr>\n";
    }
  }

  $output .= "</tbody></table>\n";
  return $output;
}

/**
 * Return a themed sort icon.
 *
 * @param $style
 *   Set to either asc or desc.  This sets which icon to show.
 * @return
 *   A themed sort icon.
 */
function theme_tablesort_indicator($style) {
  if ($style == "asc"){
    return theme('image', 'misc/arrow-asc.png', t('sort icon'), t('sort ascending'));
  }
  else {
    return theme('image', 'misc/arrow-desc.png', t('sort icon'), t('sort descending'));
  }
}

/**
 * Return a themed box.
 *
 * @param $title
 *   The subject of the box.
 * @param $content
 *   The content of the box.
 * @param $region
 *   The region in which the box is displayed.
 * @return
 *   A string containing the box output.
 */
function theme_box($title, $content, $region = 'main') {
  $output = '<h2 class="title">'. $title .'</h2><div>'. $content .'</div>';
  return $output;
}

/**
 * Return a themed block.
 *
 * You can style your blocks by defining .block (all blocks),
 * .block-<i>module</i> (all blocks of module <i>module</i>), and
 * \#block-<i>module</i>-<i>delta</i> (specific block of module <i>module</i>
 * with delta <i>delta</i>) in your theme's CSS.
 *
 * @param $block
 *   An object populated with fields from the "blocks" database table
 *   ($block->module, $block->delta ...) and fields returned by
 *   <i>module</i>_block('view') ($block->subject, $block->content, ...).
 * @return
 *   A string containing the block output.
 */
function theme_block($block) {
  $output  = "<div class=\"block block-$block->module\" id=\"block-$block->module-$block->delta\">\n";
  $output .= " <h2 class=\"title\">$block->subject</h2>\n";
  $output .= " <div class=\"content\">$block->content</div>\n";
  $output .= "</div>\n";
  return $output;
}

/**
 * Return a themed marker, useful for marking new or updated
 * content.
 *
 * @param $type
 *   Number representing the marker type to display
 * @see MARK_NEW, MARK_UPDATED, MARK_READ
 * @return
 *   A string containing the marker.
 */
function theme_mark($type = MARK_NEW) {
  global $user;
  if ($user->uid) {
    if ($type == MARK_NEW) {
      return ' <span class="marker">'. t('new') .'</span>';
    }
    else if ($type == MARK_UPDATED) {
      return ' <span class="marker">'. t('updated') .'</span>';
    }
  }
}

/**
 * Import a stylesheet using @import.
 *
 * @param $path
 *  The path to the stylesheet.
 *
 * @param $media
 *  The media type to specify for the stylesheet
 *
 * @return
 *  A string containing the HTML for the stylesheet import.
 */
function theme_stylesheet_import($path, $media = 'all') {
  return '<style type="text/css" media="'. $media .'">@import "'. $path .'";</style>';
}

/**
 * Return a themed list of items.
 *
 * @param $items
 *   An array of items to be displayed in the list.
 * @param $title
 *   The title of the list.
 * @param $type
 *   The type of list to return (e.g. "ul", "ol")
 * @return
 *   A string containing the list output.
 */
function theme_item_list($items = array(), $title = NULL, $type = 'ul') {
  $output = '<div class="item-list">';
  if (isset($title)) {
    $output .= '<h3>'. $title .'</h3>';
  }

  if (!empty($items)) {
    $output .= "<$type>";
    foreach ($items as $item) {
      $output .= '<li>'. $item .'</li>';
    }
    $output .= "</$type>";
  }
  $output .= '</div>';
  return $output;
}

/**
 * Returns code that emits the 'more help'-link.
 */
function theme_more_help_link($url) {
  return '<div class="more-help-link">' . t('[<a href="%link">more help...</a>]', array('%link' => check_url($url))) . '</div>';
}

/**
 * Return code that emits an XML icon.
 */
function theme_xml_icon($url) {
  if ($image = theme('image', 'misc/xml.png', t('XML feed'), t('XML feed'))) {
    return '<a href="'. check_url($url) .'" class="xml-icon">'. $image. '</a>';
  }
}

/**
 * Return code that emits an feed icon.
 */
function theme_feed_icon($url) {
  if ($image = theme('image', 'misc/feed.png', t('Syndicate content'), t('Syndicate content'))) {
    return '<a href="'. check_url($url) .'" class="feed-icon">'. $image. '</a>';
  }
}

/**
 * Execute hook_footer() which is run at the end of the page right before the
 * close of the body tag.
 *
 * @param $main (optional)
 *
 * @return
 *   A string containing the results of the hook_footer() calls.
 */
function theme_closure($main = 0) {
  $footer = module_invoke_all('footer', $main);
  return implode("\n", $footer);
}

/**
 * Return a set of blocks available for the current user.
 *
 * @param $region
 *   Which set of blocks to retrieve.
 * @return
 *   A string containing the themed blocks for this region.
 */
function theme_blocks($region) {
  $output = '';

  if ($list = module_invoke('block', 'list', $region)) {
    foreach ($list as $key => $block) {
      // $key == <i>module</i>_<i>delta</i>
      $output .= theme('block', $block);
    }
  }

  // Add any content assigned to this region through drupal_set_content() calls.
  $output .= drupal_get_content($region);

  return $output;
}

/**
 * Format a username.
 *
 * @param $object
 *   The user object to format, usually returned from user_load().
 * @return
 *   A string containing an HTML link to the user's page if the passed object
 *   suggests that this is a site user. Otherwise, only the username is returned.
 */
function theme_username($object) {

  if ($object->uid && $object->name) {
    // Shorten the name when it is too long or it will break many tables.
    if (drupal_strlen($object->name) > 20) {
      $name = drupal_substr($object->name, 0, 15) .'...';
    }
    else {
      $name = $object->name;
    }

    if (user_access('access user profiles')) {
      $output = l($name, 'user/'. $object->uid, array('title' => t('View user profile.')));
    }
    else {
      $output = check_plain($name);
    }
  }
  else if ($object->name) {
    // Sometimes modules display content composed by people who are
    // not registered members of the site (e.g. mailing list or news
    // aggregator modules). This clause enables modules to display
    // the true author of the content.
    if ($object->homepage) {
      $output = l($object->name, $object->homepage);
    }
    else {
      $output = check_plain($object->name);
    }

    $output .= ' ('. t('not verified') .')';
  }
  else {
    $output = variable_get('anonymous', 'Anonymous');
  }

  return $output;
}

function theme_progress_bar($percent, $message) {
  $output = '<div id="progress" class="progress">';
  $output .= '<div class="percentage">'. $percent .'%</div>';
  $output .= '<div class="status">'. $message .'</div>';
  $output .= '<div class="bar"><div class="filled" style="width: '. $percent .'%"></div></div>';
  $output .= '</div>';

  return $output;
}

/**
 * @} End of "defgroup themeable".
 */

function _theme_table_cell($cell, $header = 0) {
  $attributes = '';

  if (is_array($cell)) {
    $data = $cell['data'];
    foreach ($cell as $key => $value) {
      if ($key != 'data')  {
        $attributes .= " $key=\"$value\"";
      }
    }
  }
  else {
    $data = $cell;
  }

  if ($header) {
    $output = "<th$attributes>$data</th>";
  }
  else {
    $output = "<td$attributes>$data</td>";
  }

  return $output;
}

