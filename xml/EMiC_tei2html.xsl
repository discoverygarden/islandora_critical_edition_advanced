<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet exclude-result-prefixes="xs xlink tei rdf" version="2.0" xmlns="http://www.w3.org/1999/xhtml"
  xmlns:tei="http://www.tei-c.org/ns/1.0" 
  xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
  xmlns:xlink="http://www.w3.org/1999/xlink" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="html" omit-xml-declaration="yes"/>
  <!-- Template: Root
       This table generates a html document, where the body is populated tei:text of the incoming TEI document. 
  -->
  <xsl:template match="/tei:TEI">
    <html>
      <head>TEI</head>
      <body>
        <div id="tei-rdf-header" style="display:none;">
          <xsl:apply-templates select="rdf:RDF"/>
        </div>
        <!-- TODO Decided what to do about have Text and surfaces -->
        <xsl:apply-templates select="//tei:text"/>
        <xsl:apply-templates select="//tei:surface"/>
      </body>
    </html>
  </xsl:template>
  <!-- Template: attributes
       Adds class attribute and data-* attributes from the source tag. -->
  <xsl:template name="attributes">
    <!-- Attribute's whose values should be CSS classes -->
    <xsl:variable name="attributes-as-classes" select="('rend', 'place')"/>
    <!-- Builds the classes variable -->
    <xsl:variable name="classes">
      <!-- Add the element name as a class type -->
      <xsl:value-of select="local-name(self::node())"/>
      <xsl:for-each select="@*">
        <!-- Only attributes in $attributes-as-classes can be converted to classes -->
        <xsl:if test="local-name() = $attributes-as-classes">
          <!-- Always prepend with a space -->
          <xsl:value-of select="string(' ')"/>
          <xsl:value-of select="."/>
        </xsl:if>
      </xsl:for-each>
    </xsl:variable>
    <!-- Create the classes attribute -->
    <xsl:if test="$classes != ''">
      <xsl:attribute name="class">
        <xsl:value-of select="$classes"/>
      </xsl:attribute>
    </xsl:if>
    <!-- Add data-* for non class attributes -->
    <xsl:for-each select="@*[empty(index-of($attributes-as-classes, local-name()))]">
      <xsl:attribute name="data-{local-name(.)}">
        <xsl:value-of select="."/>
      </xsl:attribute>
    </xsl:for-each>
  </xsl:template>
  <!-- Template: Text -->
  <xsl:template match="tei:text">
    <div id="tei-text">
      <xsl:apply-templates/>
    </div>
  </xsl:template>
  <!-- Template: Surface -->
  <xsl:template match="tei:surface"> 
    <div id="tei-surface">
      <div id="tei-header">
        <xsl:apply-templates mode="header" select="descendant::tei:zone[@type = 'pagination']"/>
      </div>
      <div id="tei-margin-left">
        <xsl:apply-templates mode="margin-left" select="descendant::tei:zone[@type = 'left_margin']"/>
      </div>
      <div id="tei-body">
        <xsl:apply-templates mode="body" select="descendant::tei:zone[@type = 'main']"/>
      </div>
      <div id="tei-margin-right">
        <xsl:apply-templates mode="margin-right" select="descendant::tei:zone[@type = 'right_margin']"/>
      </div>
      <div id="tei-footer">
        <xsl:apply-templates mode="footer" select="descendant::tei:zone[@type = 'footer']"/>
      </div>
    </div>
  </xsl:template>
  <!-- Template: Structural Paragraph -->
  <xsl:template match="tei:p | tei:note | tei:epigraph" mode="#all">
    <p>
      <xsl:call-template name="attributes"/>
      <xsl:apply-templates mode="#current"/>
    </p>
  </xsl:template>
  <!-- Template: Structural Unordered List -->
  <xsl:template match="tei:listBibl | tei:list" mode="#all">
    <ul>
      <xsl:call-template name="attributes"/>
      <xsl:apply-templates mode="#current"/>
    </ul>
  </xsl:template>
  <!-- Template: Structural List item -->
  <xsl:template match="tei:item | tei:bibl" mode="#all">
    <li>
      <xsl:call-template name="attributes"/>
      <xsl:apply-templates mode="#current"/>
    </li>
  </xsl:template>
  <!-- Template: Structural Line Break -->
  <xsl:template match="tei:lb" mode="#all">
    <xsl:element name="br">
      <xsl:call-template name="attributes"/>
    </xsl:element>
  </xsl:template>
  <!-- Template: line -->
  <xsl:template match="tei:zone" mode="#all">
    <div>
      <xsl:call-template name="attributes"/>
      <xsl:apply-templates mode="#current"/>
    </div>
  </xsl:template>
  <!-- Template: line -->
  <xsl:template match="tei:line" mode="#all">
    <xsl:if test="descendant::*[@place='superlinear']">      
      <div class="above-line">
        <xsl:apply-templates mode="above-line" select="descendant::*[@place='superlinear']"/>
      </div>
    </xsl:if>
    <xsl:if test="child::text() or descendant::*[(@place != 'sublinear' and @place != 'superlinear') or not(@place) ] ">
      <div class = "in-line">
        <xsl:apply-templates mode="in-line" select="child::text() | descendant::*[(@place != 'sublinear' and @place != 'superlinear') or not(@place)]"/>
      </div>  
    </xsl:if>
    <xsl:if test="descendant::*[@place='sublinear']">
      <div class="below-line">
        <xsl:apply-templates mode="line" select="descendant::*[@place = 'sublinear']"/>
      </div>  
    </xsl:if>
  </xsl:template>
  <!-- Template: * Catch all template -->
  <xsl:template match="*" mode="#all">
    <span>
      <xsl:call-template name="attributes"/>
      <xsl:apply-templates mode="#current"/>
    </span>
  </xsl:template>
  <xsl:template match="*" mode="in-line">
    <span>
      <xsl:call-template name="attributes"/>
      <xsl:if test="self::node()[(@place != 'sublinear' and @place != 'superlinear') or not(@place)]">
        <xsl:apply-templates mode="#current"/>
      </xsl:if>
      
    </span>
  </xsl:template>
  <xsl:template match="text()" mode="#all">
    <xsl:copy-of select="."></xsl:copy-of>
  </xsl:template>

</xsl:stylesheet>
