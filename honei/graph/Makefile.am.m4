ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`filelist', `')dnl
define(`testlist', `')dnl
define(`headerlist', `')dnl
define(`benchmarklist', `')dnl
define(`sselist', `')dnl
define(`celllist', `')dnl
define(`addtest', `define(`testlist', testlist `$1_TEST')dnl
$1_TEST_SOURCES = $1_TEST.cc
$1_TEST_LDADD = \
	$(top_builddir)/unittest/libunittest.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneigraph.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(DYNAMIC_LD_LIBS)
$1_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addbench', `define(`benchmarklist', benchmarklist `$1_BENCHMARK')dnl
$1_BENCHMARK_SOURCES = $1_BENCHMARK.cc
$1_BENCHMARK_LDADD = \
	$(top_builddir)/benchmark/libbenchmark.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneigraph.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(DYNAMIC_LD_LIBS)
$1_BENCHMARK_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addhh', `define(`filelist', filelist `$1.hh')define(`headerlist', headerlist `$1.hh')')dnl
define(`addimpl', `define(`filelist', filelist `$1-impl.hh')define(`headerlist', headerlist `$1-impl.hh')')dnl
define(`addcc', `define(`filelist', filelist `$1.cc')')dnl
define(`addcell', `define(`celllist', celllist `$1-cell.cc')')dnl
define(`addsse', `define(`sselist', sselist `$1-sse.cc')')dnl
define(`addthis', `dnl
ifelse(`$2', `hh', `addhh(`$1')', `')dnl
ifelse(`$2', `impl', `addimpl(`$1')', `')dnl
ifelse(`$2', `cc', `addcc(`$1')', `')dnl
ifelse(`$2', `cell', `addcell(`$1')', `')dnl
ifelse(`$2', `sse', `addsse(`$1')', `')dnl
ifelse(`$2', `benchmark', `addbench(`$1')', `')dnl
ifelse(`$2', `test', `addtest(`$1')', `')')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')addthis(`$1',`$4')addthis(`$1',`$5')addthis(`$1',`$6')')dnl

include(`honei/graph/files.m4')

if CELL

CELLFILES = celllist
CELLTESTLIBS = $(top_builddir)/honei/cell/libcell.la

endif

if SSE

SSEFILES = sselist

endif

AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in Makefile.am
EXTRA_DIST = Makefile.am.m4 files.m4

DEFS = \
	$(CELLDEF) \
	$(SSEDEF) \
	$(DEBUGDEF)

lib_LTLIBRARIES = libhoneigraph.la

libhoneigraph_la_SOURCES = filelist $(CELLFILES) $(SSEFILES)
libhoneigraph_la_LIBADD = \
	$(top_builddir)/honei/util/libhoneiutil.la

libhoneigraph_includedir = $(includedir)/honei/graph
libhoneigraph_include_HEADERS = headerlist

TESTS = testlist
TESTS_ENVIRONMENT = bash $(top_builddir)/unittest/run.sh

check_PROGRAMS = $(TESTS)

benchmark_SOURCES = bench.cc
benchmark_LDADD = $(top_builddir)/benchmark/libbenchmark.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneigraph.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(DYNAMIC_LD_LIBS)
benchmark_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)

EXTRA_PROGRAMS = benchmark benchmarklist

benchm:
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)

bench:
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark

bench-sc: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark sc

bench-sse: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark sse

bench-mc: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark mc

bench-cpu: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark cpu

bench-cell: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/graph/benchmark cell

quickcheck: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run_quick.sh" check
quickcheck-sse: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run_quick-sse.sh" check
quickcheck-cell: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run_quick-cell.sh" check
quickcheck-mc: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run_quick-mc.sh" check
check-sse: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run-sse.sh" check
check-cell: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run-cell.sh" check
check-mc: $(TESTS)
	$(MAKE) $(AM_MAKEFLAGS) TESTS_ENVIRONMENT="bash $(top_builddir)/unittest/run-mc.sh" check

Makefile.am : Makefile.am.m4 files.m4
	$(top_srcdir)/misc/do_m4.bash Makefile.am

