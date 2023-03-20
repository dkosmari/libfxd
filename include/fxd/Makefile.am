fxddir = $(includedir)/fxd

nobase_fxd_HEADERS = \
	casting.hpp \
	compare.hpp \
	concepts.hpp \
	constructors.hpp \
	conversions.hpp \
	detail/add.hpp \
	detail/bias.hpp \
	detail/expected.hpp \
	detail/int_to_float.hpp \
	detail/opacify.hpp \
	detail/raw-div.hpp \
	detail/raw-mul.hpp \
	detail/safe-div.hpp \
	detail/safe-mul.hpp \
	detail/safe.hpp \
	detail/shift.hpp \
	detail/sub.hpp \
	detail/tuple.hpp \
	detail/types.hpp \
	error.hpp \
	except.hpp \
	expect.hpp \
	fixed.hpp \
	fxd.hpp \
	hash.hpp \
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
