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

bin_PROGRAMS = honei-config

honei_config_SOURCES = config.cc
honei_config_LDADD = \
	$(top_builddir)/honei/util/libhoneiutil.la

