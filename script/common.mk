all: build

World cleanup source build install ipkg:
	$(TOPDIR)/script/mkpkg $@

