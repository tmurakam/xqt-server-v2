all: build

World cleanup source build install ipkg devpkg:
	$(TOPDIR)/script/mkpkg $@

