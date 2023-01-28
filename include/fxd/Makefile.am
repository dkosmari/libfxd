fxddir = $(includedir)/fxd

fxd_HEADERS = \
	casting.hpp \
	concepts.hpp \
	constructors.hpp \
	conversions.hpp \
	error.hpp \
	except.hpp \
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
	traits.hpp \
	types.hpp \
	utils-add.hpp \
	utils-div.hpp \
	utils-expected.hpp \
	utils-mul.hpp \
	utils-overflow.hpp \
	utils-safe-div.hpp \
	utils-safe-includes.hpp \
	utils-safe-mul.hpp \
	utils-safe.hpp \
	utils-shift.hpp \
	utils-tuple.hpp \
	utils.hpp


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
