TOPDIR=../..

all: build

cleanup source prebuild build install:
	$(TOPDIR)/script/mkpkg $@ build.pkgdef

ipkg:
	$(TOPDIR)/script/mkpkg $@ emacs21.pkgdef \
		emacs21-lisp1.pkgdef emacs21-lisp2.pkgdef emacs21-lisp3.pkgdef emacs21-lisp4.pkgdef

World1: source prebuild notice

World2: build install ipkg

notice:
	@echo ----------------------------------------------------
	@echo You must dump manually on Zaurus as follows:
	@echo
	@echo  cd emacs-21/arm/src
	@echo  LC_ALL=C ./temacs -batch -l loadup dump
	@echo
	@echo Then execute make World2 on host machine.


