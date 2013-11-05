<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns="http://www.w3.org/1999/xhtml"
    xpath-default-namespace="http://www.tei-c.org/ns/1.0"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:jmp="http://www.joshpollock.com"
    xmlns:svg="http://www.w3.org/2000/svg"
    xmlns:xlink="http://www.w3.org/1999/xlink" 
    exclude-result-prefixes="xs jmp"
    version="2.0">
    <xsl:output method="xhtml" indent="yes"/>

    <!-- Whitespace handling -->

    <xsl:variable name="Space" select="' '"/>

    <xsl:function name="jmp:RemoveTextIndent">
        <xsl:param name="Text"></xsl:param>
        <xsl:value-of select="replace($Text, '&#x0A; +', '')"/>
        <!--<xsl:for-each select="tokenize($Text, '&#x0A;')">
            <xsl:value-of select="replace(., '^ +', '')"/>
        </xsl:for-each>-->
    </xsl:function>
    
    <xsl:template match="//text()" priority="1">
        <xsl:value-of select="jmp:RemoveTextIndent(.)"/>
    </xsl:template>
    
    <xsl:template match="//seg[@type='revSession']/text()">
        <xsl:if test="normalize-space(.)!=''">
            <xsl:message terminate="yes">Error: A revSession must not contain text.</xsl:message>
        </xsl:if>
    </xsl:template>
    
    <xsl:template match="//choice/text()[normalize-space(.)!='']">
        <xsl:message terminate="yes">Error: found non-whitespace text until a revSession.</xsl:message>
    </xsl:template>
    
<!--    <xsl:template match="//seg/node()[1][normalize-space(.)='']"/>
    <xsl:template match="//seg/node()[last()][normalize-space(.)='']"/>
-->    
    <!-- Error checking -->
        
    <xsl:template match="seg[@type='revList']//seg[@type='revList']" priority="1">
        <xsl:message terminate="yes">Error: Nested revLists are not permitted.</xsl:message>
    </xsl:template>
    
    <xsl:template match="seg[@type='revList' and not(.//seg[@type='revSession'])]">
        <xsl:message terminate="yes">Error: A revList must contain a revSession.</xsl:message>
    </xsl:template>
        
    <xsl:template match="seg[@type='revSession' and not(add|del)]">
        <xsl:message terminate="yes">Error: A revSession must have an add and/or del child.</xsl:message>
    </xsl:template>
    
    <xsl:template match="seg[@type='revSession' and (count(add)>1 or count(del)>1)]">
        <xsl:message terminate="yes">Error: A revSession must not have more than one add child, or more than one del child.</xsl:message>
    </xsl:template>    
    
    <!--<xsl:template match="seg[@type='revSession' and (count(add)+count(del)=count(node()))]" priority="1">
        <xsl:message terminate="yes">Error: A revSession must not have children other than an add and/or a del.</xsl:message>
    </xsl:template>-->    

    <xsl:template match="seg[@type='revSession' and @n and not(matches(@n, '^[A-Z][1-9][0-9]*$'))]" priority="1">
        <xsl:message terminate="yes">Error: A layer label (@n) must consist a single uppercase letter followed by a number.</xsl:message>
    </xsl:template>    

    <xsl:variable name="ImplicitFirstLayer" select="'A1'"/>
    <xsl:variable name="ImplicitFirstSession" select="substring($ImplicitFirstLayer, 1, 1)"/>
    
    <xsl:variable name="GlobalLayers" as="xs:string *">
        <xsl:value-of select="$ImplicitFirstLayer"/>
        <xsl:for-each-group select="//seg[@type='revSession']" group-by="@n">
            <xsl:sort select="@n"/>
            <xsl:value-of select="string(@n)"/>
        </xsl:for-each-group>
    </xsl:variable>

    <xsl:variable name="GlobalSessions" as="xs:string *">
        <xsl:for-each-group select="$GlobalLayers" group-by="substring(., 1, 1)">
            <xsl:value-of select="substring(., 1, 1)"/>
        </xsl:for-each-group>
    </xsl:variable>

    <xsl:function name="jmp:ClassesFromLayerList" as="xs:string*">
        <xsl:param name="Layers"/>
        <xsl:param name="Inline"/>
        <xsl:for-each select="$Layers">
            <xsl:sequence select="concat(if ($Inline) then 'layerVisibilityInline' else 'layerVisibiliy', ., if (position()!=last()) then ' ' else '')"/>
        </xsl:for-each>
    </xsl:function>
    
    <xsl:variable name="SessionColors" as="xs:string*">
        <xsl:sequence select="'#FFBFBF'"/> <!-- red --> 
        <xsl:sequence select="'#FFBF7F'"/> <!-- orange --> 
        <xsl:sequence select="'#FFFF7F'"/> <!-- yellow -->
        <xsl:sequence select="'#7FFF7F'"/> <!-- green -->
        <xsl:sequence select="'#BFBFFF'"/> <!-- blue -->
        <xsl:sequence select="'#CF8FFF'"/> <!-- purple -->
        <xsl:sequence select="'#CFCFCF'"/> <!-- gray -->
        <xsl:sequence select="'#AFAFAF'"/> <!-- gray -->
    </xsl:variable>
    
    <xsl:variable name="LayerColors" as="xs:string*">
        <xsl:for-each-group select="$GlobalLayers" group-by="substring(., 1, 1)">
            <xsl:variable name="SessionIndex" select="number(position())"/> 
            <xsl:for-each select="current-group()">
                <xsl:sequence select="$SessionColors[$SessionIndex]"/>
            </xsl:for-each>
        </xsl:for-each-group>            
    </xsl:variable>
    
    <xsl:template match="/">
        <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html&gt;</xsl:text>
        <html style="height:100%;width:100%;overflow:hidden">
            
            <head>
                
                <link rel="stylesheet" type="text/css" href="pkp.css"/>
                <title>Revisions</title>
                
                <style type="text/css">
                    body                            { font-family:calibri }
                    .bracketNormal, .bracketTypo    { display:none; font-weight:bold }
                    .bracketNormal                  { color:#DD0000 }
                    .bracketTypo                    { color:blue }
                    .layerVisibility                { display:none }
                    .bracketDel, .bracketAdd        { display:none }
                    input[type=button]              { border:none }
                </style>
                
                <style type="text/css" id="LayerIndicator">
                    .layerIndicator   { color:black; font-weight:bold; vertical-align:top; text-align:center }
                </style>

                <style type="text/css" id="LayerVisibility">
                    <xsl:for-each select="1 to count($GlobalLayers)">
                        <xsl:variable name="i" select="."/>
                        .layerVisibility<xsl:value-of select="$GlobalLayers[$i]"/> { }
                    </xsl:for-each>
                </style>
                
                <style type="text/css" id="LayerVisibilityInline">
                    <xsl:for-each select="1 to count($GlobalLayers)">
                        <xsl:variable name="i" select="."/>
                        .layerVisibilityInline<xsl:value-of select="$GlobalLayers[$i]"/> { }
                    </xsl:for-each>
                </style>
                
                <style type="text/css" id="SessionColor">
                    <xsl:for-each select="1 to count($GlobalSessions)">
                        <xsl:variable name="i" select="."/>
                        .session<xsl:value-of select="$GlobalSessions[$i]"/>Color { background-color:<xsl:value-of select="$SessionColors[$i]"/> }
                    </xsl:for-each>
                </style>

                <style type="text/css" id="SessionHeaderColor">
                    <xsl:for-each select="1 to count($GlobalSessions)">
                        <xsl:variable name="i" select="."/>
                        .sessionHeader<xsl:value-of select="$GlobalSessions[$i]"/>Color { background-color:<xsl:value-of select="$SessionColors[$i]"/> }
                    </xsl:for-each>
                </style>
                
                <style type="text/css" id="AllChangesMode">
                    .allChangesModeOn { display:none }
                    .allChangesModeOff { display:inline }
                </style>
                
                <style type="text/css" id="ReadingMode">
                    .readingModeOn { display:none }
                    .readingModeOff { display:inline }
                </style>
                
                <style type="text/css" id="FinalMode">
                    .finalModeOn { display:none }
                    .finalModeOff { display:inline }
                </style>
                               
                <style type="text/css" id="Highlights">
                </style>               

                <style type="text/css" id="LineBreak">
                    div.lineBreak { display:inline }
                </style>
                
                <style type="text/css" id="MultiLayer">
                    .layerContainer  { display: block; margin-left:35px; spacing:0; text-indent:-35px; padding:0 }
                    .layerLabel      { display: inline-block; margin:0px; text-indent:0px; width:31px; text-align:center }
                    .layerSpacer     { display: inline; margin:0px; text-indent:0px; width:5px; text-align:center; background-color:white }
                    .layerContents   { display: inline; margin:0px; text-indent:0px }
                </style>
                
                <style type="text/css" id="SingleLayer">
                    .layerContainer  { display: inline }
                    .layerLabel      { display: none }
                    .layerSpacer     { display: none }
                    .layerContents   { display: inline }
                </style>
                
                <script>

                    var layers = [
                    <xsl:for-each select="$GlobalLayers">
                        "<xsl:value-of select="."/>"
                        <xsl:if test="position()!=last()">,</xsl:if>
                    </xsl:for-each>
                    ];
                    
                    var sessions = [
                    <xsl:for-each select="$GlobalSessions">
                        "<xsl:value-of select="."/>"
                        <xsl:if test="position()!=last()">,</xsl:if>
                    </xsl:for-each>
                    ];
                                                            
                </script>

                <link rel="stylesheet" href="slider.css"/>
                <script type="text/javascript" src="slider.js"/>
                <script type="text/javascript" src="tei.js"/>
                
            </head>
            <body onkeydown="OnKeyDown(event)" style="height:100%;width:100%;overflow:hidden">

                <xsl:attribute name="onload">
                                        
                    <xsl:text>CreateSlider(document.getElementById('slider'), [</xsl:text>

                    <xsl:for-each select="1 to count($GlobalSessions)">
                        <xsl:variable name="i" select="."/>
                        <xsl:text>{label:'</xsl:text>
                        <xsl:value-of select="$GlobalSessions[$i]"/>
                        <xsl:text>', color:'</xsl:text>
                        <xsl:value-of select="$SessionColors[$i]"/>
                        <xsl:text>'}</xsl:text>
                        <xsl:if test="position()!=last()">,</xsl:if>
                    </xsl:for-each>
                    
                    <xsl:text>]); ChangeRenderModeCore(document.getElementById('changesMode')); document.getElementById('MainContent').style.visibility = 'visible';</xsl:text>
                                        
                </xsl:attribute>

               <div style="width:100%;height:170px;padding:0;spacing:0;margin:0">
                    <br/>
                    <br/>
                    <div id="slider"/>
                    <br/>
                    <input type="button" id="changesMode" value="All Changes" onclick="ChangeRenderMode(event)" style="padding:5px; width:130px"/>
                    <input type="button" id="revisionsMode" value="Revisions Only" onclick="ChangeRenderMode(event)" style="padding:5px; width:130px"/>
                    <input type="button" id="finalMode" value="Final" onclick="ChangeRenderMode(event)" style="padding:5px; width:130px"/>
                    <input type="button" id="readingMode" value="Reading" onclick="ChangeRenderMode(event)" style="padding:5px; width:130px"/>                            
                </div>
                
                <div style="width:100%;height:calc(100vh - 170px);display:inline-block;overflow:hidden;border-style:border-width:0solid;padding:0;spacing:0;margin:0">
                    <div id="MainContent" style="visibility:hidden;width:45%;height:100%;display:inline-block;overflow:auto;border-style:border-width:0;solid;padding:0;spacing:0;margin:0">
                        <xsl:apply-templates/>
                    </div>
                    <div style="width:45%;height:100%;display:inline-block;overflow:hidden;border-style:solid;border-width:0;padding:0;spacing:0;margin:0">
                        <svg:svg version="1.1" style="height:100%;width:100%" preserveAspectRatio="none" viewBox="0, 0, 1000, 1000">
                            <svg:image x="0" y="0" height="100%" width="100%" xlink:href="image.jpg" preserveAspectRatio="none"/>
                            <xsl:for-each select="document('polygons.xml')//jmp:polygon">
                                <svg:path fill="none" stroke-width="3" style="visibility:hidden" >
                                    <xsl:attribute name="id" select="@id"/>
                                    <xsl:attribute name="d" select="@path"/>
                                </svg:path>
                            </xsl:for-each>
                            <!--<svg:path id="idFoo" fill="none" stroke="red" stroke-width="3" style="visibility:hidden" d="M  190.3968 107.5457 L  190.3968 164.0972 L  271.9954 164.0972 L  271.9954 107.5457 L  190.3968 107.5457  z"/>-->
                            
                            
                        </svg:svg>
                    </div>
                </div>
                
            </body>
            
        </html>
    </xsl:template>
    
    <!-- body -->
    <xsl:template match="body">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:function name="jmp:GenerateLayerList" as="xs:string*">
        <xsl:param name="Layers"/>
        <xsl:param name="ThisLayer"/>
        <xsl:variable name="NextLayer" select="$Layers[. &gt; $ThisLayer][1]"/>
        <xsl:sequence select="$GlobalLayers[. &gt;= $ThisLayer and not(. &gt;= $NextLayer)]"/>
    </xsl:function>

    
    <xsl:template match="seg[@type='revList']">
        
        <xsl:variable name="RevList" select="."/>
        
        <xsl:variable name="Layers" as="xs:string*">
            <xsl:value-of select="$ImplicitFirstLayer"/>
            <xsl:for-each-group select=".//@n" group-by=".">
                <xsl:sort select="."/>
                <xsl:value-of select="."/>
            </xsl:for-each-group>
        </xsl:variable>
        
        <xsl:for-each select="$Layers">
            
            <xsl:variable name="ThisLayer" select="."/>
            <xsl:variable name="ThisSession" select="substring($ThisLayer,1,1)"/>
                        
            <span class="{string-join(('layerVisibility', jmp:ClassesFromLayerList(jmp:GenerateLayerList($Layers,$ThisLayer), true())), ' ')}">

                <span class="layerContainer">
                                
                    <span class="sessionHeader{$ThisSession}Color layerLabel">
                        <xsl:value-of select="if (count($Layers[substring(.,1,1)=$ThisSession])>1) then $ThisLayer else $ThisSession"/>
                    </span>
                    
                    <span class="layerSpacer">&#160;</span>
                    
                    <span class="session{$ThisSession}Color layerContents">
                        <xsl:apply-templates select="$RevList/(*|text())">
                            <xsl:with-param name="ThisLayer" select="$ThisLayer" tunnel="yes"/>
                            <xsl:with-param name="Layers" select="$Layers" tunnel="yes"/>
                        </xsl:apply-templates>
                    </span>
    
                </span>
                
            </span>
            
        </xsl:for-each>
        
    </xsl:template>

    <xsl:template match="seg[@type='revSession']">
        
        <xsl:param name="ThisLayer" tunnel="yes"/>
        <xsl:param name="Layers" tunnel="yes"/>

        <xsl:variable name="NextLayer_" select="$Layers[. &gt; $ThisLayer][1]"/>
        <xsl:variable name="PrevLayer_" select="$Layers[. &lt; $ThisLayer][last()]"/>
        <xsl:variable name="IsCompletion" select="not(@n)"/>

        <xsl:variable name="NextLayer" select="if ($IsCompletion) then $ThisLayer else $NextLayer_"/>
        <xsl:variable name="PrevLayer" select="if ($IsCompletion) then $ThisLayer else $PrevLayer_"/>
        
        <xsl:variable name="Layer">
            <xsl:choose>
                <xsl:when test="@n"><xsl:value-of select="@n"/></xsl:when>
                <xsl:otherwise><xsl:value-of select="if (ancestor-or-self::add[../@n]/../@n) then (ancestor-or-self::add[../@n]/../@n)[last()] else $ImplicitFirstLayer"/></xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="DelBracketClass" as="xs:string*">
            <xsl:value-of select="if ($IsCompletion) then 'bracketTypo' else 'bracketNormal'"></xsl:value-of>
            <xsl:value-of select="jmp:ClassesFromLayerList(jmp:GenerateLayerList($Layers, $NextLayer), true())"/>
        </xsl:variable>

        <xsl:variable name="AddBracketClass" as="xs:string*">
            <xsl:value-of select="if ($IsCompletion) then 'bracketTypo' else 'bracketNormal'"></xsl:value-of>
            <xsl:value-of select="jmp:ClassesFromLayerList(jmp:GenerateLayerList($Layers, $PrevLayer), true())"/>
        </xsl:variable>
        
        <xsl:variable name="ShowDelBracket" select="$Layer = $NextLayer"/>
        <xsl:variable name="ShowDelContents" select="$Layer &gt;= $NextLayer"/>
        
        <xsl:variable name="ShowAddBracket" select="$Layer = $ThisLayer"/>        
        <xsl:variable name="ShowAddContents" select="$Layer &lt;= $ThisLayer"/>
        
        <xsl:variable name="FinalClass" select="if ($IsCompletion) then 'allChangesModeOn' else ''"/>
        
        <span class="highlight_{generate-id()}" onmouseover="OnMouseOver(event.target ? event.target : this)" onmouseout="OnMouseOut(event.target ? event.target : this)" style="cursor:default">

            <span class="{$FinalClass}">
                <xsl:if test="$ShowDelBracket">
                    <span class="{$DelBracketClass}">&lt;</span>
                </xsl:if>
                <xsl:if test="$ShowDelContents">
                    <xsl:if test="normalize-space(del)=''">
                        <span class="{$DelBracketClass}">~</span>
                    </xsl:if>
                    <xsl:apply-templates select="del"/>
                </xsl:if>        
                <xsl:if test="$ShowDelBracket">
                    <span class="{$DelBracketClass}">&gt;</span>
                </xsl:if>
            </span>
            
            <span>
                
                <xsl:if test="add/@source">
                    <xsl:attribute name="onmouseover">
                        HighlightImage(event.target ? event.target : this, '<xsl:value-of select="add/@source"/>', <xsl:value-of select="$IsCompletion"/>)
                    </xsl:attribute>
                    <xsl:attribute name="onmouseout">
                        UnhighlightImage(event.target ? event.target : this, '<xsl:value-of select="add/@source"/>')
                    </xsl:attribute>
                </xsl:if>
                
                <xsl:if test="$ShowAddBracket">
                    <span class="{$AddBracketClass} {$FinalClass}">[</span>
                </xsl:if>
                <xsl:if test="$ShowAddContents">
                    <xsl:if test="normalize-space(add)=''">
                        <span class="{$AddBracketClass}">~</span>
                    </xsl:if>
                    <xsl:apply-templates select="add"/>
                </xsl:if>                
                <xsl:if test="$ShowAddBracket">
                    <span class="{$AddBracketClass} {$FinalClass}">]</span>
                </xsl:if>
            </span>
            
        </span>
        
    </xsl:template>
    
    <!-- div -->
    <xsl:template match="//div">
        <xsl:apply-templates/>
    </xsl:template>

    <!-- l -->
    <xsl:template match="//l">
        <xsl:apply-templates/><br/>
    </xsl:template>
    
    <!-- lb -->
    <xsl:template match="//lb">
        <xsl:if test="not(ends-with(normalize-space(preceding-sibling::text()[1]), '-'))"><xsl:value-of select="$Space"/></xsl:if>
        <span class="readingModeOff">
            <span style="color:blue">/</span>
            <br/>
        </span>
    </xsl:template>

    <!-- lg -->
    <xsl:template match="//lg">
        <xsl:apply-templates/>
        <div>&#160;</div>
    </xsl:template>
       
    <!-- milestone -->
    <xsl:template match="milestone|lg">
        <xsl:value-of select="@rend"/>
        <div>&#160;</div>
    </xsl:template>

    <!-- p -->
    <xsl:template match="p|head">

        <div class="Measure" id="{generate-id()}">
            <xsl:apply-templates/>
        </div>
        <div id="newline_{generate-id()}">&#160;</div>
        
    </xsl:template>

<!--    <xsl:template match="text()[not(ancestor::seg)]">
        <xsl:if test="normalize-space(.)!=''">
            <xsl:value-of select="."/>
        </xsl:if>
    </xsl:template>
-->    
    <xsl:template match="choice">
        <xsl:apply-templates/>
    </xsl:template>
    
    <xsl:template match="corr|reg">
        <span class="readingModeOn">
            <xsl:apply-templates/>
        </span>
    </xsl:template>
            
    <xsl:template match="sic|orig">
        <span class="readingModeOff">
            <xsl:apply-templates/>
            <xsl:if test="local-name()='sic' and not(@ana='illegible')">
                <span class="editorial" style="font-variant:small-caps;color:gray">[sic]</span>
            </xsl:if>
        </span>
    </xsl:template>
    
    <!-- teiHeader -->
    <xsl:template match="//teiHeader"/>

</xsl:stylesheet>
