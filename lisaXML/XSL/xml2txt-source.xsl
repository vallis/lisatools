<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>
    <xsl:template match="XSIL">
        <xsl:for-each select="XSIL[@Type='SourceData']"><xsl:text>
</xsl:text>
            <xsl:for-each select="XSIL">SOURCE --- <xsl:value-of select="@Name"/><xsl:text>
</xsl:text>
                <xsl:for-each select="Param">
                    <xsl:call-template name="handle-param"/>
                </xsl:for-each>
                <xsl:text>
</xsl:text>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>

    <xsl:template name="handle-param">
		<xsl:value-of select="@Name"/>: <xsl:value-of select="normalize-space(text())"/><xsl:text>
</xsl:text>
    </xsl:template>       
</xsl:stylesheet>
