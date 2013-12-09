<?php

/**
 * @file
 * Methods to consolidate page level tei streams.
 */

/**
 * Consolidates page level TEI to book level TEI.
 *
 * @param AbstractObject $object
 *   Object representing either TEI-RDF object, or enclosing Versionable Object
 */
function islandora_critical_edition_consolidate_tei($object, $dsid = 'CWRC') {
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');

  $content_models = $object->models;
  if (in_array('islandora:versionableObjectCModel', $content_models)) {
    $tei_rdfs = array_keys(islandora_critical_edition_get_members($object->id, 'islandora:criticalEditionCModel'));
    $tei_rdf_object = islandora_object_load($tei_rdfs[0]);
  }
  else {
    $tei_rdf_object = $object;
  }
  $page_results = islandora_paged_content_get_pages($tei_rdf_object);
  $pids_to_add = array_keys($page_results);
  $title = $tei_rdf_object->label;

  $master = new DOMDocument();
  $master->formatOutput = TRUE;
  $master->preserveWhiteSpace = TRUE;
  $master->loadXML(islandora_critical_edition_advanced_get_blank_tei($title));
  $master_content_nodes = array();
  $master_content_nodes['front'] = $master->getElementsByTagName('front')->item(0);
  $master_content_nodes['body'] = $master->getElementsByTagName('body')->item(0);
  $master_content_nodes['back'] = $master->getElementsByTagName('back')->item(0);

  foreach ($pids_to_add as $pid) {
    $fedora_object = islandora_object_load($pid);
    if ($fedora_object[$dsid]) {
      $new_dom = new DOMDocument();
      $new_dom->formatOutput = TRUE;
      $new_dom->preserveWhiteSpace = TRUE;
      $new_dom->loadXML($fedora_object[$dsid]->content);
      $text_node = $new_dom->getElementsByTagName('text')->item(0);
      $content_nodes = $text_node->childNodes;
      foreach ($content_nodes as $content_node) {
        $content_node = $master->importNode($content_node, TRUE);
        $children = $content_node->childNodes;
        foreach ($children as $child) {
          $master_content_nodes[$content_node->tagName]->appendChild($child);
        }
      }
    }
  }
  islandora_critical_edition_upsert_tei($tei_rdf_object, $master->saveXML());
}

/**
 * Returns empty TEI form.
 *
 * @param String $title
 *   Title for TEI
 *
 * @return String
 *   Blank TEI
 */
function islandora_critical_edition_advanced_get_blank_tei($title) {
  $tei = <<<EOD
<TEI xmlns="http://www.tei-c.org/ns/1.0">
    <teiHeader>
        <fileDesc>
            <titleStmt>
                <title>$title</title>
            </titleStmt>
            <publicationStmt>
                <p/>
            </publicationStmt>
            <sourceDesc>
                <p>Created from islandora book ingest unless otherwise noted.</p>
            </sourceDesc>
        </fileDesc>
    </teiHeader>

    <text>
        <front></front>
        <body></body>
        <back></back>
    </text>
</TEI>
EOD;

  return $tei;
}

/**
 * Updates or inserts consolidated TEI datastream.
 *
 * @param abstractObject $tei_rdf
 *   Object representing TEI_RDf editior
 * @param String $tei
 *   TEI string to be upserted
 */
function islandora_critical_edition_upsert_tei($tei_rdf, $tei) {
  if ($tei_rdf['TEI']) {
    $tei_rdf->content = $tei;
  }
  else {
    $tei_datastream = $tei_rdf->constructDatastream('TEI', 'M');
    $tei_datastream->mimetype = 'text/plain';
    $tei_datastream->content = $tei;
    $tei_datastream->label = "Consolidated TEI";
    $tei_rdf->ingestDatastream($tei_datastream);
  }
}

/**
 * Takes consolidated TEI and converts it to HTML with xsl.
 *
 * @global type $base_url
 *
 * @param abstractObject $object
 *   Object representing TEI-RDF object
 *
 * @return String
 *   Converted TEI
 */
function islandora_critical_edition_advanced_display_tei($object, $dsid = 'TEI') {
  module_load_include('inc', 'islandora', 'includes/authtokens');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');

  $content_models = $object->models;
  if (in_array('islandora:versionableObjectCModel', $content_models)) {
    $tei_rdfs = array_keys(islandora_critical_edition_get_members($object->id, 'islandora:criticalEditionCModel'));
    $object = islandora_object_load($tei_rdfs[0]);
  }
  global $base_url;
  $islandora_url = variable_get('islandora_base_url', 'http://localhost:8080/fedora');
  $tomcat_url = str_replace('/fedora', '', $islandora_url);
  $pid = $object->id;
  if (!$object[$dsid]) {
    islandora_critical_edition_consolidate_tei($object);
  }
  $path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  drupal_add_css("$path/css/islandora_critical_edition_common_tei.css");
  $xml_path = url("islandora/object/$pid/datastream/$dsid/view", array(
    'absolute' => TRUE,
    'query' => array(
      'token' => islandora_get_object_token($pid, $dsid),
    ),
  ));
  $xslt_path = "$base_url/$path/xml/EMiC_test_XSLT.xsl";
  return file_get_contents("$tomcat_url/saxon/SaxonServlet?source=$xml_path&style=$xslt_path");
}

function islandora_critical_edition_advanced_display_page_tei($object, $dsid = 'CWRC') {
  module_load_include('inc', 'islandora', 'includes/authtokens');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');
  module_load_include('inc', 'islandora_critical_edition_advanced', 'includes/utilities');

  global $base_url;
  $islandora_url = variable_get('islandora_base_url', 'http://localhost:8080/fedora');
  $tomcat_url = str_replace('/fedora', '', $islandora_url);
  $pid = $object->id;
  if (!$object['TEI']) {
    islandora_critical_edition_consolidate_tei($object);
  }
  $path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  $xml_path = url("islandora/object/$pid/datastream/TEI/view", array(
    'absolute' => TRUE,
    'query' => array(
      'token' => islandora_get_object_token($pid, $dsid),
    ),
  ));
  $xslt_path = "$base_url/$path/xml/EMiC_test_XSLT.xsl";
  return file_get_contents("$tomcat_url/saxon/SaxonServlet?source=$xml_path&style=$xslt_path");
}

