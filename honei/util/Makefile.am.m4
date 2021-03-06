ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`general_filelist', `')dnl
define(`general_testlist', `')dnl
define(`general_headerlist', `')dnl
define(`hdf5_filelist', `')dnl
define(`hdf5_testlist', `')dnl
define(`hdf5_headerlist', `')dnl
define(`addtest', `define(`$1_testlist', $1_testlist `$2_TEST')dnl
$2_TEST_SOURCES = $2_TEST.cc
$2_TEST_LDADD = \
	libhoneiutil.la \
	$(DYNAMIC_LD_LIBS)
$2_TEST_CXXFLAGS = -I$(top_srcdir) $(AM_CXXFLAGS)
')dnl
define(`addhh', `define(`$1_filelist', $1_filelist `$2.hh')define(`$1_headerlist', $1_headerlist `$2.hh')')dnl
define(`addfwd', `define(`$1_filelist', $1_filelist `$2-fwd.hh')define(`$1_headerlist', $1_headerlist `$2-fwd.hh')')dnl
define(`addimpl', `define(`$1_filelist', $1_filelist `$2-impl.hh')')dnl
define(`addcc', `define(`$1_filelist', $1_filelist `$2.cc')')dnl
define(`addthis', `dnl
ifelse(`$3', `hh', `addhh(`$1',`$2')', `')dnl
ifelse(`$3', `fwd', `addfwd(`$1', `$2')', `')dnl
ifelse(`$3', `impl', `addimpl(`$1', `$2')', `')dnl
ifelse(`$3', `cc', `addcc(`$1',`$2')', `')dnl
ifelse(`$3', `test', `addtest(`$1',`$2')', `')')dnl
define(`add', `addthis(`$1',`$2',`$3')addthis(`$1',`$2',`$4')addthis(`$1',`$2',`$5')addthis(`$1',`$2',`$6')')dnl

include(`honei/util/files.m4')

AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in Makefile.am
EXTRA_DIST = Makefile.am.m4 files.m4 run.sh mpi_run.sh
DEFS = \
	$(CELLDEF) \
	$(SSEDEF) \
	$(CUDADEF) \
	$(OPENCLDEF) \
	$(ITANIUMDEF) \
	$(DEBUGDEF) \
	$(BOOSTDEF) \
	$(MPIDEF) \
	$(GMPDEF) \
	$(PROFILERDEF) \
	-DHONEI_SOURCEDIR='"$(top_srcdir)"'

if HDF5

HDF5SOURCES = hdf5_filelist
HDF5TESTS = hdf5_testlist
HDF5HEADERS = hdf5_headerlist
HDF5LIBS = -lhdf5 -lnetcdf -lnetcdf_c++

else

endif

if GMP

GMPLIBS = -lgmpxx -lgmp

else

endif

lib_LTLIBRARIES = libhoneiutil.la

libhoneiutil_la_SOURCES = general_filelist $(HDF5SOURCES)
libhoneiutil_la_LIBADD = \
	-lpthread \
	$(HDF5LIBS) \
	$(GMPLIBS)

libhoneiutil_includedir = $(includedir)/honei/util
libhoneiutil_include_HEADERS = general_headerlist $(HDF5HEADERS)

TESTS = general_testlist $(HDF5TESTS)
TESTS_ENVIRONMENT = env BACKENDS="$(BACKENDS)" TYPE=$(TYPE) bash $(top_srcdir)/honei/util/run.sh

check_PROGRAMS = $(TESTS)

Makefile.am : Makefile.am.m4 files.m4
	cd $(top_srcdir) ; ./misc/do_m4.bash honei/util/Makefile.am
