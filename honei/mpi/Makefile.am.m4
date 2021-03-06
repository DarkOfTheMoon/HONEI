ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`filelist', `')dnl
define(`celllist', `')dnl
define(`headerlist', `')dnl
define(`sselist', `')dnl
define(`cudalist', `')dnl
define(`opencllist', `')dnl
define(`itaniumlist', `')dnl
define(`testlist', `')dnl
define(`addtest', `define(`testlist', testlist `$1_TEST')dnl
$1_TEST_SOURCES = $1_TEST.cc
$1_TEST_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(top_builddir)/honei/mpi/libhoneimpi.la \
	$(BACKEND_LIBS) \
	$(DYNAMIC_LD_LIBS)
$1_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addhh', `define(`filelist', filelist `$1.hh')define(`headerlist', headerlist `$1.hh')')dnl
define(`addimpl', `define(`filelist', filelist `$1-impl.hh')define(`headerlist', headerlist `$1-impl.hh')')dnl
define(`addfwd', `define(`filelist', filelist `$1-fwd.hh')define(`headerlist', headerlist `$1-fwd.hh')')dnl
define(`addcc', `define(`filelist', filelist `$1.cc')')dnl
define(`addcell', `define(`celllist', celllist `$1-cell.cc')')dnl
define(`addmc', `define(`filelist', filelist `$1-mc.hh')define(`headerlist', headerlist `$1-mc.hh')')dnl
define(`addsse', `define(`sselist', sselist `$1-sse.cc')')dnl
define(`addcuda', `define(`cudalist', cudalist `$1-cuda.cc')')dnl
define(`addopencl', `define(`opencllist', opencllist `$1-opencl.cc')')dnl
define(`additanium', `define(`itaniumlist', itaniumlist `$1-itanium.cc')')dnl
define(`addthis', `dnl
ifelse(`$2', `hh', `addhh(`$1')', `')dnl
ifelse(`$2', `impl', `addimpl(`$1')', `')dnl
ifelse(`$2', `fwd', `addfwd(`$1')', `')dnl
ifelse(`$2', `cc', `addcc(`$1')', `')dnl
ifelse(`$2', `cell', `addcell(`$1')', `')dnl
ifelse(`$2', `sse', `addsse(`$1')', `')dnl
ifelse(`$2', `mc', `addmc(`$1')', `')dnl
ifelse(`$2', `cuda', `addcuda(`$1')', `')dnl
ifelse(`$2', `opencl', `addopencl(`$1')', `')dnl
ifelse(`$2', `itanium', `additanium(`$1')', `')dnl
ifelse(`$2', `test', `addtest(`$1')', `')dnl
')dnl
define(`add', `addthis(`$1',`$2')addthis(`$1',`$3')addthis(`$1',`$4')addthis(`$1',`$5')addthis(`$1',`$6')addthis(`$1',`$7')addthis(`$1',`$8')addthis(`$1',`$9')addthis(`$1',`$10')')dnl

include(`honei/mpi/files.m4')

BACKEND_LIBS = \
       $(top_builddir)/honei/backends/multicore/libhoneibackendsmulticore.la

if CELL

CELLFILES = celllist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/cell/ppe/libhoneibackendscellppe.la \
	$(top_builddir)/honei/backends/cell/spe/libhoneibackendscellspe.la

endif

if CUDA

CUDAFILES = cudalist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/cuda/libhoneibackendscuda.la \
	-lcudart

endif

if SSE

SSEFILES = sselist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/sse/libhoneibackendssse.la

endif

if OPENCL

OPENCLFILES = opencllist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/opencl/libhoneibackendsopencl.la

endif

if ITANIUM

ITANIUMFILES = itaniumlist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/itanium/libhoneibackendsitanium.la

endif

if MPI

BACKEND_LIBS += \
	$(top_builddir)/honei/backends/mpi/libhoneibackendsmpi.la

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
	$(CUDA_DOUBLEDEF) \
	$(CUBLASDEF) \
	$(DEBUGDEF) \
	$(BOOSTDEF) \
	$(MPIDEF) \
	$(GMPDEF) \
	$(PROFILERDEF) \
	-DHONEI_SOURCEDIR='"$(top_srcdir)"'

lib_LTLIBRARIES = libhoneimpi.la

libhoneimpi_la_SOURCES = filelist $(CELLFILES) $(SSEFILES) $(CUDAFILES) $(OPENCLFILES) $(ITANIUMFILES)
libhoneimpi_la_LIBADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(CELLLIB)

libhoneimpi_includedir = $(includedir)/honei/mpi
libhoneimpi_include_HEADERS = headerlist

TESTS = testlist
TESTS_ENVIRONMENT = env BACKENDS="$(BACKENDS)" TYPE="$(TYPE)" bash $(top_srcdir)/honei/util/mpi_run.sh

check_PROGRAMS = $(TESTS)

Makefile.am : Makefile.am.m4 files.m4
	cd $(top_srcdir) ; ./misc/do_m4.bash honei/mpi/Makefile.am
