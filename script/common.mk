all: build

World DevWorld cleanup source build install ipkg devpkg install-devpkg post-cleanup:
	$(TOPDIR)/script/mkpkg $@

