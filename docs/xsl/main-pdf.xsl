<!-- created by Tammy Fox tfox@redhat.com for the Fedora Project -->
<!-- License: GPL -->
<!-- Copyright 2003 Tammy Fox, Red Hat, Inc. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.w3.org/TR/xhtml1/transitional"
                exclude-result-prefixes="#default">

<xsl:import href="redhat.xsl"/>
<xsl:import href="/usr/share/sgml/docbook/xsl-stylesheets/fo/docbook.xsl"/>

<xsl:param name="use.extensions" select="0"/>
<xsl:param name="tablecolumns.extensions" select="0"/>

<xsl:param name="generate.toc">
article toc
</xsl:param>
<xsl:param name="toc.section.depth">3</xsl:param>
<xsl:param name="section.autolabel" select="1"/>

</xsl:stylesheet>
