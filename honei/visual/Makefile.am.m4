ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`filelist', `')dnl
define(`testlist', `')dnl
define(`headerlist', `')dnl
define(`addtest', `define(`testlist', testlist `$1_TEST')dnl
$1_TEST_SOURCES = $1_TEST.cc
$1_TEST_LDADD = \
	$(top_builddir)/unittest/libunittest.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneivisual.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/swe/libhoneiswe.la \
	$(top_builddir)/honei/graph/libhoneigraph.la \
	$(DYNAMIC_LD_LIBS)
$1_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addhh', `define(`filelist', filelist `$1.hh')define(`headerlist', headerlist `$1.hh')')dnl
define(`addcc', `define(`filelist', filelist `$1.cc')')dnl
define(`addthis', `dnl
ifelse(`$2', `hh', `addhh(`$1')', `')dnl
ifelse(`$2', `cc', `addcc(`$1')', `')dnl
ifelse(`$2', `test', `addtest(`$1')', `')')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')')dnl

include(`honei/visual/files.m4')

AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in Makefile.am
EXTRA_DIST = Makefile.am.m4 files.m4

DEFS = \
	$(CELLDEF) \
	$(SSEDEF) \
	$(DEBUGDEF) \
	$(PROFILERDEF)

lib_LTLIBRARIES = libhoneivisual.la

libhoneivisual_la_SOURCES = filelist
libhoneivisual_la_LIBADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	-lGL \
	-lglut \
	-lGLU

libhoneivisual_includedir = $(includedir)/honei/visual
libhoneivisual_include_HEADERS = headerlist

TESTS = testlist
TESTS_ENVIRONMENT = env BACKENDS="$(BACKENDS)" TYPE=$(TYPE) bash $(top_builddir)/unittest/run.sh

check_PROGRAMS = $(TESTS)

Makefile.am : Makefile.am.m4 files.m4
	$(top_srcdir)/misc/do_m4.bash Makefile.am
