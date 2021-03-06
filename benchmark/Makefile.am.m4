ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`benchmarklist', `')dnl
define(`addbench', `define(`benchmarklist', benchmarklist `$1_BENCHMARK')dnl
$1_BENCHMARK_SOURCES = $1_BENCHMARK.cc
$1_BENCHMARK_LDADD = \
	libbenchmark.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(top_builddir)/honei/swe/libhoneiswe.la \
	$(top_builddir)/honei/lbm/libhoneilbm.la \
	$(top_builddir)/honei/graph/libhoneigraph.la \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/math/spai/src/libspai.la \
	$(top_builddir)/honei/math/SuperLU_4.1/SRC/libsuperlu.la \
	$(BACKEND_LIBS) \
	$(DYNAMIC_LD_LIBS)

if BLAPACK
else
  $1_BENCHMARK_LDADD += \
	$(top_builddir)/honei/math/spai/blas/libblas.la \
	$(top_builddir)/honei/math/spai/lapack/liblapack.la \
	$(top_builddir)/honei/math/SuperLU_4.1/CBLAS/libsblas.la
endif
$1_BENCHMARK_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addthis', `dnl
ifelse(`$2', `bench', `addbench(`$1')', `')dnl
')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')addthis(`$1',`$4')addthis(`$1',`$5')addthis(`$1',`$6')addthis(`$1',`$7')')dnl

include(`benchmark/files.m4')

BACKEND_LIBS = \
       $(top_builddir)/honei/backends/multicore/libhoneibackendsmulticore.la

if CELL

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/cell/ppe/libhoneibackendscellppe.la \
	$(top_builddir)/honei/backends/cell/spe/libhoneibackendscellspe.la

endif

if CUDA

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/cuda/libhoneibackendscuda.la \
	-lcudart

endif

if SSE

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/sse/libhoneibackendssse.la

endif

if OPENCL

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/opencl/libhoneibackendsopencl.la

endif

if ITANIUM

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/itanium/libhoneibackendsitanium.la

endif

AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in Makefile.am
EXTRA_DIST = Makefile.am.m4 files.m4

DEFS = \
	$(CELLDEF) \
	$(SSEDEF) \
	$(OPENCLDEF) \
	$(ITANIUMDEF) \
	$(CUDADEF) \
	$(CUBLASDEF) \
	$(CUDA_DOUBLEDEF) \
	$(DEBUGDEF) \
	$(GMPDEF) \
	$(BOOSTDEF) \
	$(PROFILERDEF) \
	-DHONEI_SOURCEDIR='"$(top_srcdir)"'

noinst_LTLIBRARIES = libbenchmark.la
noinst_PROGRAMS = benchmarklist

libbenchmark_la_SOURCES = \
	benchmark.cc benchmark.hh

BENCHMARKS = benchmarklist

.PHONY: benchmark
benchmark: $(BENCHMARKS)
	@failed=0; \
	for b in $(BENCHMARKS) ; do \
	    echo ">>> $$b[BACKENDS=$(BACKENDS)]" ; \
	    if ! ./$$b $(BACKENDS) ; then \
		failed=1; \
	    fi; \
	done; \
	test "$$failed" -eq 0;

Makefile.am : Makefile.am.m4 files.m4
	cd $(top_srcdir) ; ./misc/do_m4.bash benchmark/Makefile.am
