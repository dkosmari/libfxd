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
	detail/add.hpp \
	detail/bias.hpp \
	detail/div.hpp \
	detail/error.hpp \
	detail/expected.hpp \
	detail/mul.hpp \
	detail/opacify.hpp \
	detail/safe-div.hpp \
	detail/safe-includes.hpp \
	detail/safe-mul.hpp \
	detail/safe.hpp \
	detail/shift.hpp \
	detail/sub.hpp \
	detail/tuple.hpp \
	detail/types.hpp \
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
