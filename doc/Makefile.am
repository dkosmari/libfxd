# doc/Makefile.am

EXTRA_DIST = \
	Doxyfile.in \
	custom.css \
	doxygen-awesome.css \
	COPYING.MIT \
	mainpage.md


if USE_DOXYGEN

BUILT_SOURCES = html

EXTRA_DIST += html

html: doxyfile.stamp

doxyfile.stamp: Doxyfile
	$(DOXYGEN) $<
	echo Timestamp > $@

install-data-local:
	mkdir -p "$(DESTDIR)$(htmldir)"
	cp -r -t "$(DESTDIR)$(htmldir)" html/*

uninstall-local:
	$(RM) -r "$(DESTDIR)$(htmldir)"

CLEANFILES = doxyfile.stamp

clean-local:
	$(RM) -rf html


endif USE_DOXYGEN
