CLEANFILES = *~
MAINTAINERCLEANFILES = Makefile.in

CXX = $(SPU_CXX)
CXXFLAGS= $(SPU_CXXFLAGS) -I$(top_srcdir)
DEFS = $(DEBUGDEF) $(PROFILERDEF)

noinst_LIBRARIES = libla_spe.a
libla_spe_a_SOURCES = \
	dot_product_dense_dense_float.cc \
	dot_product_dense_dense_double.cc \
	element_product_dense_dense_float.cc \
	element_product_dense_dense_double.cc \
	product_dense_matrix_dense_matrix_float.cc \
	product_dense_matrix_dense_matrix_double.cc \
	sum_dense_dense_float.cc \
	sum_dense_dense_double.cc \
	sum_dense_scalar_float.cc \
	sum_dense_scalar_double.cc \
	difference_dense_dense_float.cc \
	difference_dense_dense_double.cc \
	product_dense_matrix_dense_vector_float.cc \
	product_dense_matrix_dense_vector_double.cc \
	product_banded_matrix_dense_vector_float.cc \
	product_banded_matrix_dense_vector_double.cc \
	sum_dense_sparse_float.cc \
	scale_dense_float.cc \
	scale_dense_double.cc \
	reduction_sum_dense_float.cc \
	reduction_sum_dense_double.cc \
	reduction_max_dense_float.cc \
	reduction_min_dense_float.cc \
	norm_max_dense_float.cc \
	norm_l_one_dense_float.cc \
	norm_l_one_dense_double.cc \
	norm_l_two_dense_float.cc \
	norm_l_two_dense_double.cc \
	element_inverse_float.cc \
	scaled_sum_dense_dense_float.cc \
	scaled_sum_dense_dense_double.cc \
	scale_sparse_float.cc \
	fill_float.cc \
	operations.hh

libla_spe_a_LIBADD = \
	$(top_builddir)/honei/backends/cell/spe/libutil/libutil_spe.a

libla_spe_a_CXXFLAGS = -Wall -msafe-dma -fno-exceptions -fno-rtti
