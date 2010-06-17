<?xml version="1.0" encoding="UTF-8"?>
<!--
xml2json.xsl - 0.1

Copyright (c) 2006, Keita Kitamura <keita aaaaaat keitap doooooooot com>.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
        
 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

 * Neither the name of Keita Kitamura. nor the names of its contributors
   may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:variable name="debug" select="false()" />

    <xsl:output method="text" media-type="text/javascript" />

    <!--
    this option allows you to specify a list of element names which
    should always be forced into an array representation (comma separated, ignore case)
    -->
    <xsl:variable name="force_array">item,author,creator</xsl:variable>

    <!-- callback function name -->

    <xsl:variable name="js_funcname">amz_loadComplete</xsl:variable>

    <xsl:variable name="use_normalize-space" select="true()" />
    <xsl:variable name="text_node_name">#text</xsl:variable>
    <xsl:variable name="attr_prefix">@</xsl:variable>

    <xsl:strip-space elements="*" />

    <!-- private variable -->

    <xsl:variable name="__force_array" select="concat(translate($force_array, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), ',')" />

    <xsl:template match="/">
        <xsl:choose>
            <xsl:when test="$debug = true()">
                <html>
                    <head>
                        <script type="text/javascript" charset="UTF-8" src="jkl-dumper.js"></script>
                        <style>

                            body {
                                background-color: #f5f5f5;
                                font-family: Garamond;
                            }

                            div, pre {
                                font-family: Courier New, Courier;
                            }
                        </style>
                    </head>
                    <body>
                        <h1>XML 2 JSON - Debug Mode</h1>
                        <h2>Pretty Print</h2>
                        <div id="pp">
                            Download <a href="http://www.kawa.net/works/js/jkl/dumper.html">JKL.Dumper(jkl-dumper.js)</a> to enable pretty print.
                        </div>

                        <script type="text/javascript">
                            var sp = false;
                            var d = document.getElementById('pp');
                        </script>
                        <script type="text/javascript">
                            try {
                                var j = {<xsl:apply-templates />};
                                var t = new JKL.Dumper().dump(j);
                                d.innerHTML = '&lt;pre&gt;' + t + '&lt;/pre&gt;';
                            }
                            catch (e) {
                            }
                            sp = true;
                        </script>
                        <script type="text/javascript">

                            if (!sp) {
                                d.innerHTML = '&lt;pre&gt;Failed to convert xml to json.&lt;/pre&gt;';
                            }
                        </script>
                        <h2>Raw</h2>
                        <div id="raw">
                            <xsl:value-of select="$js_funcname" />
                            <xsl:text>({</xsl:text>

                            <xsl:apply-templates />
                            <xsl:text>});</xsl:text>
                        </div>
                    </body>
                </html>
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="$js_funcname" />

                <xsl:text>({</xsl:text>
                <xsl:apply-templates />
                <xsl:text>});</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <xsl:template match="processing-instruction('xml-stylesheet')">

    </xsl:template>

    <xsl:template match="node()">
        <!-- whether the same element name as previous node or not -->
        <xsl:variable name="sap" select="name(.) = name(preceding-sibling::*[1])" />

        <!-- whether the same element name as next node or not -->
        <xsl:variable name="saf" select="name(.) = name(following-sibling::*[1])" />

        <!-- whether processing an array or not -->

        <xsl:variable name="inarr" select="$sap = true() or $saf = true()" />

        <!-- whether processing an forced array or not -->
        <xsl:variable name="infarr" select="$inarr = false() and $__force_array != ',' and contains(concat($__force_array, ','), concat(translate(name(.), 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), ','))" />

        <!-- beginning of value -->
        <xsl:if test="($sap = false() and $saf = false()) or ($sap = false() and $saf = true())">
            <xsl:call-template name="json-escape">
                <xsl:with-param name="text" select="name(.)" />
                <xsl:with-param name="value" select="false()" />

            </xsl:call-template>
            <xsl:text>:</xsl:text>
        </xsl:if>

        <!-- beginning of array -->
        <xsl:if test="$sap = false() and $saf = true() or $infarr">
            <xsl:text>[</xsl:text>
        </xsl:if>

        <xsl:choose>

            <!-- text node only -->
            <xsl:when test="count(@*) = 0 and count(child::*) = 0 and . != ''">
                <xsl:call-template name="json-escape">
                    <xsl:with-param name="text" select="." />
                    <xsl:with-param name="value" select="true()" />
                </xsl:call-template>
            </xsl:when>

            <!-- text node + attributes -->
            <xsl:when test="count(@*) != 0 and count(child::*) = 0 and . != ''">
                <xsl:text>{</xsl:text>
                <xsl:call-template name="json-escape">
                    <xsl:with-param name="text" select="$text_node_name" />
                    <xsl:with-param name="value" select="false()" />
                </xsl:call-template>
                <xsl:text>:</xsl:text>

                <xsl:call-template name="json-escape">
                    <xsl:with-param name="text" select="." />
                    <xsl:with-param name="value" select="true()" />
                </xsl:call-template>
                <xsl:text>,</xsl:text>
                <xsl:apply-templates select="@*" />
                <xsl:text>}</xsl:text>
            </xsl:when>

            <!-- attributes only -->
            <xsl:when test="count(@*) != 0 and count(child::*) = 0">
                <xsl:text>{</xsl:text>
                <xsl:apply-templates select="@*" />
                <xsl:text>}</xsl:text>
            </xsl:when>

            <!-- elements only -->

            <xsl:when test="count(@*) = 0 and count(child::*) != 0">
                <xsl:text>{</xsl:text>
                <xsl:apply-templates select="child::*" />
                <xsl:text>}</xsl:text>
            </xsl:when>

            <!-- attributes and elements -->
            <xsl:when test="count(@*) != 0 or count(child::*) != 0">

                <xsl:text>{</xsl:text>
                <xsl:apply-templates select="@*" />
                <xsl:text>,</xsl:text>
                <xsl:apply-templates select="child::*" />
                <xsl:text>}</xsl:text>
            </xsl:when>

            <xsl:otherwise>

            </xsl:otherwise>

        </xsl:choose>

        <!-- end of array -->
        <xsl:if test="$sap = true() and $saf = false() or $infarr">
            <xsl:text>]</xsl:text>
        </xsl:if>

        <!-- if not the last element, print a comma as separator -->

        <xsl:if test="position() != last()">
            <xsl:text>,</xsl:text>
        </xsl:if>

    </xsl:template>

    <xsl:template match="@*">
        <xsl:call-template name="json-escape">
            <xsl:with-param name="text" select="concat($attr_prefix, name(.))" />

            <xsl:with-param name="value" select="false()" />
        </xsl:call-template>
        <xsl:text>:</xsl:text>

        <xsl:call-template name="json-escape">
            <xsl:with-param name="text" select="." />
            <xsl:with-param name="value" select="true()" />
        </xsl:call-template>

        <xsl:if test="position() != last()">
            <xsl:text>,</xsl:text>
        </xsl:if>
    </xsl:template>

    <xsl:template match="text()">
    </xsl:template>

    <!-- escape to json string -->

    <xsl:template name="json-escape">
        <xsl:param name="text" />
        <xsl:param name="value" />

        <xsl:variable name="text2">
            <xsl:choose>
                <xsl:when test="$use_normalize-space = true()">
                    <xsl:value-of select="normalize-space($text)" />
                </xsl:when>

                <xsl:otherwise>
                    <xsl:value-of select="$text" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:choose>
            <xsl:when test="$value = true() and substring($text2, 1, 1) != '0' and $text2 = number()">
                <xsl:value-of select="$text2" />

            </xsl:when>

            <xsl:when test="$value = true() and translate($text2, 'TRUE', 'true') = 'true'">
                <xsl:text>true</xsl:text>
            </xsl:when>

            <xsl:when test="$value = true() and translate($text2, 'FALSE', 'false') = 'false'">
                <xsl:text>false</xsl:text>
            </xsl:when>

            <xsl:otherwise>
                <xsl:text>&quot;</xsl:text>

                <xsl:call-template name="replace-string">
                    <xsl:with-param name="text">
                        <xsl:call-template name="replace-string">
                            <xsl:with-param name="text" select="$text2"/>
                            <!-- to avoid a msxml bug, use a select attribute -->
                            <xsl:with-param name="replace" select="'&#xA;'"/>

                            <xsl:with-param name="with">\n</xsl:with-param>
                        </xsl:call-template>
                    </xsl:with-param>
                    <xsl:with-param name="replace">&quot;</xsl:with-param>
                    <xsl:with-param name="with">\&quot;</xsl:with-param>
                </xsl:call-template>

                <xsl:text>&quot;</xsl:text>

            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- http://www.dpawson.co.uk/xsl/sect2/replace.html#d8315e55 -->
    <xsl:template name="replace-string">
        <xsl:param name="text" />
        <xsl:param name="replace" />
        <xsl:param name="with" />

        <xsl:choose>
            <xsl:when test="contains($text, $replace)">
                <xsl:value-of select="substring-before($text, $replace)" />
                <xsl:value-of select="$with" />
                <xsl:call-template name="replace-string">
                    <xsl:with-param name="text" select="substring-after($text, $replace)" />
                    <xsl:with-param name="replace" select="$replace" />
                    <xsl:with-param name="with" select="$with" />

                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$text" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>
