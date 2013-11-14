<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns="http://www.w3.org/1999/xhtml" xpath-default-namespace="http://www.tei-c.org/ns/1.0"
    version="2.0">
    <xsl:output method="xhtml" indent="yes"/>
    <xsl:template match="/">
        <html>
            <head>
                <link rel="stylesheet" type="text/css" href="pkp.css"/>
                <title>formatted</title>
            </head>
            <body>
                <xsl:apply-templates/>
            </body>
        </html>
    </xsl:template>


    <!-- add -->
    <xsl:template match="add">
        <xsl:apply-templates/>
    </xsl:template>

    <!-- cb -->
    <xsl:template match="//cb"/>

    <!-- corr -->
    <xsl:template match="corr">
        <xsl:apply-templates/>
    </xsl:template>

    <!-- damage -->
    <xsl:template match="damage">
        <xsl:choose>
            <xsl:when test="@degree='1'"/>
            <xsl:when test="not(@degree='1')">
                <span class="small-caps">
                    <span class="editorial">[<xsl:value-of select="@type"/>]</span>
                </span>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">Found a 'damage' element with an unexpected value for
                    'degree' attribute</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- dateline -->
    <xsl:template match="dateline"/>

    <!-- div -->
    <xsl:template match="div">
        <xsl:choose>
            <xsl:when test="@type='poem'">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td>&#160;</td>
                    </tr>
                </table>
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100"/>
                        <td>
                            <span class="bold">
                                <span class="revision"><xsl:value-of select="@xml:id"/>
                                        (<xsl:value-of select="@ana"/>) </span>
                            </span>
                        </td>
                    </tr>
                </table>
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td/>
                        <td>
                            <xsl:apply-templates/>
                        </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="not(@type='poem')">
                <xsl:apply-templates/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">Found a 'div' element with an unexpected value for
                    'type' attribute</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- emph -->
    <xsl:template match="emph">
        <span class="italic">
            <xsl:apply-templates/>
        </span>
    </xsl:template>

    <!-- epigraph -->
    <xsl:template match="epigraph">
        <br/>
        <xsl:apply-templates/>
    </xsl:template>

    <!-- gap -->
    <xsl:template match="gap">
        <span class="editorial">[<xsl:value-of select="@reason"/>]</span>
    </xsl:template>

    <!-- head -->
    <xsl:template match="head">
        <xsl:choose>

            <xsl:when test="@type!='poem'"/>
            <xsl:when
                test="@type='poem' and not(descendant::del) or not(@type='contents') and not(@type='section') and not(@type='subsection')">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100" valign="top"> </td>
                        <td valign="top">
                            <span class="bold">
                                <xsl:apply-templates/>
                            </span>
                        </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">Found a 'head' element with an unexpected value for
                    'type' attribute</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- item -->
    <xsl:template match="item"/>

    <!-- l -->
    <xsl:template match="l">
        <xsl:choose>
            <xsl:when test="descendant::del"/>
            <xsl:when test="@rend='line space' and not(@ana) and descendant::add[@ana='completion']">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100" valign="top"> &#160; </td>
                        <td valign="top"/>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="not(@part) and not(descendant::del)">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100" valign="top">
                            <xsl:if test="not(contains(@n,'/')) and (@n mod 5)=0">
                                <xsl:value-of select="@n"/>
                            </xsl:if>
                        </td>
                        <xsl:if test="not(@rend)">
                            <td valign="top">
                                <xsl:apply-templates/>
                            </td>
                        </xsl:if>
                        <xsl:if test="@rend='indent'">
                            <td width="25" valign="top"/>
                            <td valign="top">
                                <xsl:apply-templates/>
                            </td>
                        </xsl:if>
                        <xsl:if test="@rend='indent2'">
                            <td width="50" valign="top"/>
                            <td valign="top">
                                <xsl:apply-templates/>
                            </td>
                        </xsl:if>
                        <xsl:if test="@rend='indent3'">
                            <td width="75" valign="top"/>
                            <td valign="top">
                                <xsl:apply-templates/>
                            </td>
                        </xsl:if>
                        <xsl:if test="@rend='indent4'">
                            <td width="100" valign="top"/>
                            <td valign="top">
                                <xsl:apply-templates/>
                            </td>
                        </xsl:if>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="not(@part) and not(descendant::del)">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100" valign="top">
                            <xsl:if test="not(contains(@n,'/')) and (@n mod 5)=0">
                                <xsl:value-of select="@n"/>
                            </xsl:if>
                        </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="@rend='line space' and not(descendant::del)">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100" valign="top"> </td>
                        <td valign="top"> &#160; </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="@part and @part!='I'"/>
            <xsl:when test="(not(@part) and not(@rend) or @part='I') and not(descendant::del)">
                <table border="0" cellspacing="0" cellpadding="0">
                    <tr>
                        <td width="100" valign="top">
                            <xsl:if test="not(contains(@n,'/')) and (@n mod 5)=0">
                                <xsl:value-of select="@n"/>
                            </xsl:if>
                        </td>
                        <td valign="top">
                            <xsl:if test="not(@part)">
                                <xsl:apply-templates/>
                            </xsl:if>
                            <xsl:if test="@part">
                                <xsl:variable name="LineNum" select="@n"/>
                                <table border="0" cellspacing="0" cellpadding="0">
                                    <xsl:for-each
                                        select="../l[@n=$LineNum and not(descendant::del)]">
                                        <tr>
                                            <xsl:element name="td">
                                                <xsl:attribute name="colspan">
                                                  <xsl:value-of
                                                  select="count(preceding-sibling::l[@n=$LineNum])+1"
                                                  />
                                                </xsl:attribute>
                                            </xsl:element>
                                            <td>
                                                <xsl:apply-templates/>
                                            </td>
                                        </tr>
                                    </xsl:for-each>
                                </table>
                            </xsl:if>
                        </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">Found an 'l' element with an unexpected value for 'ana'
                    or 'part' or 'rend' attribute</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- lb -->
    <xsl:template match="lb"/>

    <!-- lg-->
    <xsl:template match="//lg">
        <xsl:choose>
            <!-- when line space added-->
            <xsl:when test="@ana='revised'">
                <xsl:apply-templates/>
            </xsl:when>
            <!-- when no line space added -->
            <xsl:when test="not(@ana)">
                <br/>
                <xsl:apply-templates/>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <!-- list -->
    <xsl:template match="list"/>

    <!-- note -->
    <xsl:template match="note"/>

    <!-- orig -->
    <xsl:template match="orig"/>

    <!-- pb -->
    <xsl:template match="pb"/>

    <!-- q -->
    <xsl:template match="q">
        <xsl:choose>
            <xsl:when test="@rend='dq'">"<xsl:apply-templates/>"</xsl:when>
            <xsl:when test="@rend='ldq'">"<xsl:apply-templates/></xsl:when>
            <xsl:when test="@rend='rdq'"><xsl:apply-templates/>"</xsl:when>
            <xsl:when test="@rend='sq'">'<xsl:apply-templates/>'</xsl:when>
            <xsl:when test="@rend='lsq'">'<xsl:apply-templates/></xsl:when>
            <xsl:when test="@rend='rsq'"><xsl:apply-templates/>'</xsl:when>
            <xsl:when test="@rend='notq'">
                <xsl:apply-templates/>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <!-- ref -->
    <xsl:template match="ref"/>

    <!-- reg-->
    <xsl:template match="reg">
        <xsl:choose>
            <!-- adding title -->
            <xsl:when test="@ana='titled'">[<xsl:apply-templates/>]</xsl:when>
            <!-- not adding title -->
            <xsl:when test="not(ana='titled')">
                <xsl:apply-templates/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">Found a 'reg' element with an unexpected value for
                    'ana' attribute</xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- sic -->
    <xsl:template match="sic">
        <xsl:choose>
            <xsl:when test="parent::choice"/>
            <xsl:when test="not(parent::choice)">
                <xsl:apply-templates/>
                <span class="editorial">[sic]</span>
            </xsl:when>
        </xsl:choose>
    </xsl:template>

    <!-- soCalled -->
    <xsl:template match="soCalled">"<xsl:apply-templates/>"</xsl:template>

    <!-- sp -->
    <xsl:template match="sp">
        <br/>
        <xsl:apply-templates/>
    </xsl:template>

    <!-- space -->
    <xsl:template match="space">&#160;&#160;&#160;&#160;&#160;&#160;</xsl:template>

    <!-- 1 speaker: unaltered -->
    <xsl:template match="speaker">

        <xsl:choose>
            <xsl:when test="not(descendant::del)">
                <table border="0" cellpadding="0" cellspacing="0">
                    <tr>
                        <td width="100"/>
                        <td>
                            <span class="bold">
                                <xsl:apply-templates/>
                            </span>
                        </td>
                    </tr>
                </table>
            </xsl:when>
            <xsl:when test="descendant::dell"/>
        </xsl:choose>
    </xsl:template>

    <!-- teiHeader -->
    <xsl:template match="teiHeader"/>

    <xsl:template match="title">
        <xsl:apply-templates/>
    </xsl:template>

    <!-- titlePart -->
    <xsl:template match="titlePart"/>

    <!-- trailer -->
    <xsl:template match="trailer">
        <table border="0" cellpadding="0" cellspacing="0">
            <tr>
                <td width="100" valign="top"/>
                <td valign="top">
                    <xsl:choose>
                        <xsl:when test="descendant::del"/>
                        <xsl:when test="not(descendant::del)">
                            <xsl:apply-templates/>
                        </xsl:when>
                    </xsl:choose>
                </td>
            </tr>
        </table>
    </xsl:template>

</xsl:stylesheet>