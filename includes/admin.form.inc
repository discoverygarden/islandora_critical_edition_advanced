<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_admin_settings_form(array $form, array &$form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  $default_streams = variable_get('critical_apparatus_streams', FALSE);
  $streams = $default_streams ? $default_streams : islandora_critical_apparatus_get_datastreams();
  $default_stream_text = '';
  foreach ($streams as $dsid => $title) {
    $default_stream_text .= "$dsid ~ $title \n";
  }
  $form = array();
  $form['repository'] = array(
    '#type' => 'fieldset',
    '#title' => 'Mongo Configuration',
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );

  $form['repository']['islandora_critical_edition_mongo_port'] = array(
    '#type' => 'textfield',
    '#title' => t("Port"),
    '#description' => t("Mongo Database Port"),
    '#default_value' => variable_get('islandora_critical_edition_mongo_port', "27017"),
  );
  $form['repository']['islandora_critical_edition_mongo_host'] = array(
    '#type' => 'textfield',
    '#title' => t("Address"),
    '#description' => t("Mongo Database server address"),
    '#default_value' => variable_get('islandora_critical_edition_mongo_host', "localhost"),
  );
  $form['repository']['islandora_critical_edition_mongo_db'] = array(
    '#type' => 'textfield',
    '#title' => t("Database name"),
    '#description' => t("Mongo Database instance name"),
    '#default_value' => variable_get('islandora_critical_edition_mongo_db', "test"),
  );
  $form['critical_edition_thumbnail_size'] = array(
    '#title' => t("Scaled size of Critical Edition thumbnail"),
    '#description' => t("This will alter the size of new thumbnails only. !break Existing thumbnails will retain their current size.", array("!break" => '<br />')),
    '#type' => 'textfield',
    '#size' => 6,
    '#default_value' => variable_get('critical_edition_thumbnail_size', 150),
  );
  $form['apparatus_streams'] = array(
    '#type' => 'textarea',
    '#title' => t('Apparatus Components'),
    '#description' => t("DSID - Title to populate the Critical Apparatus. ie  EDITORIAL_INTRODUCTION ~ Editorial Introduction !break
                       DSID's must begin and end with a letter and may contain only letters and underscores !break
                       DSID's must be separated from Titles with tildes, and DSID - Title pairs must be separated by commas", array("!break" => '<br />')),
    '#default_value' => $default_stream_text,
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to Default DSIDs'),
    '#attributes' => array('source' => 'reset_dsids'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
    '#attributes' => array('source' => 'save_dsids'),
  );
  return $form;
}

/**
 * Submit handler for admin form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 */
function islandora_critical_edition_admin_settings_form_submit(array &$form, array $form_state) {
  $action = $form_state['triggering_element']['#attributes']['source'];
  if ($action == 'save_dsids') {
    $input = trim($form_state['values']['apparatus_streams']);
    $streams = explode("\n", $input);
    $apparatus_streams = array();
    foreach ($streams as $stream) {
      $parts = explode('~', $stream);
      $apparatus_streams[trim($parts[0])] = trim($parts[1]);
    }
    variable_set('critical_apparatus_streams', $apparatus_streams);
    $confirmation = t('The configuration has been saved.');
  }
  else {
    module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
    variable_set('critical_apparatus_streams', islandora_critical_apparatus_get_datastreams());
    $confirmation = t('The configuration has been reset.');
  }
  drupal_set_message($confirmation);
}
