DOCNAME		= devel

XMLFILE		= $(DOCNAME).xml
XMLFILE_UTF8	= $(DOCNAME).utf8.xml
#XSLFILE		= ../xsl/docbook-xsl-1.66.1/html/tldp-one-page.xsl
XSLFILE		= ../xsl/main-html.xsl

all:	html

$(XMLFILE_UTF8):	$(XMLFILE)
	iconv -f EUC-JP -t UTF-8 $< > $@

html: $(XMLFILE_UTF8)
	xmlto html -x $(XSLFILE) -o $(DOCNAME) $(XMLFILE_UTF8)
#	xsltproc -o test.html $(XSLFILE) $(XMLFILE_UTF8)
	mkdir -p $(DOCNAME)/images
	cp ../images/*.png $(DOCNAME)/images
	cp ../css/xqt.css $(DOCNAME)

pdf: $(XMLFILE_UTF8)
	xmlto pdf -x ../xsl/main-pdf.xsl $(XMLFILE_UTF8)

clean:
	-/bin/rm -rf *.html *.pdf $(DOCNAME) $(XMLFILE_UTF8)
