AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

DEFS = \
	$(CELLDEF) \
	$(SSEDEF) \
	$(CUDADEF) \
	$(DEBUGDEF) \
	$(BOOSTDEF) \
	$(PROFILERDEF)

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
	$(top_builddir)/honei/backends/cuda/libhoneibackendscuda.la

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

bin_PROGRAMS = honei-swe
honei_swe_SOURCES = honei_swe.hh scenario_controller.hh honei_swe.cc
honei_swe_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/swe/libhoneiswe.la \
	$(BACKEND_LIBS) \
	-lGL \
	-lglut \
	-lGLU
