fxddir = $(includedir)/fxd

nobase_fxd_HEADERS = \
	casting.hpp \
	compare.hpp \
	concepts.hpp \
	constructors.hpp \
	conversions.hpp \
	except.hpp \
	fixed.hpp \
	fxd.hpp \
	hash.hpp \
	impl/add.hpp \
	impl/bias.hpp \
	impl/div.hpp \
	impl/error.hpp \
	impl/expected.hpp \
	impl/mul.hpp \
	impl/opacify.hpp \
	impl/safe-div.hpp \
	impl/safe-includes.hpp \
	impl/safe-mul.hpp \
	impl/safe.hpp \
	impl/shift.hpp \
	impl/sub.hpp \
	impl/tuple.hpp \
	impl/types.hpp \
	limits.hpp \
	math.hpp \
	numbers.hpp \
	operators.hpp \
	random.hpp \
	round-div.hpp \
	round-mul.hpp \
	saturate.hpp \
	traits.hpp


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
