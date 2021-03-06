<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:date="http://exslt.org/dates-and-times"
  exclude-result-prefixes="date" version="1.0">
  <xsl:import 
    href="http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl" />
  <xsl:include 
  href="./../common/common.xsl"/>
  <xsl:include href="xeno-titlepage.xsl" />

  <xsl:param name="shade.verbatim" select="1" />
  <xsl:param name="use.id.as.filename" select="1" />
  <xsl:param name="chunker.output.indent" select="'yes'" />
  <xsl:param name="navig.graphics" select="1" />
  <xsl:param name="navig.graphics.extension" select="'.gif'" /> 
  <xsl:param name="generate.toc" select="'article toc,table sect1 toc'" />
  <xsl:param name="generate.section.toc.level" select="2" />

  <!-- Which element should include a TOC ? -->
  <xsl:param name="generate.toc">
    appendix  nop
    article   toc, title, figure, table, example, equation
    book      toc, title, figure, table, example, equation
    chapter   toc
    part      nop
    preface   nop
    qandadiv  nop
    qandaset  nop
    reference nop
    section   toc
    sect1     toc
    set       toc
  </xsl:param>

  <!-- Footer customization -->
  <xsl:template name="user.footer.content">
    <hr/><p class="copyright">&#x00A9; 2005 Xenomai Project</p>

    <a href="http://validator.w3.org/check/referer" 
      title="Markup validation for this page">

    <xsl:element name = "img" > 
      <xsl:attribute name = "src" >
        <xsl:value-of select ="concat($navig.graphics.path,'w3_xhtml11.png')" />
        </xsl:attribute>
        <xsl:attribute name="alt">Standard-compliant XHTML</xsl:attribute>
        <xsl:attribute name="width">80</xsl:attribute>
        <xsl:attribute name="height">15</xsl:attribute>
      </xsl:element>

    </a>

    <a href="http://jigsaw.w3.org/css-validator/check/referer"
      title="Style Sheet validation for this page">

    <xsl:element name = "img" > 
      <xsl:attribute name = "src" >
        <xsl:value-of select ="concat($navig.graphics.path, 'w3_css2.png')" />
        </xsl:attribute>
        <xsl:attribute name="alt">Standard-compliant CSS 2</xsl:attribute>
        <xsl:attribute name="width">80</xsl:attribute>
        <xsl:attribute name="height">15</xsl:attribute>
      </xsl:element> 

    </a>

  </xsl:template>

</xsl:stylesheet>
