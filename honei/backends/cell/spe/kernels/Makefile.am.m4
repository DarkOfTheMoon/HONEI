ifdef(`__gnu__',`',`errprint(`This is not GNU m4...
')m4exit(1)') dnl include(`misc/generated-file.txt')

dnl vim: set ft=m4 noet :

define(`filelist', `')dnl
define(`cleanlist', `')dnl
define(`objlist', `')dnl
define(`proglist', `')dnl
define(`sourceslist', `')dnl
define(`add', `dnl
define(`filelist', filelist `$1.sk $1.opcodes')dnl
define(`cleanlist', cleanlist `$1.body' `$1.functions' `$1.opcodes' `$1.cc')dnl
define(`objlist', objlist `libcell-$1.o')dnl
define(`proglist', proglist `$1')dnl
define(`sourceslist', sourceslist `$1.cc' `$1-registrator.cc')dnl

$1.cc : $1.sk $(top_srcdir)/misc/make_sk.bash $2-kernel.cc.in $1.sk
	$(top_srcdir)/misc/make_sk.bash $(top_srcdir)/honei/backends/cell/spe/kernels/$1.sk
	sed -e "/@FUNCTIONS@/r $1.functions" \
	    -e "/@FUNCTIONS@/d" \
	    -e "/@BODY@/r $1.body" \
	    -e "/@BODY@/d" \
	    -e "/@HEADER@/r $(top_srcdir)/misc/generated-file.txt" \
	    -e "/@HEADER@/d" \
	    -e "/vim/s/set/set ro/" \
	    $2-kernel.cc.in > $`'@

$1-registrator.cc : registrator.cc.in $1
	sed -e "s/@BEGIN@/$$(spu-readelf -s $1 | sed -ne "/_end/s/^[^:]*:[^0]*\([^ ]*\).*/0x\1/p")/" \
	    -e "s/@END@/0x35000/" \
	    -e "s/@IDENTIFIER@/$1/g" \
	    -e "s/@NAME@/$$(echo $1 | sed -e "s/kernel_//")/" \
	    -e "/@OPCODES@/r $(top_srcdir)/honei/backends/cell/spe/kernels/$1.opcodes" \
	    -e "/@OPCODES@/d" \
	    -e "s/@OPCODECOUNT@/$$(wc -l $(top_srcdir)/honei/backends/cell/spe/kernels/$1.opcodes | cut -d " " -f 1)/" \
	    -e "s/$(AT)TYPE$(AT)/kt_$2/g" \
	    -e "/@HEADER@/r $(top_srcdir)/misc/generated-file.txt" \
	    -e "/@HEADER@/d" \
	    -e "/vim/s/set/set ro/" \
	    $< > $`'@

$1_SOURCES = $1.cc
$1_CXXFLAGS = -Wall -msafe-dma -fno-exceptions -fno-rtti
$1_LDADD = \
	$(top_builddir)/honei/backends/cell/spe/libutil/libutil_spe.a \
	$(top_builddir)/honei/backends/cell/spe/libgraph/libgraph_spe.a \
	$(top_builddir)/honei/backends/cell/spe/libla/libla_spe.a \
	$(top_builddir)/honei/backends/cell/spe/liblbm/liblbm_spe.a \
	$(top_builddir)/honei/backends/cell/spe/libmath/libmath_spe.a \
	$(top_builddir)/honei/backends/cell/spe/libswe/libswe_spe.a \
	$(top_builddir)/honei/backends/cell/spe/libutil/libutil_spe.a

libcell-$1.o : $1
	$(PPU_EMBEDSPU) $1_handle $< $`'@
	sed -e "s/@NAME@/libcell-$1.o/"\
	    $(srcdir)/libtool-hack.in > libcell-$1.lo
')dnl

include(`honei/backends/cell/spe/kernels/files.m4')

AM_CXXFLAGS = -I$(top_srcdir)

CXX = $(SPU_CXX)
CXXFLAGS = $(SPU_CXXFLAGS) -I$(top_srcdir)
AT = @

BUILT_SOURCES = sourceslist objlist
CLEANFILES = *~ *.body *.functions *.opcodes *.cc *.o
DISTCLEANFILES = cleanlist
MAINTAINERCLEANFILES = Makefile.in Makefile.am

EXTRA_DIST = \
	Makefile.am.m4 \
	files.m4 \
	libtool-hack.in \
	registrator.cc.in \
	stand_alone-kernel.cc.in \
	filelist

DEFS = \
	$(DEBUGDEF)

noinst_PROGRAMS = proglist

Makefile.am : Makefile.am.m4 files.m4
	cd $(top_srcdir) ; ./misc/do_m4.bash honei/backends/cell/spe/kernels/Makefile.am
