CLEANFILES = *~ cleanauthors cleanauthorstex cleancontributors *.out *.log *.aux *.toc *.pdf

MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = pg512.css doxygen.conf authors.html.skel licence.html.skel footer.html header.html header.tex.in \
	     powered-by-c++.png powered-by-vim.png bee_270.png bee.svg

doxygen: doxygen.conf htmlpages texfiles
	doxygen $(top_srcdir)/doc/doxygen.conf
	cp powered-by-*.png $(top_srcdir)/doc/html/

HTMLPAGES = \
	authors.html \
	licence.html

htmlpages: $(HTMLPAGES)
	mkdir -p $(srcdir)/html/
	for f in $(HTMLPAGES) pg512.css ; do cp $(srcdir)/$$f $(srcdir)/html/ ; done

cleanauthors: $(top_srcdir)/AUTHORS
	sed -e '1,3d' \
	    -e '/^Contributions/,$$d' \
	    -e 's,&,\&amp;,g' -e 's,<,\&lt;,g' -e 's,>,\&gt;,g' -e 's,@, at ,g' \
	    -e 's,^    \(.*\),<dd>\1</dd>,' \
	    -e 's,^\([^ <].*\),<dt>\1</dt>,' \
	    < ../AUTHORS > cleanauthors

cleancontributors: $(top_srcdir)/AUTHORS
	sed -e '1,/^Contributions/d' \
	    -e '/^=*$$/d' \
	    -e 's,&,\&amp;,g' -e 's,<,\&lt;,g' -e 's,>,\&gt;,g' -e 's,@, at ,g' \
	    -e 's,^    \(.*\),<dd>\1</dd>,' \
	    -e 's,^\([^ <].*\),<dt>\1</dt>,' \
	    < ../AUTHORS > cleancontributors

authors.html: authors.html.skel cleanauthors cleancontributors header.html footer.html
	sed -e '/@AUTHORS@/r cleanauthors' -e '/@AUTHORS@/d' \
	    -e '/@CONTRIBUTORS@/r cleancontributors' -e '/@CONTRIBUTORS@/d' \
	    -e '/@HEADER@/r header.html' \
	    -e '/@HEADER@/d' \
	    -e '/@FOOTER@/r footer.html' \
	    -e '/@FOOTER@/d' \
	     < $(srcdir)/authors.html.skel > authors.html

licence.html: licence.html.skel ../COPYING header.html footer.html
	sed -e '/@HEADER@/r header.html' \
	    -e '/@HEADER@/d' \
	    -e '/@FOOTER@/r footer.html' \
	    -e '/@FOOTER@/d' \
	    -e '/@COPYING@/r ../COPYING' \
	    -e '/@COPYING@/d' \
	    < $(srcdir)/licence.html.skel > licence.html

cleanauthorstex: $(top_srcdir)/AUTHORS
	sed -e '1,3d' \
	    -e '/^Contributions/,$$d' \
	    -e 's,<,$$\\langle$$,g' -e 's,>,$$\\rangle$$,g' -e 's,@, at ,g' \
	    -e '/^    \(.*\)/d' \
	    -e 's,^\([^ <].*\),{\\large \1}\\\\,' \
	    < $(top_srcdir)/AUTHORS > cleanauthorstex

TEXFILES = \
	header.tex

texfiles: $(TEXFILES)

header.tex: header.tex.in cleanauthorstex
	sed -e '/@AUTHORS@/r cleanauthorstex' -e '/@AUTHORS@/d' \
	    -e 's/@VERSION@/Prerelease/' \
	    < $(srcdir)/header.tex.in > header.tex

pdf: tutorial.tex
	pdflatex tutorial.tex
	pdflatex tutorial.tex
	pdflatex grid.tex
	pdflatex grid.tex
