<?php

/**
 * @file
 * Forms for populating Critical Apparatus
 */

/**
 * Form to populate Critical Apparatus.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form stateu
 * @param AbstractObject $apparatus
 *   object representing Critical Apparatus
 *
 * @return array
 *   Drupal form
 */
function critical_apparatus_form($form, $form_state, $apparatus) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  module_load_include('inc', 'islandora', 'includes/breadcrumb');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($apparatus));
  drupal_set_title($apparatus->label);
  $datastreams = variable_get('critical_apparatus_streams', islandora_critical_apparatus_get_datastreams());
  $chosen = isset($form_state['values']['datastream']) ? $form_state['values']['datastream'] : 'title_page';
  $content = t('Enter text for @datastream', array('@datastream' => $datastreams[$chosen]));
  if ($apparatus[$chosen]) {
    $content = $apparatus[$chosen]->content;
  }
  $form = array();
  $form['pid'] = array(
    '#type' => 'hidden',
    '#value' => $apparatus->id,
  );

  $form['datastream'] = array(
    '#title' => t('Select Component'),
    '#type' => 'select',
    '#options' => $datastreams,
    '#default_value' => $chosen,
    '#ajax' => array(
      'callback' => 'datastream_ajax_callback',
      'wrapper' => 'wrapper',
      'effect' => 'fade',
      'method' => 'replace',
    ),
  );

  $form['wrapper'] = array(
    '#prefix' => '<div id="wrapper">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
  );

  $form['wrapper'][$chosen] = array(
    '#title' => t('Enter text'),
    '#type' => 'text_format',
    '#base_type' => 'textarea',
    '#default_value' => $content,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create / Update'),
  );

  return $form;
}

/**
 * Ajax callback.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 *
 * @return array
 *   element to be re-rendered
 */
function datastream_ajax_callback($form, $form_state) {
  return $form['wrapper'];
}

/**
 * Submit handler for apparatus form.
 *
 * @param array $form
 *   Drupal form
 * @param array $form_state
 *   Drupal form state
 */
function critical_apparatus_form_submit($form, $form_state) {
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');
  $datastreams = variable_get('critical_apparatus_streams', islandora_critical_apparatus_get_datastreams());
  $apparatus = islandora_object_load($form_state['values']['pid']);
  $chosen = $form_state['values']['datastream'];
  $content = $form_state['values'][$chosen];
  if (is_array($content)) {
    $content = $content['value'];
  }
  if ($apparatus[$chosen]) {
    $apparatus[$chosen]->content = $content;
  }
  else {
    $datastream = $apparatus->constructDatastream($chosen, 'M');
    $datastream->setContentFromString($content);
    $datastream->label = $chosen;
    $datastream->mimetype = 'text/plain';
    $apparatus->ingestDatastream($datastream);
  }
  drupal_set_message(t("Updated @ds", array('@ds' => $datastreams[$chosen])));
}
