<?xml version="1.0" encoding="UTF-8"?>
<!-- We need all lower level namespaces to be declared here for exclude-result-prefixes attributes
     to be effective -->
<xsl:stylesheet exclude-result-prefixes="exts" version="1.0" xmlns:dc="http://purl.org/dc/elements/1.1/"
  xmlns:dwc="http://rs.tdwg.org/dwc/xsd/simpledarwincore/" xmlns:eaccpf="urn:isbn:1-931666-33-4"
  xmlns:encoder="xalan://java.net.URLEncoder" xmlns:exts="xalan://dk.defxws.fedoragsearch.server.GenericOperationsImpl"
  xmlns:fedora="info:fedora/fedora-system:def/relations-external#"
  xmlns:fedora-model="info:fedora/fedora-system:def/model#" xmlns:foxml="info:fedora/fedora-system:def/foxml#"
  xmlns:islandora-exts="xalan://ca.upei.roblib.DataStreamForXSLT" xmlns:mods="http://www.loc.gov/mods/v3"
  xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
  xmlns:rel="info:fedora/fedora-system:def/relations-external#"
  xmlns:res="http://www.w3.org/2001/sw/DataAccess/rf1/result"
  xmlns:sparql="http://www.w3.org/2001/sw/DataAccess/rf1/result" xmlns:tei="http://www.tei-c.org/ns/1.0"
  xmlns:uvalibadmin="http://dl.lib.virginia.edu/bin/admin/admin.dtd/"
  xmlns:uvalibdesc="http://dl.lib.virginia.edu/bin/dtd/descmeta/descmeta.dtd" xmlns:xalan="http://xml.apache.org/xalan"
  xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:zs="http://www.loc.gov/zing/srw/">
  <xsl:output encoding="UTF-8" indent="yes" method="xml"/>
  <!-- gsearch magik @TODO: see if any of the explicit variables can be replaced by these -->
  <xsl:param name="REPOSITORYNAME" select="'fedora'"/>
  <xsl:param name="FEDORASOAP" select="'http://emic.local:8080/fedora/services'"/>
  <xsl:param name="FEDORAUSER" select="'fedoraAdmin'"/>
  <xsl:param name="FEDORAPASS" select="'fedoraAdmin'"/>
  <xsl:param name="TRUSTSTOREPATH" select="'fedora'"/>
  <xsl:param name="TRUSTSTOREPASS" select="'fedoraAdmin'"/>
  <!-- These values are accessible in included xslts -->
  <xsl:variable name="PROT">http</xsl:variable>
  <xsl:variable name="HOST">localhost</xsl:variable>
  <xsl:variable name="PORT">8080</xsl:variable>
  <xsl:variable name="PID" select="/foxml:digitalObject/@PID"/>
  <!--  Used for indexing other objects. -->
  <xsl:variable name="FEDORA" select="substring($FEDORASOAP, 1,
    java_string:lastIndexOf(java_string:new(string($FEDORASOAP)), '/'))" xmlns:java_string="xalan://java.lang.String"/>
  <!--
  This xslt stylesheet generates the IndexDocument consisting of IndexFields
    from a FOXML record. The IndexFields are:
      - from the root element = PID
      - from foxml:property   = type, state, contentModel, ...
      - from oai_dc:dc        = title, creator, ...
    The IndexDocument element gets a PID attribute, which is mandatory,
    while the PID IndexField is optional.
  -->
  <!-- These includes are for transformations on individual datastreams;
     disable the ones you do not want to perform;
     the paths may need to be updated if the standard install was not followed
     TODO: look into a way to make these paths relative -->
  <!-- older gsearches (slurp_all_MODS_to_solr also contains an include that would need to be
     altered if you use these)-->
  <!--
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/DC_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/RELS-EXT_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/RELS-INT_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/FOXML_properties_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/datastream_id_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/slurp_all_MODS_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/MODS_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/EACCPF_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/TEI_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/text_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/XML_to_one_solr_field.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/XML_text_nodes_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/library/traverse-graph.xslt"/>
    -->
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/DC_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/RELS-EXT_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/RELS-INT_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/FOXML_properties_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/datastream_id_to_solr.xslt"/>
  <!--<xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/MODS_to_solr.xslt"/>-->
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/slurp_all_MODS_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/EACCPF_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/TEI_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/text_to_solr.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/XML_to_one_solr_field.xslt"/>
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/XML_text_nodes_to_solr.xslt"/>
  <!--  Used for indexing other objects. -->
  <xsl:include href="/usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms/library/traverse-graph.xslt"/>
  <!-- Decide which objects to modify the index of -->
  <xsl:template match="/">
    <update>
      <!-- The following allows only active and data oriented FedoraObjects to be indexed -->
      <xsl:if test="not(foxml:digitalObject/foxml:datastream[@ID='METHODMAP' or @ID='DS-COMPOSITE-MODEL'])">
        <xsl:choose>
          <xsl:when
            test="foxml:digitalObject/foxml:objectProperties/foxml:property[@NAME='info:fedora/fedora-system:def/model#state'
            and @VALUE='Active']">
            <add>
              <xsl:apply-templates mode="indexFedoraObject" select="/foxml:digitalObject">
                <xsl:with-param name="PID" select="$PID"/>
              </xsl:apply-templates>
            </add>
            <!-- reindex versionable object if a child of one -->
            <xsl:call-template name="indexVersionableObjectIfChildOf">
              <xsl:with-param name="PID" select="$PID"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates mode="unindexFedoraObject" select="/foxml:digitalObject"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </update>
  </xsl:template>
  <!-- Index an object -->
  <xsl:template match="/foxml:digitalObject" mode="indexFedoraObject">
    <xsl:param name="PID"/>
    <doc>
      <!-- put the object pid into a field -->
      <field name="PID">
        <xsl:value-of select="$PID"/>
      </field>
      <!-- These templates are in the /usr/local/fedora/tomcat/webapps/fedoragsearch/WEB-INF/classes/fgsconfigFinal/index/FgsIndex/islandora_transforms -->
      <xsl:apply-templates select="foxml:objectProperties/foxml:property"/>
      <xsl:apply-templates mode="index_object_datastreams" select="/foxml:digitalObject"/>
      <!-- THIS IS SPARTA!!!
        These lines call a matching template on every datastream id so that you only have to edit included files
        handles inline and managed datastreams
        The datastream level element is used for matching,
        making it imperative to use the $content parameter for xpaths in templates
        if they are to support managed datstreams -->
      <!-- TODO: would like to get rid of the need for the content param -->
      <xsl:for-each select="foxml:datastream">
        <xsl:choose>
          <xsl:when test="@CONTROL_GROUP='X'">
            <xsl:apply-templates select="foxml:datastreamVersion[last()]">
              <xsl:with-param name="content" select="foxml:datastreamVersion[last()]/foxml:xmlContent"/>
            </xsl:apply-templates>
          </xsl:when>
          <xsl:when test="@CONTROL_GROUP='M' and foxml:datastreamVersion[last()][@MIMETYPE='text/xml' or
            @MIMETYPE='application/xml' or @MIMETYPE='application/rdf+xml' or @MIMETYPE='text/html']">
            <!-- TODO: should do something about mime type filtering
              text/plain should use the getDatastreamText extension because document will only work for xml docs
              xml files should use the document function
              other mimetypes should not be being sent
              will this let us not use the content variable? -->
            <xsl:apply-templates select="foxml:datastreamVersion[last()]">
              <xsl:with-param name="content" select="document(concat($PROT, '://', encoder:encode($FEDORAUSER), ':',
                encoder:encode($FEDORAPASS), '@', $HOST, ':', $PORT, '/fedora/objects/', $PID, '/datastreams/', @ID,
                '/content'))"/>
            </xsl:apply-templates>
          </xsl:when>
          <!-- non-xml managed datastreams...

               Really, should probably only
               handle the mimetypes supported by the "getDatastreamText" call:
               https://github.com/fcrepo/gsearch/blob/master/FedoraGenericSearch/src/java/dk/defxws/fedoragsearch/server/TransformerToText.java#L185-L200
          
          <xsl:when test="@CONTROL_GROUP='M' and foxml:datastreamVersion[last() and not(starts-with(@MIMETYPE,
            'image'))]">
              TODO: should do something about mime type filtering
              text/plain should use the getDatastreamText extension because document will only work for xml docs
              xml files should use the document function
              other mimetypes should not be being sent
              will this let us not use the content variable?
            <xsl:apply-templates select="foxml:datastreamVersion[last()]">
              <xsl:with-param name="content" select="normalize-space(exts:getDatastreamText($PID, $REPOSITORYNAME, @ID,
                $FEDORASOAP, $FEDORAUSER, $FEDORAPASS, $TRUSTSTOREPATH, $TRUSTSTOREPASS))"/>
            </xsl:apply-templates>
          </xsl:when>
                    -->
        </xsl:choose>
      </xsl:for-each>
      <!-- Should only apply to Versionable Objects -->
      <xsl:call-template name="indexVersionableObjectRelatedObjectsAsFields">
        <xsl:with-param name="PID" select="$PID"/>
      </xsl:call-template>
      <!-- this is an example of using template modes to have multiple ways of indexing the same stream -->
      <!--
      <xsl:apply-templates select="foxml:datastream[@ID='EAC-CPF']/foxml:datastreamVersion[last()]/foxml:xmlContent//eaccpf:eac-cpf">
        <xsl:with-param name="pid" select="$PID"/>
      </xsl:apply-templates>

      <xsl:apply-templates mode="fjm" select="foxml:datastream[@ID='EAC-CPF']/foxml:datastreamVersion[last()]/foxml:xmlContent//eaccpf:eac-cpf">
        <xsl:with-param name="pid" select="$PID"/>
        <xsl:with-param name="suffix">_s</xsl:with-param>
      </xsl:apply-templates>
      -->
    </doc>
  </xsl:template>
  <!-- These are execptional cases that override the defaults of slurp_all_MODS_to_solr.xslt.
       Basically we want to have Genre and typeOfResource to be index as strings so they can be sorted.
       
       Object Type: mods:mods/mods:typeOfResource
       Genre: mods:mods/mods:genre
    -->
  <xsl:template match="mods:typeOfResource | mods:genre" mode="slurping_MODS" priority="2">
    <xsl:param name="prefix"/>
    <xsl:variable name="suffix" select="'s'"/>
    <xsl:call-template name="create_hierarchical_field">
      <xsl:with-param name="prefix" select="$prefix"/>
      <xsl:with-param name="suffix" select="$suffix"/>
    </xsl:call-template>
  </xsl:template>
  <!-- Creates a field that includes it's parents -->
  <xsl:template name="create_hierarchical_field">
    <xsl:param name="prefix"/>
    <xsl:param name="suffix"/>
    <xsl:variable name="this_prefix">
      <xsl:value-of select="concat($prefix, local-name(), '_')"/>
      <xsl:if test="@type">
        <xsl:value-of select="@type"/>
        <xsl:text>_</xsl:text>
      </xsl:if>
    </xsl:variable>
    <xsl:variable name="textValue">
      <xsl:value-of select="normalize-space(text())"/>
    </xsl:variable>
    <xsl:if test="$textValue">
      <field>
        <xsl:attribute name="name">
          <xsl:value-of select="concat($this_prefix, $suffix)"/>
        </xsl:attribute>
        <xsl:value-of select="$textValue"/>
      </field>
    </xsl:if>
  </xsl:template>
  <!-- Index Versionable Object: Checks if the object is a child of a Versionable Object and reindexs it. -->
  <xsl:template name="indexVersionableObjectIfChildOf">
    <xsl:param name="PID"/>
    <xsl:variable name="children">
      <xsl:call-template name="perform_traversal_query">
        <xsl:with-param name="risearch" select="concat($FEDORA, '/risearch')"/>
        <xsl:with-param name="lang">sparql</xsl:with-param>
        <xsl:with-param name="query"> 
          PREFIX fre: &lt;info:fedora/fedora-system:def/relations-external#&gt; 
          PREFIX fm: &lt;info:fedora/fedora-system:def/model#&gt; 
          PREFIX islandora: &lt;http://islandora.ca/ontology/relsext#&gt;
          SELECT ?obj FROM &lt;#ri&gt; WHERE { 
            ?obj fm:hasModel &lt;info:fedora/islandora:versionableObjectCModel&gt; .
            ?sub fre:isMemberOf ?obj . 
            FILTER(sameTerm(?sub, &lt;info:fedora/<xsl:value-of select="$PID"/>&gt;)) 
          }
        </xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:for-each select="xalan:nodeset($children)//sparql:result/sparql:obj">
      <add commitWithin="5000">
        <xsl:variable name="xml_url" select="concat(substring-before($FEDORA, '://'), '://',
          encoder:encode($FEDORAUSER), ':', encoder:encode($FEDORAPASS), '@', substring-after($FEDORA, '://') ,
          '/objects/', substring-after(@uri, '/'), '/objectXML')"/>
        <xsl:variable name="object" select="document($xml_url)"/>
        <xsl:if test="$object">
          <xsl:apply-templates mode="indexFedoraObject" select="$object/foxml:digitalObject">
            <xsl:with-param name="PID" select="$object/foxml:digitalObject/@PID"/>
          </xsl:apply-templates>
        </xsl:if>
      </add>
    </xsl:for-each>
  </xsl:template>
  <!-- Index's child objects -->
  <xsl:template name="indexVersionableObjectRelatedObjectsAsFields">
    <xsl:param name="PID"/>
    <xsl:variable name="children">
      <xsl:call-template name="perform_traversal_query">
        <xsl:with-param name="risearch" select="concat($FEDORA, '/risearch')"/>
        <xsl:with-param name="lang">sparql</xsl:with-param>
        <xsl:with-param name="query">
            PREFIX fre: &lt;info:fedora/fedora-system:def/relations-external#&gt; 
            PREFIX fm: &lt;info:fedora/fedora-system:def/model#&gt; 
            PREFIX islandora: &lt;http://islandora.ca/ontology/relsext#&gt;
            SELECT ?obj ?label ?model ?source FROM &lt;#ri&gt; WHERE { 
              ?sub fm:hasModel &lt;info:fedora/islandora:versionableObjectCModel&gt; . 
              ?obj fre:isMemberOf ?sub ; 
                   fm:label ?label ;
                   fm:hasModel ?model ; 
                   fm:state fm:Active . 
              OPTIONAL { ?obj islandora:isCriticalEditionOf ?source . }
              FILTER(sameTerm(?sub, &lt;info:fedora/<xsl:value-of select="$PID" />&gt;) &amp;&amp; 
                    !sameTerm(?model, &lt;info:fedora/fedora-system:FedoraObject-3.0&gt;))
            } 
            ORDER BY ?source
        </xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:for-each select="xalan:nodeset($children)//sparql:result">
      <xsl:choose>
        <xsl:when test="sparql:model[@uri = 'info:fedora/islandora:transcriptionCModel']">
          <field>
            <xsl:attribute name="name">transcription_ms</xsl:attribute>
            <xsl:value-of select="substring-after(sparql:obj/@uri, 'info:fedora/')"/>
          </field>
          <field>
            <xsl:attribute name="name">transcription_label_ms</xsl:attribute>
            <xsl:value-of select="sparql:label"/>
          </field>
        </xsl:when>
        <xsl:when test="sparql:model[@uri = 'info:fedora/islandora:criticalEditionCModel']">
          <xsl:if test="sparql:source[not(@bound) or @bound = true]">
            <field>
              <xsl:attribute name="name">source_ms</xsl:attribute>
              <xsl:value-of select="sparql:source"/>
            </field>
          </xsl:if>
          <field>
            <xsl:attribute name="name">tei_rdf_ms</xsl:attribute>
            <xsl:value-of select="substring-after(sparql:obj/@uri, 'info:fedora/')"/>
          </field>
          <field>
            <xsl:attribute name="name">tei_rdf_label_ms</xsl:attribute>
            <xsl:value-of select="sparql:label"/>
          </field>
        </xsl:when>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>
  <!-- Delete the solr doc of an object -->
  <xsl:template match="/foxml:digitalObject" mode="unindexFedoraObject">
    <xsl:comment> name="PID" This is a hack, because the code requires that to be present </xsl:comment>
    <delete>
      <id>
        <xsl:value-of select="$PID"/>
      </id>
    </delete>
    <!-- We must update the versionable object so that it reflects the correct existing transcriptions -->
    <xsl:variable name="solr_url" select="concat($PROT, '://', $HOST, ':', $PORT, '/solr/select?q=',
      encoder:encode(concat('transcription_ms:&quot;', $PID, '&quot;')))"/>
    <xsl:variable name="object" select="document($solr_url)"/>
    <xsl:for-each select="xalan:nodeset($object)//str[@name='PID']">
      <add commitWithin="5000">
        <xsl:variable name="xml_url" select="concat(substring-before($FEDORA, '://'), '://',
          encoder:encode($FEDORAUSER), ':', encoder:encode($FEDORAPASS), '@', substring-after($FEDORA, '://') ,
          '/objects/', self::node(), '/objectXML')"/>
        <xsl:variable name="object" select="document($xml_url)"/>
        <xsl:if test="$object">
          <xsl:apply-templates mode="indexFedoraObject" select="$object/foxml:digitalObject">
            <xsl:with-param name="PID" select="$object/foxml:digitalObject/@PID"/>
          </xsl:apply-templates>
        </xsl:if>
      </add>
    </xsl:for-each>
  </xsl:template>
  <!-- This prevents text from just being printed to the doc without field elements JUST TRY COMMENTING IT OUT -->
  <xsl:template match="text()"/>
  <xsl:template match="text()" mode="indexFedoraObject"/>
  <xsl:template match="text()" mode="unindexFedoraObject"/>
  <xsl:template match="text()" mode="index_object_datastreams"/>
</xsl:stylesheet>
