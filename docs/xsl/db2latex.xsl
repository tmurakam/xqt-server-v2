<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>
  <xsl:import href="/usr/local/db2latex/xsl/docbook.xsl"/>

  <!-- optionally, insert your customisations here -->
  <xsl:param name="latex.documentclass.article">a4paper,10pt,twoside</xsl:param>
  <xsl:output method="text" encoding="EUC-JP" indent="yes"/>

  <xsl:variable name="latex.admonition.path">/usr/local/db2latex/latex/xsl/figures</xsl:variable>

</xsl:stylesheet>
