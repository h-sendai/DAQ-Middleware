<?xml version = "1.0" encoding = "UTF-8" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
<html>
<head>
<title>Configuration Parameter for MLF DAQ</title>
<!--
<meta http-equiv="content-type" content="text/html;charset=shift_jis" >
<meta http-equiv="Pragma" content="no-cache" />
<meta http-equiv="Cache-Control" content="no-cache">
<meta http-equiv="Expires" content="Thu, 01 Dec 1994 16:00:00 GMT">
-->
<style type="text/css">
h1 {color: red}
body {background-color: linen}
</style>
</head>
<body>
<h1>Configuration Parameter for MLF DAQ</h1>
  <xsl:apply-templates select="configInfo" />

</body>
</html>
</xsl:template>

<xsl:template match="configInfo">
 <xsl:apply-templates select="daqGroups" />
</xsl:template>

<xsl:template match="daqGroups">
 <xsl:apply-templates select="daqGroup" />
</xsl:template>

<xsl:template match="daqGroup">
<ul>
<li>Group name: <xsl:value-of select="@gid" /></li>
 <xsl:apply-templates select="components" />
</ul>
</xsl:template>

<xsl:template match="components">
<ol>
 <xsl:apply-templates select="component" />
</ol>
</xsl:template>

<xsl:template match="component">
<li>Component name: <xsl:value-of select="@cid" /></li>
<ul>
 <xsl:apply-templates select="hostAddr" />
 <xsl:apply-templates select="hostPort" />
 <xsl:apply-templates select="instName" />
 <xsl:apply-templates select="execPath" />
 <xsl:apply-templates select="confFile" />
 <xsl:apply-templates select="startOrd" />
 <xsl:apply-templates select="inPorts" />
 <xsl:apply-templates select="outPorts" />
 <xsl:apply-templates select="params" />
</ul>
</xsl:template>

<xsl:template match="hostAddr">
<li>Run on the node : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="hostPort">
<li>Port number for : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="instName">
<li>Name of DAQ component in RT middleware : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="execPath">
<li>Process Name : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="confFile">
<li>Configuration file for RT middleware : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="startOrd">
<li>Order of DAQ command(lower value means the component can get the command faster) : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="inPorts">
<li>InPorts</li>
<ol>
 <xsl:apply-templates select="inPort" />
</ol>
</xsl:template>
<xsl:template match="inPort">
<li>Name of InPort : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="outPorts">
<li>OutPorts</li>
<ol>
 <xsl:apply-templates select="outPort" />
</ol>
</xsl:template>
<xsl:template match="outPort">
<li>Name of OutPort : <xsl:value-of select="." /></li>
</xsl:template>
<xsl:template match="params">
<li>Parameters</li>
<ol>
 <xsl:apply-templates select="param" />
</ol>
</xsl:template>
<xsl:template match="param">
<li>Parameters for ID : <xsl:value-of select="@pid" /> = <xsl:value-of select="." /></li>
</xsl:template>

</xsl:stylesheet>
