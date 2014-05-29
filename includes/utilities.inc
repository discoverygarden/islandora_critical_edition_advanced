<?php

/**
 * @file
 * Utility functions for islandora_critical_editions_advanced.
 */

/**
 * Returns associated objects.
 *
 * @param string $object_pid
 *   PID of object
 * @param string $content_model
 *   Optional Content Model to restrict results.
 *
 * @return array
 *   associative array of PID => Content Model pairs.
 */
function islandora_critical_edition_get_members($object_pid, $content_model = NULL) {
  $qualifier = $content_model ? "fm:hasModel <info:fedora/$content_model>;" : "";
  $tuque = islandora_get_tuque_connection();
  $query = "PREFIX fm: <" . FEDORA_MODEL_URI . ">
            PREFIX fr: <" . FEDORA_RELS_EXT_URI . ">
            SELECT ?subject ?object
            FROM <#ri>
            WHERE {
              {?subject fm:state fm:Active;
                        fr:isMemberOfCollection <info:fedora/$object_pid>;
                        fm:hasModel ?object;
                        $qualifier

              }
            UNION
              {?subject fm:state fm:Active;
                        fr:isMemberOf <info:fedora/$object_pid>;
                        fm:hasModel ?object;
                        $qualifier
              }
            }
            ORDER BY ?subject";
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    if ($result['object']['value'] != 'fedora-system:FedoraObject-3.0') {
      $objects[$result['subject']['value']] = $result['object']['value'];
    }
  }
  return $objects;
}

/**
 * Gets all transcriptions related to this object.
 *
 * @param string $versionable_object_pid
 *   PID of object
 *
 * @return array
 *   Array of pid=>label pairs
 */
function islandora_critical_get_related_transcriptions($versionable_object_pid) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $tuque = islandora_get_tuque_connection();
  $query = "
            PREFIX fm: <" . FEDORA_MODEL_URI . ">
            PREFIX fr: <" . FEDORA_RELS_EXT_URI . ">

            SELECT DISTINCT ?subject ?label FROM <#ri>
                WHERE {
                        {?subject fm:hasModel <info:fedora/islandora:transcriptionCModel>;
                                  fr:isMemberOf <info:fedora/$versionable_object_pid>;
                       }
                 OPTIONAL{
                           ?subject fm:label  ?label;
                          }
                       }";
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $objects[$result['subject']['value']] = $result['label']['value'];
  }
  return $objects;
}

/**
 * Gets all Critical Editions to which this object is a member.
 *
 * @param string $versionable_object_pid
 *   PID of object
 *
 * @return array
 *   Array of pid=>label pairs
 */
function islandora_critical_get_parent_editions($versionable_object_pid) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $tuque = islandora_get_tuque_connection();
  $query = "
            PREFIX fm: <" . FEDORA_MODEL_URI . ">
            PREFIX fr: <" . FEDORA_RELS_EXT_URI . ">

            SELECT DISTINCT ?object FROM <#ri>
                WHERE {
                        {<info:fedora/$versionable_object_pid> fm:hasModel <info:fedora/islandora:versionableObjectCModel>;
                                  fr:isMemberOf ?object;
                        }

                      }";
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  foreach ($results as $result) {
    $objects[] = $result['object']['value'];
  }
  return $objects;
}

/**
 * Returns array of possible apparatus datastreams.
 *
 * @return array
 *   Associative array of dsid => label
 */
function islandora_critical_apparatus_get_datastreams() {
  return array(
    'title_page' => 'Title Page',
    'epigraph' => 'Epigraph',
    'preface' => 'Preface',
    'foreword' => 'Foreword',
    'introduction' => 'Introduction',
    'illustrations' => 'List of Illustrations, Figures, Graphs',
    'acknowledgements' => 'Acknowledgements',
    'editorial_introduction' => 'Editorial Introduction',
    'headnote' => 'Headnote',
    'footnote' => 'Footnotes',
    'endnotes' => 'Endnotes',
    'epilogue' => 'Epilogue',
    'afterword' => 'Afterword',
    'editorial_procedures' => 'Editorial Procedures',
  );
}

/**
 * Returns all specified objects in the Fedora Repository.
 *
 * @return array
 *   array of PID => Label pairs
 */
function islandora_critical_edition_get_related_objects($content_model, $offset = 0, $limit = 'unlimited') {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $tuque = islandora_get_tuque_connection();
  $query = islandora_critical_edition_get_associated_objects_query($content_model, $offset);
  $objects = array();
  $results = $tuque->repository->ri->sparqlQuery($query, $limit);
  foreach ($results as $result) {
    $objects[$result['subject']['value']] = $result['label']['value'];
  }
  return $objects;
}

/**
 * Builds and returns query to get objects filtered by content model.
 *
 * @param string $content_model
 *   The content model to filter by.
 * @param int $offset
 *   Offset for query
 *
 * @return string
 *   Query to get relevant objects
 */
function islandora_critical_edition_get_associated_objects_query($content_model, $offset) {
  $query = "PREFIX fm: <info:fedora/fedora-system:def/model#>
                SELECT DISTINCT ?subject ?label FROM <#ri>
                WHERE {
                        {?subject fm:hasModel <info:fedora/$content_model>;
                       }
                 OPTIONAL{
                           ?subject fm:label  ?label;
                          }
                       }
            ORDER BY ?label OFFSET $offset";
  return $query;
}

/**
 * Adds paging to form and determines offset.
 *
 * @param AbstrctObject $object
 *   Object representing containing object
 * @param string $content_model
 *   PID of content model to limit search results
 *
 * @return int
 *   Offset for paging
 */
function islandora_critical_edition_prep_form($object, $content_model) {
  module_load_include('inc', 'islandora_critical_edition', 'includes/utilities');
  drupal_set_breadcrumb(islandora_get_breadcrumbs($object));
  drupal_set_title($object->label);
  $offset = 0;
  $tuque = islandora_get_tuque_connection();
  $repository = $tuque->repository;
  $total = $repository->ri->countQuery(islandora_critical_edition_get_associated_objects_query($content_model, 0), 'sparql');
  $limit = 25;
  if ($total < 28) {
    $limit = $total;
  }
  if ($total) {
    $current_page = pager_default_initialize($total, $limit);
    $offset = $current_page * $limit;
  }

  return $offset;
}

/**
 * Theme callback to theme the trimmed editor.
 *
 * @param AbstractObject $object
 *   The object to process in the simple viewer.
 */
function islandora_critical_edition_advanced_view_trimmed_editor(AbstractObject $object) {
  $module_path = drupal_get_path('module', 'islandora_critical_edition_advanced');
  // Hide what we dont need to see in the plain viewer.
  drupal_add_css("$module_path/css/show_plain.css");
  return theme("islandora_critical_edition", array('islandora_object' => $object));
}

/**
 * Base form for All/Audio/Video associate TEI-RDF forms.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $object
 *   The Object to associated the selected TEI-RDF object with.
 * @param string $content_model
 *   Limit the possible TEI-RDF objects to those with this content model type.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_critical_edition_associate_tei_rdf_base_form($form, $form_state, $object, $content_model) {
  $object_models = $object->models;
  $system = array_search("fedora-system:FedoraObject-3.0", $object_models);
  unset($object_models[$system]);
  // Assume the object's first content model is the definitive one, ignore
  // the default object content model though.
  $object_model = array_shift($object_models);
  $offset = islandora_critical_edition_prep_form($object, $content_model);
  $name_mappings = array(
    'islandora:criticalEditionContainerCModel' => t('Critical Edition'),
    'islandora:versionableObjectCModel' => t('Versionable Object'),
  );
  $associate_with_label = $name_mappings[$object_model];
  $objects = islandora_critical_edition_get_related_objects($content_model, $offset);
  $rows = array();
  foreach ($objects as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  return array(
    'pager' => array(
      '#type' => 'markup',
      '#markup' => theme('pager', array('quantity', count($rows))),
    ),
    'intro' => array(
      '#type' => 'markup',
      '#markup' => t("Choose RDF-TEI object to associate with this @label.", array('@label' => $associate_with_label)),
    ),
    'table' => array(
      '#type' => 'tableselect',
      '#header' => array(
        'title' => array('data' => t('TEI-RDF Object')),
        'pid' => array('data' => t('PID')),
      ),
      '#options' => $rows,
      '#empty' => t("There are no RDF-TEI Objects in this Fedora Repository."),
      '#js' => FALSE,
      '#multiple' => FALSE,
    ),
    'vo_title' => array(
      '#type' => 'textfield',
      '#title' => t("Title for new Versionable Object"),
      '#required' => TRUE,
    ),
    'next' => array(
      '#type' => 'submit',
      '#value' => t('Associate TEI-RDF Object'),
    ),
    'container_object_pid' => array(
      '#type' => 'hidden',
      '#value' => $object->id,
    ),
    'container_object_model' => array(
      '#type' => 'hidden',
      '#value' => $object_model,
    ),
  );
}

/**
 * Creates a select table for choosing one of the provided objects.
 */
function islandora_critical_edition_create_select_teirdf_object_table(array $objects, $associate_with_label) {
  $name_mappings = array(
    'islandora:criticalEditionContainerCModel' => t('Critical Edition'),
    'islandora:versionableObjectCModel' => t('Versionable Object'),
  );
  $associate_with_label = $name_mappings[$object_model];
  $rows = array();
  foreach ($objects as $pid => $label) {
    $rows[$pid] = array(
      'pid' => $pid,
      'title' => $label,
    );
  }
  return array(
    'pager' => array(
      '#type' => 'markup',
      '#markup' => theme('pager', array('quantity', count($rows))),
    ),
    'intro' => array(
      '#type' => 'markup',
      '#markup' => t("Choose RDF-TEI object to associate with this @label.", array('@label' => $name_mappings[$object_model])),
    ),
    'table' => array(
      '#type' => 'tableselect',
      '#header' => array(
        'title' => array('data' => t('TEI-RDF Object')),
        'pid' => array('data' => t('PID')),
      ),
      '#options' => $rows,
      '#empty' => t("There are no RDF-TEI Objects in this Fedora Repository."),
      '#js' => FALSE,
      '#multiple' => FALSE,
    ),
  );
}

/**
 * Gets the number of child versionable objects the given object has.
 *
 * @param AbstractObject $object
 *   A critical edition containter object.
 *
 * @return int
 *   The number of versionable objects the given object has.
 */
function islandora_critical_edition_number_of_child_versionable_objects(AbstractObject $object) {
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
PREFIX islandora: <http://islandora.ca/ontology/relsext#>
SELECT DISTINCT ?object FROM <#ri> WHERE {
 ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:versionableObjectCModel> .
}
EOT;
  return $object->repository->ri->countQuery($query, 'sparql');
}

/**
 * Gets the number of child collation objects the given object has.
 *
 * @param AbstractObject $object
 *   A critical edition containter object.
 *
 * @return int
 *   The number of collation objects the given object has.
 */
function islandora_critical_edition_number_of_child_transcriptions(AbstractObject $object) {
  // Count the number of transcriptions of all verionable objects.
  $query = <<<EOT
    PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
    PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
    PREFIX islandora: <http://islandora.ca/ontology/relsext#>
      SELECT DISTINCT ?transcript FROM <#ri> WHERE {
        ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
        fedora-model:hasModel <info:fedora/islandora:versionableObjectCModel> .
        ?transcript fedora-rels-ext:isMemberOf ?object ;
        fedora-model:hasModel <info:fedora/islandora:transcriptionCModel> .
      }
EOT;
  return $object->repository->ri->countQuery($query, 'sparql');
}

/**
 * Gets the number of child collation objects the given object has.
 *
 * @param AbstractObject $object
 *   A critical edition containter object.
 *
 * @return int
 *   The number of collation objects the given object has.
 */
function islandora_critical_edition_number_of_child_collation_objects(AbstractObject $object) {
  // Count the number of collations.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
PREFIX islandora: <http://islandora.ca/ontology/relsext#>
SELECT DISTINCT ?object FROM <#ri> WHERE {
 ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:mvdCModel> .
}
EOT;
  return $object->repository->ri->countQuery($query, 'sparql');
}

/**
 * Gets the apparatus associated with this critical edition container.
 *
 * @param AbstractObject $object
 *   A critical edition containter object.
 *
 * @return string
 *   The apparatus associated with this critical edition container.
 */
function islandora_critical_edition_get_apparatus(AbstractObject $object) {
  // Count the number of collations.
  $query = <<<EOT
PREFIX fedora-rels-ext: <info:fedora/fedora-system:def/relations-external#>
PREFIX fedora-model: <info:fedora/fedora-system:def/model#>
PREFIX islandora: <http://islandora.ca/ontology/relsext#>
SELECT DISTINCT ?object FROM <#ri> WHERE {
 ?object fedora-rels-ext:isMemberOf <info:fedora/$object->id> ;
         fedora-model:hasModel <info:fedora/islandora:criticalApparatusCModel> .
}
EOT;
  $results = $object->repository->ri->sparqlQuery($query);
  if (count($results)) {
    return $results[0]['object']['value'];
  }
  return NULL;
}

/**
 * Converts the given TEI xml document to plain text.
 *
 * @param string $tei
 *   A document containing TEI xml.
 */
function islandora_critical_edition_convert_tei_to_plain_text($tei) {
  // @codingStandardsIgnoreStart
  $replace_candidates = array(
    '<br>',
    '<br />',
    '<lb></lb>',
    '<lb/>',
    '</l>',
    '</lg>',
  );
  // @codingStandardsIgnoreEnd
  $text = simplexml_load_string($tei)->text[0]->asXML();
  foreach ($replace_candidates as $candidate) {
    $text = str_replace($candidate, "$candidate\n", $text);
  }
  return strip_tags($text);
}

/**
 * Append the given TEI-RDF documents text to the given TEI Document.
 *
 * This function is used to generate a consolidated TEI document from the
 * TEI-RDF page objects.
 *
 * @param string $tei
 *   The TEI document to append the TEI-RDF document to.
 * @param string $tei_rdf
 *   The TEI-RDF document to append.
 *
 * @return string
 *   The given TEI document with the appended TEI-RDF document.
 */
function islandora_critical_edition_advanced_append_tei_rdf_text_to_tei($tei, $tei_rdf) {
  define('TEI_NAMESPACE', 'http://www.tei-c.org/ns/1.0');
  $tei_document = new DOMDocument();
  $tei_document->formatOutput = TRUE;
  $tei_document->preserveWhiteSpace = TRUE;
  $tei_document->loadXML($tei);
  $front = $tei_document->getElementsByTagName('front')->item(0);
  $body = $tei_document->getElementsByTagName('body')->item(0);
  $back = $tei_document->getElementsByTagName('back')->item(0);
  $tei_rdf_document = new DOMDocument();
  $tei_rdf_document->formatOutput = TRUE;
  $tei_rdf_document->preserveWhiteSpace = TRUE;
  $tei_rdf_document->loadXML($tei_rdf);
  $tei_rdf_text_node = $tei_rdf_document->getElementsByTagNameNS(TEI_NAMESPACE, 'text')->item(0);
  foreach ($tei_rdf_text_node->childNodes as $tei_rdf_content_node) {
    if (isset(${$tei_rdf_content_node->nodeName})) {
      $tei_rdf_fragment = $tei_document->createDocumentFragment();
      foreach ($tei_rdf_content_node->childNodes as $tei_rdf_content_node_child) {
        $tei_rdf_fragment->appendXML($tei_rdf_document->saveXML($tei_rdf_content_node_child));
      }
      ${$tei_rdf_content_node->nodeName}->appendChild($tei_rdf_fragment);
    }
  }
  $my_doc = $tei_document->saveHTML();
  return $tei_document->saveXML();
}

/**
 * Returns empty TEI form.
 *
 * @param string $title
 *   Title for TEI
 *
 * @return string
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
 * Get audio params for jwplayer.
 *
 * @param AbstractObject $reference_object
 *   object representing fedora audio object
 *
 * @return array
 *   Associative array containing params.
 */
function islandora_critical_edition_advanced_get_audio_params($reference_object) {
  $audio_url = url("islandora/object/{$reference_object->id}/datastream/PROXY_MP3/view", array('absolute' => TRUE));
  $audio_params = array(
    "pid" => $reference_object->id,
    "url" => $audio_url,
    "mime" => 'audio/mpeg',
    "width" => "100%",
  );
  return $audio_params;
}

/**
 * Get video params for jwplayer.
 *
 * @param AbstractObject $reference_object
 *   object representing fedora audio object
 *
 * @return array
 *   Associative array containing params.
 */
function islandora_critical_edition_advanced_get_video_params($reference_object) {
  if (stristr($_SERVER['HTTP_USER_AGENT'], 'chrome') !== FALSE) {
    $viewer_dsid = 'OGG';
  }
  else {
    $viewer_dsid = 'MP4';
  }

  $video_params = array(
    'pid' => $reference_object->id,
  );
  // Video player.
  if (isset($reference_object[$viewer_dsid]) && islandora_datastream_access(FEDORA_VIEW_OBJECTS, $reference_object[$viewer_dsid])) {
    $video_url = url("islandora/object/{$reference_object->id}/datastream/$viewer_dsid/view");
    $video_params += array(
      'mime' => 'video/mp4',
      'url' => $video_url,
      'width' => "100%",
    );
  }
  // Thumbnail.
  if (isset($reference_object['TN']) && islandora_datastream_access(FEDORA_VIEW_OBJECTS, $reference_object['TN'])) {
    $video_params += array(
      'tn' => url("islandora/object/{$reference_object->id}/datastream/TN/view", array('absolute' => TRUE)),
    );
  }
  return $video_params;
}
