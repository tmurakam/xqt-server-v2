all: build

World DevWorld cleanup source build install ipkg devpkg install-devpkg:
	$(TOPDIR)/script/mkpkg $@

