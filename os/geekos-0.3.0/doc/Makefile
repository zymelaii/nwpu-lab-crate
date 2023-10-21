# The manual is written in DocBook 
# (see http://www.docbook.org/tdg/en/html/docbook.html)
# It uses xsltproc to convert the SGML using two style sheets:
#   conversion to HTML
#   conversion to latex for constructing PDF
# These are available, respectively, at
#   http://docbook.sourceforge.net/projects/xsl/
#   http://db2latex.sourceforge.net/      
# You can create a Makefile.local to override the locations of
#   these stylesheets defined in the variables below (the _BASE
#   variables don't matter, only the _XSL_* ones).

DOCBOOK_BASE := /export/home/daveho/linux/docbook/docbook-xsl-1.64.1
DOCBOOK_XSL_HTML := $(DOCBOOK_BASE)/html/chunk.xsl

DB2LATEX_HOME := /export/home/daveho/linux/docbook/db2latex-xsl-0.8pre1
DB2LATEX_XSL := $(DB2LATEX_HOME)/xsl/docbook.xsl

-include Makefile.local

CLASSPATH := $(SAXON_HOME)/saxon.jar:$$CLASSPATH

all : index.html hacking.pdf

index.html : hacking.xml hacking-html.xsl
	xsltproc -o $@ hacking-html.xsl hacking.xml

hacking-html.xsl : hacking-html.xsl.in
	perl -n -e 's,\@HTML_XSL_STYLESHEET\@,$(DOCBOOK_XSL_HTML),g;print' hacking-html.xsl.in > $@

hacking.pdf : hacking.tex
	pdflatex hacking.tex
	pdflatex hacking.tex

hacking.tex : hacking.xml hacking-db2latex.xsl
	xsltproc -o $@ hacking-db2latex.xsl hacking.xml

hacking-db2latex.xsl : hacking-db2latex.xsl.in
	perl -n -e 's,\@DB2LATEX_XSL_STYLESHEET\@,$(DB2LATEX_XSL),g;print' hacking-db2latex.xsl.in > $@

clean :
	rm -f hacking.{aux,glo,idx,log,out,pdf,tex} *.xsl
	rm -f apiref.html hacking101.html introproject.html projectoverview.html vmproject.html \
		index.html  ipcproject.html schedulingproject.html \
		fsproject.html intro.html  overview.html usermodeproject.html elfparsingproject.html

