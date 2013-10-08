<?php
/**
 * @file
 * Utility functions for islandora_critical_editions_advanced.
 */

/**
 * Returns associated objects.
 *
 * @param String $object_pid
 *   PID of object
 * @param String $content_model
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
 * @param String $versionable_object_pid
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
 * Builds and returns query to get all book objects in repository.
 *
 * @param integer $offset
 *   Offset for query
 *
 * @return String
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
