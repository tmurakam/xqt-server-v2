DOCNAME		= devel

XMLFILE		= $(DOCNAME).xml

XMLFILE_UTF8	= $(DOCNAME).utf8.xml

all:	html

$(XMLFILE_UTF8):	$(XMLFILE)
	iconv -f EUC-JP -t UTF-8 $< > $@

html: $(XMLFILE_UTF8)
	xmlto html -x ../xsl/main-html.xsl $(XMLFILE_UTF8)
	mkdir -p $(DOCNAME)/images
	cp ../images/*.png $(DOCNAME)/images
	cp ../css/xqt.css $(DOCNAME)

pdf: $(XMLFILE_UTF8)
	xmlto pdf -x ../xsl/main-pdf.xsl $(XMLFILE_UTF8)

clean:
	-/bin/rm -rf *.html *.pdf $(XMLFILE_UTF8)
