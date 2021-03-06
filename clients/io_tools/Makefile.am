AM_CXXFLAGS = -I$(top_srcdir)

CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

DEFS = \
	$(DEBUGDEF) \
	$(GMPDEF) \
	$(SSEDEF) \
	$(BOOSTDEF)

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

if ITANIUM

ITANIUMFILES = itaniumlist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/itanium/libhoneibackendsitanium.la

endif

if OPENCL

OPENCLFILES = opencllist
BACKEND_LIBS += \
	$(top_builddir)/honei/backends/opencl/libhoneibackendsopencl.la

endif

bin_PROGRAMS = m2ell mtx2ell exp2dv m2dv ell2mtx mtxinfo minfo ellinfo ellexpand ell2spai ell2grote ell2sainv feast2ell feast2dv match_sainv

m2ell_SOURCES = mtoell.cc
m2ell_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

mtx2ell_SOURCES = mtxtoell.cc
mtx2ell_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

exp2dv_SOURCES = exptodv.cc
exp2dv_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

m2dv_SOURCES = mtodv.cc
m2dv_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

ell2mtx_SOURCES = elltomtx.cc
ell2mtx_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

mtxinfo_SOURCES = mtxinfo.cc
mtxinfo_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

ellinfo_SOURCES = ellinfo.cc
ellinfo_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

ellexpand_SOURCES = ellexpand.cc
ellexpand_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

minfo_SOURCES = minfo.cc
minfo_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

ell2spai_SOURCES = elltospai.cc
ell2spai_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(top_builddir)/honei/math/spai/src/libspai.la \
	$(top_builddir)/honei/math/spai/lapack/liblapack.la \
	$(top_builddir)/honei/math/spai/blas/libblas.la \
	$(BACKEND_LIBS)
#ell2spai_LDFLAGS = -Wl,--start-group -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -Wl,--end-group -lpthread -fopenmp

ell2grote_SOURCES = elltogrote.cc
ell2grote_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(top_builddir)/honei/math/spai/src/libspai.la \
	$(top_builddir)/honei/math/spai/lapack/liblapack.la \
	$(top_builddir)/honei/math/spai/blas/libblas.la \
	$(BACKEND_LIBS)

ell2sainv_SOURCES = elltosainv.cc
ell2sainv_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

feast2ell_SOURCES = feasttoell.cc
feast2ell_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

feast2dv_SOURCES = feasttodv.cc
feast2dv_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

match_sainv_SOURCES = match_sainv.cc
match_sainv_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la \
	$(top_builddir)/honei/la/libhoneila.la \
	$(top_builddir)/honei/math/libhoneimath.la \
	$(BACKEND_LIBS)

if BLAPACK
else
  ell2spai_LDADD += \
	$(top_builddir)/honei/math/spai/blas/libblas.la
endif
