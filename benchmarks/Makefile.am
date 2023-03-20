# benchmarks/Makefile.am


AM_DEFAULT_SOURCE_EXT = .cpp


noinst_HEADERS = \
	make-random-vec.hpp \
	matrix.hpp


AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(BENCHMARK_CFLAGS)


AM_LDFLAGS = $(BENCHMARK_LIBS)


if USE_BENCHMARK

check_PROGRAMS = \
	bench-gauss-seidel \
	bench-matrix-invert \
	bench-sqrt

endif
