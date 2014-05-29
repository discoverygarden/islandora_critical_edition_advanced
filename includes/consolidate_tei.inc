<?php

/**
 * @file
 * Methods to consolidate page level tei streams.
 */

/**
 * Returns transformed from CWRC datatstream.
 *
 * @global type $base_url
 *
 * @param AbstractObject $object
 *   Object representing fedora page object
 * @param string $dsid
 *   Datastream ID of object
 *
 * @return string
 *   transformed xml
 */
function islandora_critical_edition_advanced_display_page_tei($object, $dsid = 'CWRC') {
  module_load_include('inc', 'islandora', 'includes/authtokens');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');

  global $base_url;
  $islandora_url = variable_get('islandora_base_url', 'http://localhost:8080/fedora');
  $tomcat_url = str_replace('/fedora', '', $islandora_url);
  $pid = $object->id;

  $path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  $xml_path = url("islandora/object/$pid/datastream/$dsid/view", array(
    'absolute' => TRUE,
    'query' => array(
      'token' => islandora_get_object_token($pid, $dsid),
    ),
  ));
  $xslt_path = "$base_url/$path/xml/EMiC_test_XSLTv2.xsl";
  return file_get_contents("$tomcat_url/saxon/SaxonServlet?source=$xml_path&style=$xslt_path");
}
