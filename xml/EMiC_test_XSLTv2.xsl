<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet exclude-result-prefixes="xs xlink" version="2.0" xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xlink="http://www.w3.org/1999/xlink" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:TEI="http://www.tei-c.org/ns/1.0"
  xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <xsl:output doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" method="xml" indent="yes"/>
  <xsl:template match="/TEI:TEI">
    <html>
      <head>
        <title></title>
      </head>
      <body>
        <div id="tei-header" style="display:none;">
          <xsl:apply-templates select="rdf:RDF"/>
        </div>
        <xsl:apply-templates select="TEI:text"/>
      </body>
    </html>
  </xsl:template>
  <!-- match patterns good? -->
  <!-- playing with choose -->
  <xsl:template match="*">
    <xsl:choose>
      <xsl:when test="local-name() = 'p'">
        <p>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </p>
      </xsl:when>
      <xsl:when test="local-name() = 'note'">
        <p>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </p>
      </xsl:when>
      <xsl:when test="local-name() = 'epigraph'">
        <p>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </p>
      </xsl:when>
      <xsl:when test="local-name() = 'listBibl'">
        <ul>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </ul>
      </xsl:when>
      <xsl:when test="local-name() = 'list'">
        <ul>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </ul>
      </xsl:when>
      <xsl:when test="local-name() = 'item'">
        <li>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </li>
      </xsl:when>
      <xsl:when test="local-name() = 'bibl'">
        <li>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </li>
      </xsl:when>
      <xsl:when test="local-name() = 'lb'">
        <br>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </br>
      </xsl:when>
      <xsl:otherwise>
        <span>
          <xsl:call-template name="attributes"/>
          <xsl:apply-templates/>
        </span>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <!-- Create a named template to deal with attributes, called repeatedly above  -->
  <xsl:template name="attributes">
    <!-- Attribute's who's values should be CSS classes -->
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
  

  <!--structural element templates
        
column. @n of TEI element cb determines the number of columns, 
    so we'll have to create the number of col that n specifies? 
  <xsl:template match="cb">
    <colgroup>
        <xsl:apply-templates/>
    </colgroup>
  </xsl:template>
  
  
  -->

    
    

</xsl:stylesheet>
