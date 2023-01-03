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
	safe-operators.hpp \
	traits.hpp \
	types.hpp \
	utils.hpp


.PHONY: company

company: compile_flags.txt

compile_flags.txt: Makefile
	echo "$(AM_CPPFLAGS) $(CPPFLAGS) $(DEFS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
