<?php

/**
 * @file
 * Workbench page and related forms.
 */

/**
 * Add the given object to the Work Bench.
 *
 * @param AbstractObject $object
 *   The object to add to the work bench.
 */
function islandora_critical_edition_advanced_add_to_workbench(AbstractObject $object) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    Bookmark::getList($default_list)->addPid($object->id);
  }
}

/**
 * Remove the given object from the Work Bench.
 *
 * We don't check that the object exists for removal, as we don't care if it
 * exists anymore.
 *
 * @param string $pid
 *   The object to remove to the work bench.
 */
function islandora_critical_edition_advanced_remove_from_workbench($pid) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    Bookmark::getList($default_list)->removePid($pid);
  }
}

/**
 * Toggles the given objects membership to the current users workbench.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_toggle_workbench_membership_form(array $form, array &$form_state, AbstractObject $object) {
  module_load_include('inc', 'islandora_bookmark', 'includes/api');
  form_load_include($form_state, 'inc', 'islandora_critical_edition_advanced', 'includes/workbench');
  dsm($form_state);
  $in_work_bench = FALSE;
  $default_list = islandora_bookmark_get_default_list();
  if ($default_list !== FALSE) {
    $default_list = Bookmark::getList($default_list);
    $in_work_bench = in_array($object->id, $default_list->getPids());
  }
  return array(
    'object' => array(
      '#type' => 'value',
      '#value' => $object->id,
    ),
    'in_work_bench' => array(
      '#type' => 'value',
      '#value' => $in_work_bench,
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    ),
  );
}

/**
 * Toggles the membership of the objects within the work bench.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_critical_edition_advanced_toggle_workbench_membership_form_submit(array $form, array &$form_state) {
  watchdog('testing', 'tests');
  $object = islandora_object_load($form_state['values']['object']);
  if ($form_state['values']['in_work_bench']) {
    islandora_critical_edition_advanced_remove_from_workbench($object->id);
  }
  else {
    islandora_critical_edition_advanced_add_to_workbench($object);
  }
}
