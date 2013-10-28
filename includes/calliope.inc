<?php

/**
 * @file
 * Handles Calliope's Mongo interaction.
 */

/**
 * Creates MongoFSGrid.
 *
 * @return MongoFSGrid
 *   MongoFSGrid using supplied credentials
 */
function islandora_calliope_create_mongo_grid() {
  $dbhost = variable_get('austese_repository_mongo_host', "localhost");
  $dbport = variable_get('austese_repository_mongo_port', "27017");
  $dbname = variable_get('austese_repository_mongo_db', "test");
  try {
    $mongo = new Mongo("$dbhost:$dbport");
    $db = $mongo->selectDB($dbname);
    $grid = $db->getGridFS();
  }
  catch (Exception $e) {
    drupal_add_http_header('Status', '500 Internal server error');
    echo $e->getMessage();
    exit;
  }
  return $grid;
}

/**
 * Send documents to Calliope to create MVD.
 *
 * @param String $mvd_pid
 *   identifier for MVD
 * @param array $transcriptions
 *   array of transcription identifiers
 *
 * @return String
 *   Success/failure message
 */
function islandora_calliope_create_mvd($mvd_pid, $transcriptions = array()) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $doctitle = $mvd_pid;
  // Hard coding to our known working calliope.
  $base_url = "http://experimental.emic.dgicloud.com";
  $calliope_import = "$base_url/calliope/import/";
  $boundary = "-------------" . uniqid();
  $data = "";
  $id_json_array = "";
  foreach ($transcriptions as $transcription) {
    if ($transcription != "") {
      if ($id_json_array) {
        $id_json_array .= ",";
      }
      $object = islandora_object_load($transcription);
      $versionid = islandora_escape_pid_for_function($transcription);
      $id_json_array .= "{\"id\":\"$versionid\",\"name\":\"$versionid\"}";
      // Load contents for resource.
      $contents = $object['TRANSCRIPTION']->content;
      $contenttype = 'text/plain';
      $data .= "--" . $boundary . "\r\n";
      $data .= "Content-Disposition: form-data; name=\"uploadedfile[]\"; filename=\"" . $versionid . "\"\r\n";
      $data .= "Content-Type: " . $contenttype . "\r\n";
      $data .= "Content-Length: " . strlen($contents) . "\r\n";
      $data .= "Content-Transfer-Encoding: binary\r\n\r\n";
      $data .= $contents . "\r\n";
    }
  }
  $data .= "--" . $boundary . "\r\n";
  $data .= "Content-Disposition: form-data; name=\"DOC_ID\"\r\n\r\n" . $doctitle . "\r\n";
  $data .= "--" . $boundary . "--\n";

  // Post to calliope server import.
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, trim($calliope_import));
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
  curl_setopt($ch, CURLOPT_POST, 1);
  // Add data to curl request.
  curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
  curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-type: multipart/form-data; boundary=" . $boundary));
  // Get back status or response.
  $response = curl_exec($ch);
  $err = curl_errno($ch);
  $error = curl_error($ch);
  curl_close($ch);

  if ($err) {
    return t('Error: ') . $err . " " . $error . " " . $response;
  }
  else {
    $message = "";

    // All docdata has been retrieved at this point.
    $hriterror = preg_match('/rror/', $response);
    if ($hriterror == 0) {
      $hriterror = preg_match('/HritServer Error/', $response);
      if ($hriterror == 0) {
        // Store MVD details in db for now.
        // Parse existing mvd results to check if one exists with same name.
        // Get id and delete.
        $mvd_search_url = $base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/?searchField=name&q=" . $doctitle . "$";
        $mvd_search = file_get_contents($mvd_search_url);
        $mvd_search_json = json_decode($mvd_search, TRUE);
        $existing_mvd_count = $mvd_search_json['count'];
        if ($existing_mvd_count != 0) {
          $existing_mvd = $mvd_search_json['results'][0]['id'];
          $ch3 = curl_init();
          curl_setopt($ch3, CURLOPT_CUSTOMREQUEST, 'DELETE');
          curl_setopt($ch3, CURLOPT_URL, trim($base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/" . $existing_mvd));
          $db_response = curl_exec($ch3);
          $dberr = curl_errno($ch3);
          $dberror = curl_error($ch3);
          curl_close($ch3);
          if ($dberr) {
            return t('Error: ') . $dberr . " " . $dberror . $db_response;
          }
        }
        // Post new MVD details to db.
        $mvd_db_url = $base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/";
        $mvd_json = "{\"name\":\"$mvd_pid\",\"resources\":[$id_json_array]}";
        $ch2 = curl_init();
        curl_setopt($ch2, CURLOPT_URL, trim($mvd_db_url));
        curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch2, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch2, CURLOPT_POST, 1);
        curl_setopt($ch2, CURLOPT_POSTFIELDS, $mvd_json);
        curl_setopt($ch2, CURLOPT_HTTPHEADER, array("Content-type: application/json;"));

        $db_response = curl_exec($ch2);
        $dberr = curl_errno($ch2);
        $dberror = curl_error($ch2);
        curl_close($ch2);
        if ($dberr) {
          return t('Error: ') . $dberr . " " . $dberror . $db_response;
        }

        // Success message.
        $message = "<p>MVD created. You may now view <a href='/collationtools/apparatus#$mvd_pid'>table apparatus</a> or <a href='/collationtools/compare#$mvd_pid'>side-by-side comparison</a></p>";
      }
      else {
        $message = "<p>MVD was not created. Refer to the log above for the cause.";
      }
    }
    else {
      // Message is failure message.
      $message = "<p>There was an error creating the MVD. Refer to the log above for the cause. If the log does not indicate a reason, the operation may have failed as a result of versions being too large or too much variation between versions. Try splitting larger transcriptions into smaller sections.</p>";
    }
    return t("<p> $response </p>") . $message;
  }
}

/**
 * Adds Transcription record to Mongo.
 *
 * @param String $pid
 *   PID of Transcription Object
 * @param String $project
 *   Optional project associated with transcription
 */
function islandora_calliope_write_file($pid, $project = '') {
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
    $query = array('_resourceid' => $flat_pid);
    $record = $grid->findOne($query);
    if ($record) {
      $id = $record->file['_id'];
      $grid->remove($query);
    }
    $storedfile = $grid->storeFile($real_path, array(
      'metadata' => array(
        'filetype' => $transcription_ds->mimetype,
        'project' => $project))
    );
    $id = $storedfile->{'$id'};
    $grid->update(
        array('_id' => new MongoId($id)), array('$set' => array('_resourceid' => $flat_pid, 'filename' => $transcription_object->label)), array('safe' => TRUE));
  }
  catch (Exception $e) {
    drupal_add_http_header('Status', '500 Internal server error');
    echo $e->getMessage();
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
  $flat_pid = islandora_escape_pid_for_function($pid);
  try {
    $grid = islandora_calliope_create_mongo_grid();
    $query = array('_resourceid' => $flat_pid);
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
 * Creates MVD from Versionable Object.
 *
 * @param AbstractObject $critical_edition_container_object
 *   Object representing Critical Edition object
 *
 * @return String
 *   Success/failure message
 */
function islandora_critical_create_mvd($critical_edition_container_object) {
  module_load_include('inc', 'islandora_calliope', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $versionable_objects = array_keys(islandora_critical_edition_get_members($critical_edition_container_object->id, 'islandora:versionableObjectCModel'));
  $transcriptions = array();
  foreach ($versionable_objects as $versionable_object) {
    $transcriptions = array_merge($transcriptions, array_keys(islandora_critical_get_related_transcriptions($versionable_object)));
  }
  $success = islandora_calliope_create_mvd($critical_edition_container_object->id, $transcriptions);
  drupal_set_message($success);
  drupal_goto('islandora/object/' . $critical_edition_container_object->id);
}