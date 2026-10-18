<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:param name="elemname" /><!-- i.e. 'added_symbols' or 'removed_symbols -->
<xsl:param name="kind" /><!-- i.e. 'source' or 'binary' -->
<xsl:output method="text"/>
<xsl:template match="/">
<!-- Source symbols added: -->
<xsl:apply-templates select="/reports/report[@kind = $kind]/*[name()=$elemname]/header/library/name"/>
</xsl:template>
<xsl:template match="name">
<xsl:value-of select="substring-before(concat(., '@'), '@')"/><xsl:text>&#xa;</xsl:text>
</xsl:template>
</xsl:stylesheet>
