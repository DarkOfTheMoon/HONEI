ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`benchmarklist', `')dnl
define(`filelist', `')dnl
define(`celllist', `')dnl
define(`headerlist', `')dnl
define(`sselist', `')dnl
define(`testlist', `')dnl
define(`addtest', `define(`testlist', testlist `$1_TEST')dnl
$1_TEST_SOURCES = $1_TEST.cc
$1_TEST_LDADD = \
	$(top_builddir)/unittest/libunittest.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneiswe.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(DYNAMIC_LD_LIBS)
$1_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addbench', `define(`benchmarklist', benchmarklist `$1_BENCHMARK')dnl
$1_BENCHMARK_SOURCES = $1_BENCHMARK.cc
$1_BENCHMARK_LDADD = \
	$(top_builddir)/benchmark/libbenchmark.a \
	$(top_builddir)/honei/la/libhoneila.la \
	libhoneiswe.la \
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
ifelse(`$2', `test', `addtest(`$1')', `')dnl
ifelse(`$2', `benchmark', `addbench(`$1')', `')')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')addthis(`$1',`$4')addthis(`$1',`$5')addthis(`$1',`$6')')dnl

include(`honei/swe/files.m4')

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
	$(DEBUGDEF) \
	$(PROFILERDEF)

lib_LTLIBRARIES = libhoneiswe.la

libhoneiswe_la_SOURCES = filelist $(CELLFILES) $(SSEFILES)
libhoneiswe_la_LIBADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(CELLLIB)

libhoneiswe_includedir = $(includedir)/honei/libhoneiswe
libhoneiswe_include_HEADERS = headerlist

TESTS = testlist
TESTS_ENVIRONMENT = bash $(top_builddir)/unittest/run.sh

check_PROGRAMS = $(TESTS)

selected_benchmarks_SOURCES = selected_benchmarks.cc
selected_benchmarks_LDADD = \
	$(top_builddir)/benchmark/libbenchmark.a \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	libhoneiswe.la \
	$(DYNAMIC_LD_LIBS)
selected_benchmarks_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)

EXTRA_PROGRAMS = benchmarklist selected_benchmarks

benchm:
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)

bench:
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK

bench-sc: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK sc

bench-sse: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK sse

bench-mc: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK mc

bench-cpu: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK cpu

bench-cell: 
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	bash $(top_builddir)/honei/libhoneiswe/solver_BENCHMARK cell
	bash $(top_builddir)/honei/libhoneiswe/relax_solver_BENCHMARK cell

sel-benchs:
	$(MAKE) $(AM_MAKEFLAGS) $(EXTRA_PROGRAMS)
	$(top_builddir)/honei/libhoneiswe/selected_benchmarks plot
	gnuplot *.plt
	pdflatex -shell-escape RecentPlots.tex
	pdflatex -shell-escape RecentPlots.tex
	rm PlotOut_*.plt
	rm PlotOut_*.pdf

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

