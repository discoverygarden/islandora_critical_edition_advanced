<?php

/**
 * @file
 * Handles the display/submission of the publish settings form for this module.
 */

/**
 * Define the title children form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   The collection to share child objects from.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_advanced_title_page_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_epigraph_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_preface_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_foreword_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_introduction_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_illustrations_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_acknowledgements_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_editorial_introduction_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_headnote_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_footnote_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_endnotes_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_epilogue_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_afterword_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}

function islandora_critical_edition_advanced_editorial_procedures_form(array $form, array &$form_state, AbstractObject $object) {
	$form_state['collection'] = $object;
	$fragment = '#share-children';
	return array(
			'#action' => request_uri() . $fragment,
			'collection' => array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			),
			'children' => array(),
	);
}
