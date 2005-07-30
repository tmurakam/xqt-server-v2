name = xpkg
version  = 1.0.4

distdir	= $(name)-$(version)

prefix	= /usr
bindir	= $(prefix)/bin
libdir  = $(prefix)/lib
datadir	= $(prefix)/share
mandir  = $(datadir)/man
pkgdatadir = $(datadir)/$(name)
pkglibdir  = $(libdir)/$(name)
sysconfdir = /etc/$(name)

TARGET	= xpkg-common.rb

all: $(TARGET)

xpkg-common.rb: xpkg-common.rb.in
	cat xpkg-common.rb.in | sed \
	-e 's!@prefix@!$(prefix)!' \
	-e 's!@pkglibdir@!$(pkglibdir)!' \
	-e 's!@sysconfdir@!$(sysconfdir)!' \
	-e 's!@version@!$(version)!' \
	> xpkg-common.rb

clean:
	-/bin/rm $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(bindir)
	install -m 755 xpkg xpkg-install xpkg-batch xpkg-common.rb $(DESTDIR)$(bindir)

	install -d $(DESTDIR)$(pkglibdir)
	install -m 644 lib/*.rb $(DESTDIR)$(pkglibdir)
	install -m 755 lib/ipkg-build $(DESTDIR)$(pkglibdir)

	install -d $(DESTDIR)$(sysconfdir)
	install -m 644 etc/*.def etc/*.site etc/*.rb etc/*.sample $(DESTDIR)$(sysconfdir)

	install -d $(DESTDIR)$(mandir)/man1
	install -d $(DESTDIR)$(mandir)/man5
	install -d $(DESTDIR)$(mandir)/ja/man1
	install -d $(DESTDIR)$(mandir)/ja/man5

	install -m 644 doc/man/*.1 $(DESTDIR)$(mandir)/man1
	install -m 644 doc/man/*.5 $(DESTDIR)$(mandir)/man5
	install -m 644 doc/man.ja/*.1 $(DESTDIR)$(mandir)/ja/man1
	install -m 644 doc/man.ja/*.5 $(DESTDIR)$(mandir)/ja/man5

	./doc/fixcode.sh $(DESTDIR)$(mandir)/ja/man[1-9]/*

dist: all
	-/bin/rm -rf $(distdir)
	mkdir $(distdir)
	mkdir $(distdir)/lib
	mkdir $(distdir)/doc
	mkdir $(distdir)/doc/man
	mkdir $(distdir)/doc/man.ja
	mkdir $(distdir)/etc

	cp AUTHORS COPYING README INSTALL ChangeLog Makefile xpkg.spec $(distdir)
	cp xpkg xpkg-install xpkg-batch *.in $(distdir)
	cp lib/*.rb lib/ipkg-build $(distdir)/lib
	cp etc/*.def etc/*.site etc/*.rb etc/*.sample $(distdir)/etc
	cp doc/*.sh $(distdir)/doc
	cp doc/man/*.[1-9]  $(distdir)/doc/man
	cp doc/man.ja/*.[1-9] $(distdir)/doc/man.ja

	tar cvzf $(distdir).tar.gz $(distdir)
	-/bin/rm -rf $(distdir)

rpm: dist
	rpmbuild --ta $(distdir).tar.gz
