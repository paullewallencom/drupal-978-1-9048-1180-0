<?php

function flexinode_field_mp3_name($field) {
	return t('MP3');
}

function flexinode_field_mp3_form($field, $node) {
	$fieldname = 'flexinode_'. $field->field_id;
	$output = '';
	if ($node->$fieldname) {
		$output .= form_hidden($fieldname .'_old', serialize($node->$fieldname));
	}

	$output .= form_file(t($field->label), $fieldname, 40, ($node->$fieldname ? t('"%filename" has been uploaded. If you upload another file, the current file will be replaced.', array('%filename' => $node->$fieldname->filename)) : '') .' '. t($field->description), $field->required);

	return $output;
}

function flexinode_field_mp3_db_select($field) {
	$fieldname = 'flexinode_'. $field->field_id;
	return $fieldname .'.serialized_data AS '. $fieldname;
}

function flexinode_field_mp3_db_sort_column($field) {
	return 'flexinode_'. $field->field_id .'.textual_data';
}

function flexinode_field_mp3_insert($field, $node) {
	$fieldname = 'flexinode_'. $field->field_id;
	$node->$fieldname = file_save_upload($node->$fieldname, $node->$fieldname->filename);

	// get mp3 path for m3u
	$basename = basename($node->$fieldname->filepath, '.mp3');

	// put uploaded mp3 in files directory
	file_move($node->$fieldname->filepath, file_create_path());

	// make the download url for the mp3 (this won't survive a domain name change)
	$downloadPathMp3 = file_create_url($node->$fieldname->filepath);

	// make a tmp file with the m3u text
	$downloadPathMp3 = implode('%20', explode(' ', $downloadPathMp3));
	$tmpM3uPath = file_save_data($downloadPathMp3, dirname($node->$fieldname->filepath));

	// build m3u path
	$m3uPath = dirname($tmpM3uPath).'/'.$basename.'.m3u';

	// delete file of that name in case it is already there (node is being updated)
	file_delete($m3uPath);

	// rename the m3u file
	rename($tmpM3uPath, $m3uPath);

	// add m3u info to $node so it can get serialized
	$node->$fieldname->m3upath = $m3uPath;

	$serialized = is_object($node->$fieldname) ? serialize($node->$fieldname) : '';
	db_query("INSERT INTO {flexinode_data} (nid, field_id, textual_data, serialized_data) VALUES (%d, %d, '%s', '%s')", $node->nid, $field->field_id, $node->$fieldname->filename, $serialized);
}

function flexinode_field_mp3_delete($field, $node, $unconditional = 0) {
	$fieldname = 'flexinode_'. $field->field_id;
	$result = db_fetch_object(db_query('SELECT serialized_data FROM {flexinode_data} WHERE nid = %d AND field_id = %d', $node->nid, $field->field_id));
	$file = unserialize($result->serialized_data);
	if ($unconditional || $node->$fieldname != $file) {
		file_delete($file->filepath);
		file_delete($file->m3upath);
	}
}


function flexinode_field_mp3_validate($field, $node) {

	// get the library for reading tag information
	require_once('getid3/getid3.php');

	$fieldname = 'flexinode_'. $field->field_id;

	if ($_FILES["edit"]["name"][$fieldname]) {

		switch ($_FILES['edit']['error'][$fieldname]) { // Note: error code is returned only since PHP 4.2.0

		case 1:  // The uploaded file exceeds the upload_max_filesize directive in php.ini.
		$errortxt = t('The file you are trying to upload is too big.');
		watchdog('error', 'uploaded file exceeds the upload_max_filesize directive in php.ini: ' . $upload_name);
		break;

		case 2:  // The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the html form.
		$errortxt = t('The file you are trying to upload is too big.');
		watchdog('error', 'uploaded file exceeds the MAX_FILE_SIZE directive: ' . $upload_name);
		break;

		case 3:  // The uploaded file was only partially uploaded.
		$errortxt = t("The file was only partially uploaded. Please try again.");
		break;
		}

		if ($errortxt) {
			drupal_set_message($errortxt, 'error');
		}
	}
	if(!$errortxt){
		if ($file = file_save_upload($fieldname)) {
			// restrict types of files allowed here
			if($file->filemime != 'audio/mpeg' && 
				$file->filemime != 'audio/mp3' &&
				$file->filemime != 'audio/x-mp3'){
				return array('error' => 'File must be an mp3 file. ');
			}

			// get tag information here
			$getID3 = new getID3;
			$musicinfo = $getID3->analyze($file->filepath);
			if($musicinfo['tags'] and $musicinfo['tags']['id3v2']){
				foreach($musicinfo['tags']['id3v2'] as $key => $val){
					if($file->$key == NULL){
						$file->$key = $val[0];
					}
				}
			}
			if($musicinfo['fileformat']){
				$file->fileformat = $musicinfo['fileformat'];
			}
			if($musicinfo['bitrate']){
				$file->bitrate = $musicinfo['bitrate'];
			}
			if($musicinfo['playtime_string']){
				$file->playtime_string = $musicinfo['playtime_string'];
			}
			return array('value' => $file);
		}
	}
	if (empty($node->$fieldname)) {
		return array('value' => unserialize($node->{$fieldname .'_old'}));
	}

}

function flexinode_field_mp3_format($field, $node, $brief = 0) {
	$fieldname = 'flexinode_'. $field->field_id;
	$file = is_object($node->$fieldname) ? $node->$fieldname : unserialize($node->$fieldname);

	if ($file) {
		$result = '<a href="'. file_create_url($file->filepath) .'">'. check_plain($file->filename) .'</a> ('. format_size($file->filesize) .')';
		$result .= '<br><a href="'. file_create_url($file->m3upath) .'">'. check_plain($file->filename) .'</a> (streaming)';
		return $result;
	}
}


function flexinode_field_mp3_load($field, $node) {
	$fieldname = 'flexinode_'. $field->field_id;
	return unserialize($node->$fieldname);
}

function flexinode_field_mp3_rss($field, $node) {
  $f = 'flexinode_'. $field->field_id;
  $file = $node->$f;
  return array('key' => 'enclosure',
               'attributes' => array('url' => file_create_url($file->filepath),
               'length' => $file->filesize,
               'type' => $file->filemime));
}

/**
* @addtogroup themeable
* @{
*/

/**
* Format an MP3 download for display in a node.
*
* @param field_id
*   Which field is being displayed (useful when overriding this function
*   if you want to style one particular field differently).
* @param label
*   The label for the field as displayed on the node form.
* @param file
*   The file that the user has uploaded. This is an object as provided
*   by file.inc.
* @param formatted_value
*   A download link to the file.
*/
function theme_flexinode_mp3($field_id, $label, $file, $formatted_value) {
	$mp3fields = array("title","artist","album","track","genre","bitrate","playtime_string");

	$output = theme('form_element', $label, $formatted_value);

	$output = '<div class="flexinode-mp3-'. $field_id .'">'. $output .'</div>';

	foreach($mp3fields as $key){
		$keyvalue = theme('form_element', $key, $file->$key);
		$output .= '<div class="flexinode-mp3-'. $field_id .'-'.$key.'">'. $keyvalue .'</div>';
	}
	return $output;
}

/** @} End of addtogroup themeable */

?>