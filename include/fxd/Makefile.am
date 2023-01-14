fxddir = $(includedir)/fxd

fxd_HEADERS = \
	casting.hpp \
	concepts.hpp \
	constructors.hpp \
	conversions.hpp \
	fixed.hpp \
	fxd.hpp \
	hash.hpp \
	math.hpp \
	numbers.hpp \
	operators.hpp \
	random.hpp \
	round-add.hpp \
	round-div.hpp \
	round-mul.hpp \
	round-sub.hpp \
	safe-operators.hpp \
	traits.hpp \
	types.hpp \
	utils-add.hpp \
	utils-div.hpp \
	utils-mul.hpp \
	utils-overflow.hpp \
	utils-shift.hpp \
	utils-tuple.hpp \
	utils.hpp


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
