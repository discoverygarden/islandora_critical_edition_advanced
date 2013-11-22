<?php

/**
 * @file
 * Handles the display/submission of the publish settings form for this module.
 */

function islandora_critical_edition_advanced_publish_scroll_form(array $form, array &$form_state, AbstractObject $object) {
	$form['all_page_title_wrapper']['#type'] = 'fieldset';
	$form['all_page_title_wrapper']['fldst_one'] = array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			);
	return $form;
}

function islandora_critical_edition_advanced_title_page_form(array $form, array &$form_state, AbstractObject $object) {
	$form['all_page_title_wrapper']['#type'] = 'fieldset';
	$form['all_page_title_wrapper']['#attributes'] = array('class' => array('container-inline'));
	
	$form['all_page_title_wrapper']['fldst_one']['#type'] = 'fieldset';
	
	$form['all_page_title_wrapper']['fldst_two']['#type'] = 'fieldset';
	
	$form['all_page_title_wrapper']['fldst_one']['title_content'] = array(
					'#title' => t('Share members with collection'),
					'#description' => t("Members can be shared with any number of collections."),
					'#type' => 'select',
					'#options' => array(),
			);
	return $form;
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
