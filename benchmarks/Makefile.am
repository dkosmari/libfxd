# benchmarks/Makefile.am


noinst_HEADERS = \
	make-random-vec.hpp \
	matrix.hpp


AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(BENCHMARK_CFLAGS)


AM_LDFLAGS = $(BENCHMARK_LIBS)


check_PROGRAMS = \
	bench-gauss-seidel \
	bench-matrix-invert \
	bench-sqrt


AM_DEFAULT_SOURCE_EXT = .cpp
