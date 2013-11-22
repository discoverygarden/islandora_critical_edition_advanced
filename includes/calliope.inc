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
  $dbhost = variable_get('islandora_critical_edition_mongo_host', "localhost");
  $dbport = variable_get('islandora_critical_edition_mongo_port', "27017");
  $dbname = variable_get('islandora_critical_edition_mongo_db', "test");
  try {
    $mongo = new Mongo("$dbhost:$dbport");
    $db = $mongo->selectDB($dbname);
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
    unlink($filename);
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
 * Send selected documents to calliope to create MVD.
 *
 * @global type $base_url
 *
 * @param String $project
 *   MVD's Project
 * @param String $mvd_title
 *   Title for MVD
 * @param array $resource_ids
 *   Array of transcriptions to build MVD
 * @param String $filter
 *   Optional Filter
 *
 * @return String
 *   Status message
 */
function islandora_calliope_create_mvd($project, $mvd_title, $resource_ids = NULL, $filter = "Empty") {
  global $base_url;
  $mvd_url = "$base_url/calliope/";
  $calliope_import_url = "$mvd_url/import/";

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
      $resource_id = islandora_escape_pid_for_function($resource_id);
      $file = islandora_calliope_get_file($resource_id);
      if (!$file) {
        islandora_calliope_write_file($resource_id);
        $file = islandora_calliope_get_file($resource_id);
        if (!$file) {
          drupal_set_message("Resource_id could not be used to make file");
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
    $message = "";

    // Unfortunately HRIT Server does not use HTTP status to indicate errors.
    // Look for 'rror' in response instead.
    $hriterror = preg_match('/rror/', $response);
    if ($hriterror == 0) {
      // Try to fetch json for created mvd.
      $hriterror = preg_match('/HritServer Error/', $response);
      if ($hriterror == 0) {
        // Store MVD details in db for now.
        // Parse existing mvd results to check if one exists with same name.
        // Get id and delete.
        $mvdsearchurl = $base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/?searchField=name&query=" . $docpath . "$";
        $mvdsearch = file_get_contents($mvdsearchurl);
        $mvdsearchjson = json_decode($mvdsearch, TRUE);
        $existingmvdcount = $mvdsearchjson['count'];
        if ($existingmvdcount != 0) {
          $existingmvd = $mvdsearchjson['results'][0]['id'];
          $ch3 = curl_init();
          curl_setopt($ch3, CURLOPT_CUSTOMREQUEST, 'DELETE');
          curl_setopt($ch3, CURLOPT_RETURNTRANSFER, TRUE);
          curl_setopt($ch3, CURLOPT_URL, trim($base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/" . $existingmvd));
          $dbresponse = curl_exec($ch3);
          $dberr = curl_errno($ch3);
          $dberror = curl_error($ch3);
          $info3 = curl_getinfo($ch3);
          curl_close($ch3);
          if ($dberr) {
            return t("Error: @dberr @dberror @dbresponse", array(
              '@dberr' => $dberr,
              '@dberror' => $dberror,
              '@dbresponse' => $dbresponse));
          }
          if ($info3['http_code'] !== 204) {
            // Success with No Content.
            return t('Error deleting old mvd record: @dbresponse', array('@dbresponse' => $dbresponse));
          }
        }
        // Post new MVD details to db.
        $mvddburl = $base_url . "/" . drupal_get_path('module', 'repository') . "/api/mvds/";

        $newmvd = array(
          'name' => $origdocpath,
          'resources' => $id_array,
          'filter' => $filter,
          'project' => $project,
        );

        $ch2 = curl_init();
        curl_setopt($ch2, CURLOPT_URL, trim($mvddburl));
        curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch2, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($ch2, CURLOPT_POST, 1);
        curl_setopt($ch2, CURLOPT_POSTFIELDS, json_encode($newmvd));
        curl_setopt($ch2, CURLOPT_HTTPHEADER, array("Content-type: application/json;"));

        $dbresponse = curl_exec($ch2);
        $dberr = curl_errno($ch2);
        $dberror = curl_error($ch2);
        $info = curl_getinfo($ch2);
        curl_close($ch2);
        if ($dberr) {
          return t("Error: @dberr @dberror @dbresponse", array(
            '@dberr' => $dberr,
            '@dberror' => $dberror,
            '@dbresponse' => $dbresponse));
        }
        if ($info['http_code'] !== 201) {
          // Success with No Content.
          return t("Error creating mvd record: ") . $dbresponse;
        }

        // Message is success message.
        $project_param = '';
        if ($project) {
          $project_param = "?project= $project";
        }
        $message = "<p>MVD created. You may now view <a href='/collationtools/apparatus$project_param#$docpath'>table apparatus</a> or <a href='/collationtools/compare$project_param#$docpath'>side-by-side comparison</a></p>";
      }
      else {
        $message = "<p>MVD was not created. Refer to the log above for the cause.";
      }
    }
    else {
      // Else message is failure message.
      $message = "<p>There was an error creating the MVD. Refer to the log above for the cause. If the log does not indicate a reason, the operation may have failed as a result of versions being too large or too much variation between versions. Try splitting larger transcriptions into smaller sections.</p>";
    }
    $breadcrumb = array();
    $breadcrumb[] = l(t('Home'), '<front>');
    _add_project_or_repository($breadcrumb);
    $breadcrumb[] = t(drupal_get_title());
    drupal_set_breadcrumb($breadcrumb);
    return t("<p>$response</p> @message", array('@message' => $message));
  }
}

/**
 * Returns Mongo file associated with pid.
 *
 * @param String $pid
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
  $query = array('_resourceid' => $id);
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
function islandora_calliope_find_mvd($critical_edition_object) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $dbhost = variable_get('islandora_critical_edition_mongo_host', "localhost");
  $dbport = variable_get('islandora_critical_edition_mongo_port', "27017");
  $dbname = variable_get('islandora_critical_edition_mongo_db', "test");
  $project = islandora_escape_pid_for_function($critical_edition_object->id);
  $query = array('metadata.project' => $project);
  try {
    $mongoclient = new MongoClient("$dbhost:$dbport");
    $db = $mongoclient->selectDB($dbname);
    $collection = $db->selectCollection('mvds');
    $mvd_cursor = $collection->find($query);
    $mvd_data = array();
    foreach ($mvd_cursor as $mvd) {
      $mvd_data[] = $mvd['metadata'];
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage());
    exit;
  }
  return $mvd_data;
}
