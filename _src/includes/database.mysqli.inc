<?php
// $Id: database.mysqli.inc,v 1.16 2006/02/20 16:46:59 dries Exp $

/**
 * @file
 * Database interface code for MySQL database servers using the mysqli client libraries. mysqli is included in PHP 5 by default and allows developers to use the advanced features of MySQL 4.1.x, 5.0.x and beyond.
 */

/* Maintainers of this file should consult
 * http://www.php.net/manual/en/ref.mysqli.php
 */

/**
 * @ingroup database
 * @{
 */

/**
 * Initialise a database connection.
 *
 * Note that mysqli does not support persistent connections.
 */
function db_connect($url) {
  // Check if MySQLi support is present in PHP
  if (!function_exists('mysqli_init') && !extension_loaded('mysqli')) {
    drupal_maintenance_theme();
    drupal_set_title('PHP MySQLi support not enabled');
    print theme('maintenance_page', '<p>We were unable to use the MySQLi database because the MySQLi extension for PHP is not installed. Check your <code>PHP.ini</code> to see how you can enable it.</p>
<p>For more help, see the <a href="http://drupal.org/node/258">Installation and upgrading handbook</a>. If you are unsure what these terms mean you should probably contact your hosting provider.</p>');
    exit;
  }

  $url = parse_url($url);

  // Decode url-encoded information in the db connection string
  $url['user'] = urldecode($url['user']);
  $url['pass'] = urldecode($url['pass']);
  $url['host'] = urldecode($url['host']);
  $url['path'] = urldecode($url['path']);

  $connection = mysqli_init();
  @mysqli_real_connect($connection, $url['host'], $url['user'], $url['pass'], substr($url['path'], 1), $url['port'], NULL, MYSQLI_CLIENT_FOUND_ROWS);

  // Find all database connection errors and error 1045 for access denied for user account
  if (mysqli_connect_errno() >= 2000 || mysqli_connect_errno() == 1045) {
    drupal_maintenance_theme();
    drupal_set_title('Unable to connect to database server');
    print theme('maintenance_page', '<p>This either means that the username and password information in your <code>settings.php</code> file is incorrect or we can\'t contact the MySQL database server through the mysqli libraries. This could also mean your hosting provider\'s database server is down.</p>
<p>The MySQL error was: '. theme('placeholder', mysqli_error($connection)) .'.</p>
<p>Currently, the username is '. theme('placeholder', $url['user']) .' and the database server is '. theme('placeholder', $url['host']) .'.</p>
<ul>
  <li>Are you sure you have the correct username and password?</li>
  <li>Are you sure that you have typed the correct hostname?</li>
  <li>Are you sure that the database server is running?</li>
  <li>Are you sure that the mysqli libraries are compiled in your PHP installation? Try using the mysql library instead by editing your <code>settings.php</code> configuration file in Drupal.</li>
</ul>
<p>For more help, see the <a href="http://drupal.org/node/258">Installation and upgrading handbook</a>. If you are unsure what these terms mean you should probably contact your hosting provider.</p>');
    exit;
  }
  else if (mysqli_connect_errno() > 0) {
    drupal_maintenance_theme();
    drupal_set_title('Unable to select database');
    print theme('maintenance_page', '<p>We were able to connect to the MySQL database server (which means your username and password are okay) but not able to select the database.</p>
<p>The MySQL error was: '. theme('placeholder', mysqli_error($connection)) .'.</p>
<p>Currently, the database is '. theme('placeholder', substr($url['path'], 1)) .'. The username is '. theme('placeholder', $url['user']) .' and the database server is '. theme('placeholder', $url['host']) .'.</p>
<ul>
  <li>Are you sure you have the correct database name?</li>
  <li>Are you sure the database exists?</li>
  <li>Are you sure the username has permission to access the database?</li>
</ul>
<p>For more help, see the <a href="http://drupal.org/node/258">Installation and upgrading handbook</a>. If you are unsure what these terms mean you should probably contact your hosting provider.</p>');
    exit;
  }

  /* Force UTF-8 */
  mysqli_query($connection, 'SET NAMES "utf8"');

  /**
   * from: http://bugs.php.net/bug.php?id=33772
   * Write and Close handlers are called after destructing objects since PHP
   * 5.0.5. Thus destructors can use sessions but session handler can't use
   * objects. In prior versions, they were called in the opposite order. It
   * is possible to call session_write_close() from the destructor to solve
   * this chicken and egg problem.
   */
  register_shutdown_function('session_write_close');

  return $connection;
}

/**
 * Helper function for db_query().
 */
function _db_query($query, $debug = 0) {
  global $active_db, $queries;

  if (variable_get('dev_query', 0)) {
    list($usec, $sec) = explode(' ', microtime());
    $timer = (float)$usec + (float)$sec;
  }

  $result = mysqli_query($active_db, $query);

  if (variable_get('dev_query', 0)) {
    $bt = debug_backtrace();
    $query = $bt[2]['function'] . "\n" . $query;
    list($usec, $sec) = explode(' ', microtime());
    $stop = (float)$usec + (float)$sec;
    $diff = $stop - $timer;
    $queries[] = array($query, $diff);
  }

  if ($debug) {
    print '<p>query: '. $query .'<br />error:'. mysqli_error($active_db) .'</p>';
  }

  if (!mysqli_errno($active_db)) {
    return $result;
  }
  else {
    trigger_error(check_plain(mysqli_error($active_db) ."\nquery: ". $query), E_USER_WARNING);
    return FALSE;
  }
}

/**
 * Fetch one result row from the previous query as an object.
 *
 * @param $result
 *   A database query result resource, as returned from db_query().
 * @return
 *   An object representing the next row of the result. The attributes of this
 *   object are the table fields selected by the query.
 */
function db_fetch_object($result) {
  if ($result) {
    return mysqli_fetch_object($result);
  }
}

/**
 * Fetch one result row from the previous query as an array.
 *
 * @param $result
 *   A database query result resource, as returned from db_query().
 * @return
 *   An associative array representing the next row of the result. The keys of
 *   this object are the names of the table fields selected by the query, and
 *   the values are the field values for this result row.
 */
function db_fetch_array($result) {
  if ($result) {
    return mysqli_fetch_array($result, MYSQLI_ASSOC);
  }
}

/**
 * Determine how many result rows were found by the preceding query.
 *
 * @param $result
 *   A database query result resource, as returned from db_query().
 * @return
 *   The number of result rows.
 */
function db_num_rows($result) {
  if ($result) {
    return mysqli_num_rows($result);
  }
}

/**
* Return an individual result field from the previous query.
*
* Only use this function if exactly one field is being selected; otherwise,
* use db_fetch_object() or db_fetch_array().
*
* @param $result
*   A database query result resource, as returned from db_query().
* @param $row
*   The index of the row whose result is needed.
* @return
*   The resulting field.
*/
function db_result($result, $row = 0) {
  if ($result && mysqli_num_rows($result) > $row) {
    $array = mysqli_fetch_array($result, MYSQLI_NUM);
    return $array[0];
  }
}

/**
 * Determine whether the previous query caused an error.
 */
function db_error() {
  global $active_db;
  return mysqli_errno($active_db);
}

/**
 * Return a new unique ID in the given sequence.
 *
 * For compatibility reasons, Drupal does not use auto-numbered fields in its
 * database tables. Instead, this function is used to return a new unique ID
 * of the type requested. If necessary, a new sequence with the given name
 * will be created.
 */
function db_next_id($name) {
  $name = db_prefix_tables($name);
  db_query('LOCK TABLES {sequences} WRITE');
  $id = db_result(db_query("SELECT id FROM {sequences} WHERE name = '%s'", $name)) + 1;
  db_query("REPLACE INTO {sequences} VALUES ('%s', %d)", $name, $id);
  db_query('UNLOCK TABLES');

  return $id;
}

/**
 * Determine the number of rows changed by the preceding query.
 */
function db_affected_rows() {
  global $active_db; /* mysqli connection resource */
  return mysqli_affected_rows($active_db);
}

/**
 * Runs a limited-range query in the active database.
 *
 * Use this as a substitute for db_query() when a subset of the query is to be
 * returned.
 * User-supplied arguments to the query should be passed in as separate parameters
 * so that they can be properly escaped to avoid SQL injection attacks.
 *
 * Note that if you need to know how many results were returned, you should do
 * a SELECT COUNT(*) on the temporary table afterwards. db_num_rows() and
 * db_affected_rows() do not give consistent result across different database
 * types in this case.
 *
 * @param $query
 *   A string containing an SQL query.
 * @param ...
 *   A variable number of arguments which are substituted into the query
 *   using printf() syntax. The query arguments can be enclosed in one
 *   array instead.
 *   Valid %-modifiers are: %s, %d, %f, %b (binary data, do not enclose
 *   in '') and %%.
 *
 *   NOTE: using this syntax will cast NULL and FALSE values to decimal 0,
 *   and TRUE values to decimal 1.
 *
 * @param $from
 *   The first result row to return.
 * @param $count
 *   The maximum number of result rows to return.
 * @return
 *   A database query result resource, or FALSE if the query was not executed
 *   correctly.
 */
function db_query_range($query) {
  $args = func_get_args();
  $count = array_pop($args);
  $from = array_pop($args);
  array_shift($args);

  $query = db_prefix_tables($query);
  if (isset($args[0]) and is_array($args[0])) { // 'All arguments in one array' syntax
    $args = $args[0];
  }
  _db_query_callback($args, TRUE);
  $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
  $query .= ' LIMIT '. $from .', '. $count;
  return _db_query($query);
}

/**
 * Runs a SELECT query and stores its results in a temporary table.
 *
 * Use this as a substitute for db_query() when the results need to stored
 * in a temporary table. Temporary tables exist for the duration of the page
 * request.
 * User-supplied arguments to the query should be passed in as separate parameters
 * so that they can be properly escaped to avoid SQL injection attacks.
 *
 * Note that if you need to know how many results were returned, you should do
 * a SELECT COUNT(*) on the temporary table afterwards. db_num_rows() and
 * db_affected_rows() do not give consistent result across different database
 * types in this case.
 *
 * @param $query
 *   A string containing a normal SELECT SQL query.
 * @param ...
 *   A variable number of arguments which are substituted into the query
 *   using printf() syntax. The query arguments can be enclosed in one
 *   array instead.
 *   Valid %-modifiers are: %s, %d, %f, %b (binary data, do not enclose
 *   in '') and %%.
 *
 *   NOTE: using this syntax will cast NULL and FALSE values to decimal 0,
 *   and TRUE values to decimal 1.
 *
 * @param $table
 *   The name of the temporary table to select into. This name will not be
 *   prefixed as there is no risk of collision.
 * @return
 *   A database query result resource, or FALSE if the query was not executed
 *   correctly.
 */
function db_query_temporary($query) {
  $args = func_get_args();
  $tablename = array_pop($args);
  array_shift($args);

  $query = preg_replace('/^SELECT/i', 'CREATE TEMPORARY TABLE '. $tablename .' SELECT', db_prefix_tables($query));
  if (isset($args[0]) and is_array($args[0])) { // 'All arguments in one array' syntax
    $args = $args[0];
  }
  _db_query_callback($args, TRUE);
  $query = preg_replace_callback(DB_QUERY_REGEXP, '_db_query_callback', $query);
  return _db_query($query);
}

/**
 * Returns a properly formatted Binary Large Object value.
 *
 * @param $data
 *   Data to encode.
 * @return
 *  Encoded data.
 */
function db_encode_blob($data) {
  global $active_db;
  return "'" . mysqli_real_escape_string($active_db, $data) . "'";
}

/**
 * Returns text from a Binary Large OBject value.
 *
 * @param $data
 *   Data to decode.
 * @return
 *  Decoded data.
 */
function db_decode_blob($data) {
  return $data;
}

/**
 * Prepare user input for use in a database query, preventing SQL injection attacks.
 */
function db_escape_string($text) {
  global $active_db;
  return mysqli_real_escape_string($active_db, $text);
}

/**
 * Lock a table.
 */
function db_lock_table($table) {
  db_query('LOCK TABLES {%s} WRITE', $table);
}

/**
 * Unlock all locked tables.
 */
function db_unlock_tables() {
  db_query('UNLOCK TABLES');
}

/**
 * @} End of "ingroup database".
 */

