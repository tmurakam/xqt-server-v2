TOPDIR=../..

all: build

cleanup source build:
	$(TOPDIR)/script/mkpkg $@ stage1.pkgdef

build2 install ipkg:
	$(TOPDIR)/script/mkpkg $@ stage2.pkgdef


World1: cleanup source build notice

World2: build2 install ipkg

notice:
	@echo ----------------------------------------------------
	@echo You must dump manually on Zaurus as follows:
	@echo
	@echo  cd emacs-21/arm/src
	@echo  strip temacs
	@echo  LC_ALL=C ./temacs -batch -l loadup dump
	@echo
	@echo Then execute stage1.sh on host machine.


