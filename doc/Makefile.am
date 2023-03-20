# doc/Makefile.am

EXTRA_DIST = \
	COPYING.MIT \
	custom.css \
	design.md \
	Doxyfile.in \
	doxygen-awesome-sidebar-only.css \
	doxygen-awesome.css \
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
