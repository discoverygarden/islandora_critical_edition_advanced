<?php

/**
 * @file
 * Handles Calliope's Mongo interaction.
 */

/**
 * Creates configured MongoDB object.
 *
 * @return MongoDB
 *   MongoDB built with Islandora configuration
 */
function islandora_calliope_create_mongo_db() {
  $dbhost = variable_get('islandora_critical_edition_mongo_host', "localhost");
  $dbport = variable_get('islandora_critical_edition_mongo_port', "27017");
  $dbname = variable_get('islandora_critical_edition_mongo_db', "test");
  try {
    $mongo = new MongoClient("$dbhost:$dbport");
    $db = $mongo->selectDB($dbname);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
  return $db;
}

/**
 * Creates MongoFSGrid.
 *
 * @return MongoFSGrid
 *   MongoFSGrid using supplied credentials
 */
function islandora_calliope_create_mongo_grid() {
  try {

    $db = islandora_calliope_create_mongo_db();
    $grid = $db->getGridFS();
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
  return $grid;
}

/**
 * Adds Transcription record to Mongo.
 *
 * @param string $pid
 *   PID of Transcription Object
 * @param string $critical_edition
 *   Optional project associated with transcription
 */
function islandora_calliope_write_file($pid, $critical_edition = '') {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $transcription_object = islandora_object_load($pid);
  $transcription_ds = $transcription_object['TRANSCRIPTION'];
  if (!$transcription_ds) {
    drupal_set_message(t("@pid has no transcription datastream", array('@pid' => $pid)));
    return;
  }
  $flat_pid = islandora_escape_pid_for_function($pid);
  $filename = file_create_filename($flat_pid . "_temp_for_mongo", 'temporary://');
  $real_path = drupal_realpath($filename);
  $contents = $transcription_ds->content;
  file_put_contents($filename, $contents);
  try {
    $grid = islandora_calliope_create_mongo_grid();
    $query = array('_resourceid' => $pid);
    $record = $grid->findOne($query);
    if ($record) {
      $id = $record->file['_id'];
      $grid->remove($query);
    }
    $record2 = $grid->findOne($query);
    $metadata['filetype'] = $transcription_ds->mimetype;
    if ($critical_edition) {
      $metadata['project'] = $critical_edition;
    }

    $storedfile = $grid->storeFile($real_path, array('metadata' => $metadata));
    $id = $storedfile->{'$id'};
    $grid->update(
        array('_id' => new MongoId($id)), array('$set' => array('_resourceid' => $pid, 'filename' => $transcription_object->label)), array('w' => 1));
    unlink($filename);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
}

/**
 * Deletes Mongo record assoicated with pid.
 *
 * @param string $pid
 *   PID of associated object
 */
function islandora_calliope_delete_file($pid) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  try {
    $grid = islandora_calliope_create_mongo_grid();
    $query = array('_resourceid' => $pid);
    $record = $grid->findOne($query);
    $id = $record->file['_id'];
    if ($id) {
      $grid->remove($query);
    }
  }
  catch (Exception $e) {
    drupal_add_http_header('Status', '500 Internal server error');
    echo $e->getMessage();
    exit;
  }
}

/**
 * Deletes Mongo MVD record.
 *
 * @param string $id
 *   Mongo ID of MVD
 */
function islandora_calliope_delete_mvd($id) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  try {
    $db = islandora_calliope_create_mongo_db();
    $collection = $db->selectCollection('mvds');
    $collection->remove(array('_id' => $id));
  }
  catch (Exception $e) {
    drupal_add_http_header('Status', '500 Internal server error');
    echo $e->getMessage();
    exit;
  }
}

/**
 * Send selected documents to calliope to create MVD.
 *
 * @global type $base_url
 *
 * @param string $critical_edition
 *   MVD's Project
 * @param string $mvd_title
 *   Title for MVD
 * @param array $resource_ids
 *   Array of transcriptions to build MVD
 * @param string $filter
 *   Optional Filter
 *
 * @return string
 *   Status message
 */
function islandora_calliope_create_mvd($critical_edition, $mvd_title, $mvd_description, $resource_ids = NULL, $filter = "Empty") {
  global $base_url;
  $mvd_url = "$base_url/calliope";
  $calliope_import_url = "$mvd_url/import/";
  $verb = t('created');

  // Remove spaces from doc path.
  $docpath = str_replace(' ', '_', "$mvd_title");
  $origdocpath = $docpath;
  if ($resource_ids == NULL) {
    return t('Identifiers of resources to add to MVD not specified');
  }
  $boundary = "-------------" . uniqid();
  $data = "";
  $id_array = array();
  foreach ($resource_ids as $resource_id) {
    if ($resource_id != "") {
      $file = islandora_calliope_get_file($resource_id);
      if (!$file) {
        islandora_calliope_write_file($resource_id);
        $file = islandora_calliope_get_file($resource_id);
        if (!$file) {
          drupal_set_message(t("Resource_id could not be used to make file"));
          return;
        }
      }
      $contents = $file->getBytes();
      $filename = $file->file['filename'];
      $file_label = str_replace('.', '_', pathinfo($filename, PATHINFO_FILENAME));
      $id_array[] = array('id' => $resource_id, 'name' => $file_label);

      $data .= "--" . $boundary . "\r\n";
      $data .= "Content-Disposition: form-data; name=\"uploadedfile[]\"; filename=\"" . $file_label . "\"\r\n";
      $data .= "Content-Type: " . 'text/xml' . "\r\n";
      $data .= "Content-Length: " . strlen($contents) . "\r\n";
      $data .= "Content-Transfer-Encoding: binary\r\n\r\n";
      $data .= $contents . "\r\n";
    }
  }
  $data .= "--" . $boundary . "\r\n";
  $data .= "Content-Disposition: form-data; name=\"DOC_ID\"\r\n\r\n" . $docpath . "\r\n";
  $data .= "--" . $boundary . "--\n";

  $data .= "--" . $boundary . "\r\n";
  $data .= "Content-Disposition: form-data; name=\"FILTER\"\r\n\r\n" . $filter . "\r\n";
  $data .= "--" . $boundary . "--\n";

  $ch = curl_init();
  // Post to Calliope server import.
  curl_setopt($ch, CURLOPT_URL, trim($calliope_import_url));
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt($ch, CURLOPT_POST, 1);
  curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
  curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-type: multipart/form-data; boundary=" . $boundary));
  // Get back status or response.
  $response = curl_exec($ch);
  $err = curl_errno($ch);
  $error = curl_error($ch);
  curl_close($ch);

  if ($err) {
    return t("Error @err @error @response", array(
      '@err' => $err,
      '@error' => $error,
      '@response' => $response,
    ));
  }
  else {
    // Unfortunately HRIT Server does not use HTTP status to indicate errors.
    // Look for 'rror' in response instead.
    $hriterror = preg_match('/rror/', $response);
    if ($hriterror == 0) {
      // Try to fetch json for created mvd.
      $hriterror = preg_match('/HritServer Error/', $response);
      if ($hriterror == 0) {
        $query = array(
          'metadata.critical_edition' => $critical_edition,
          'metadata.name' => $docpath,
        );
        $mvds = islandora_calliope_find_mvd($query);
        if (!empty($mvds)) {
          foreach ($mvds as $mvd) {
            islandora_calliope_delete_mvd($mvd['_id']);
          }
          $verb = t('updated');
        }
        // Post new MVD details to db.
        $newmvd = array(
          'name' => $origdocpath,
          'resources' => $id_array,
          'filter' => $filter,
          'critical_edition' => $critical_edition,
          'description' => $mvd_description,
        );
        $mvd_success = islandora_calliope_write_mvd($newmvd);
      }
    }
    if ($mvd_success) {
      drupal_set_message(t("Collation successfully @verb.", array('@verb' => $verb)));
    }
  }
}

/**
 * Returns Mongo file associated with pid.
 *
 * @param string $pid
 *   PID of Fedora object with associated Mongo file
 *
 * @return MongoGridFSFile
 *   Associated file
 */
function islandora_calliope_get_file($pid) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $id = islandora_escape_pid_for_function($pid);
  $grid = islandora_calliope_create_mongo_grid();
  if (!$grid) {
    return;
  }
  $query = array('_resourceid' => $pid);
  $file = $grid->findOne($query);
  if ($file == NULL) {
    drupal_set_message(t('The requested resource does not exist'));
    return;
  }
  if (array_key_exists('_deleted', $file->file)) {
    drupal_set_message(t('The requested resource has been deleted'));
    return;
  }
  return $file;
}

/**
 * Pulls all MVD metadata from Mongo.
 *
 * @param AbstractObject $critical_edition_object
 *   Object representing Critical Edition
 *
 * @return array
 *   All values associated with all MVD's attached to this Critical Edition
 */

/**
 * Pulls all MVD metadata from Mongo.
 *
 * @param AbstractObject $critical_edition_object
 *   Object representing Critical Edition
 * @param string $sort
 *   Sort type
 * @param int $limit
 *   Size of resultset
 * @param int $offset
 *   Offset for db call
 *
 * @return array
 *   Associative array of MVD data
 */
function islandora_calliope_find_mvds($critical_edition_object, $sort = "asc", $limit = NULL, $offset = 0) {
  $id = $critical_edition_object->id;
  $query = array('metadata.critical_edition' => $critical_edition_object->id);
  $sort = strtolower($sort);
  $sort_val = ($sort == 'asc') ? 1 : -1;
  try {
    $db = islandora_calliope_create_mongo_db();
    $collection = $db->selectCollection('mvds');
    $mvd_cursor = $collection->find($query);
    $count = $mvd_cursor->count();
    $mvd_cursor->sort(array('metadata.name' => $sort_val));
    $mvd_data = array();
    foreach ($mvd_cursor as $mvd) {
      if (is_array($mvd['metadata']['resources'])) {
        $transcriptions = array();
        foreach ($mvd['metadata']['resources'] as $transcription) {
          $transcriptions[$transcription['id']] = $transcription['name'];
        }
        $mvd_data[$mvd['metadata']['name']]['name'] = $mvd['metadata']['name'];
        $mvd_data[$mvd['metadata']['name']]['filter'] = $mvd['metadata']['filter'];
        $mvd_data[$mvd['metadata']['name']]['transcriptions'] = $transcriptions;
        if (isset($mvd['metadata']['description'])) {
          $mvd_data[$mvd['metadata']['name']]['description'] = $mvd['metadata']['description'];
        }
        else {
          $mvd_data[$mvd['metadata']['name']]['description'] = t("No description");
        }
      }
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
  return $mvd_data;
}

/**
 * Pulls all MVD metadata from Mongo.
 *
 * @param array $query
 *   Mongo query
 *
 * @return array
 *   All values associated with all MVD's attached to this Critical Edition
 */
function islandora_calliope_find_mvd($query) {
  try {
    $db = islandora_calliope_create_mongo_db();
    $collection = $db->selectCollection('mvds');
    $mvd_cursor = $collection->find($query);
    $mvd_data = array();
    foreach ($mvd_cursor as $mvd) {
      $mvd_data[] = $mvd;
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
  return $mvd_data;
}

/**
 * Writes new MVD to Mongo.
 *
 * @param array $newmvd
 *   All paramaters to build new MVD
 *
 * @return bool
 *   Success indicator
 */
function islandora_calliope_write_mvd($newmvd) {
  $success = FALSE;
  $newobj = array('_revisions' => array($newmvd), 'metadata' => $newmvd);
  $db = islandora_calliope_create_mongo_db();
  $coll = $db->selectCollection('mvds');
  $inserted = $coll->insert($newobj, array('w' => 1));
  if ($inserted) {
    $success = TRUE;
  }
  return $inserted;
}

/**
 * Brings back basic mvd information filtered by Critical Edition.
 *
 * @param string $critical_edition_pid
 *   PID of critical edition
 *
 * @return MongoCursor
 *   Query results
 */
function islandora_calliope_compare_callback($critical_edition_pid) {
  $query = array('metadata.critical_edition' => $critical_edition_pid);
  $db = islandora_calliope_create_mongo_db();
  $collection = $db->selectCollection('mvds');
  return $collection->find($query);
}
