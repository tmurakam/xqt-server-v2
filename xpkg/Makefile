name = xpkg
version  = 1.0.0

distdir	= $(name)-$(version)

prefix	= /usr
bindir	= $(prefix)/bin
libdir  = $(prefix)/lib
datadir	= $(prefix)/share
mandir  = $(prefix)/man
pkgdatadir = $(datadir)/$(name)
pkglibdir  = $(libdir)/$(name)
sysconfdir = /etc/$(name)

TARGET	= xpkg xpkg-install

all: $(TARGET)

xpkg: xpkg.in
	cat xpkg.in | sed \
	-e 's!@prefix@!$(prefix)!' \
	-e 's!@pkglibdir@!$(pkglibdir)!' \
	-e 's!@sysconfdir@!$(sysconfdir)!' \
	-e 's!@version@!$(version)!' \
	> xpkg
	chmod 755 xpkg

xpkg-install: xpkg-install.in
	cat xpkg-install.in | sed \
	-e 's!@prefix@!$(prefix)!' \
	-e 's!@pkglibdir@!$(pkglibdir)!' \
	-e 's!@sysconfdir@!$(sysconfdir)!' \
	-e 's!@version@!$(version)!' \
	> xpkg-install
	chmod 755 xpkg-install

clean:
	-/bin/rm $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(bindir)
	install -m 755 xpkg xpkg-install $(DESTDIR)$(bindir)

	install -d $(DESTDIR)$(pkglibdir)
	install -m 644 lib/*.rb $(DESTDIR)$(pkglibdir)
	install -m 755 lib/ipkg-build $(DESTDIR)$(pkglibdir)

	install -d $(DESTDIR)$(sysconfdir)
	install -m 644 etc/*.def etc/*.site etc/*.rb etc/*.sample $(DESTDIR)$(sysconfdir)

	install -d $(DESTDIR)$(mandir)/man1
	install -m 644 doc/*.1 $(DESTDIR)$(mandir)/man1
	install -d $(DESTDIR)$(mandir)/man5
	install -m 644 doc/*.5 $(DESTDIR)$(mandir)/man5

	./doc/fixcode.sh $(DESTDIR)$(mandir)/man[1-9]/*

dist:
	-/bin/rm -rf $(distdir)
	mkdir $(distdir)
	mkdir $(distdir)/lib
	mkdir $(distdir)/doc
	mkdir $(distdir)/etc

	cp AUTHORS COPYING README INSTALL ChangeLog Makefile *.in xpkg.spec $(distdir)
	cp lib/*.rb lib/ipkg-build $(distdir)/lib
	cp etc/*.def etc/*.site etc/*.rb etc/*.sample $(distdir)/etc
	cp doc/*.[1-9] doc/*.sh $(distdir)/doc

	tar cvzf $(distdir).tar.gz $(distdir)
	-/bin/rm -rf $(distdir)

rpm: dist
	rpmbuild --ta $(distdir).tar.gz
