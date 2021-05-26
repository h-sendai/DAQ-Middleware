<?xml version="1.0" ?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
<xsl:output method="html" encoding="iso-8859-1"/>

<xsl:template match="/response">
<html>
<body>
<ul>
<xsl:apply-templates select="methodName"/>
<xsl:apply-templates select="returnValue/result/status"/>
</ul>
</body>
</html>
</xsl:template>

<xsl:template match="methodName">
<li>Method Name: <xsl:value-of select="."/></li>
</xsl:template>

<xsl:template match="returnValue/result/status">
<li>Status: <xsl:value-of select="."/></li>
</xsl:template>

</xsl:stylesheet>
