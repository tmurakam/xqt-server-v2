#!/bin/sh
../../script/mkpkg cleanup stage1.pkgdef
../../script/mkpkg source stage1.pkgdef
../../script/mkpkg build stage1.pkgdef

### notify
echo ----------------------------------------------------
echo You must dump manually on Zaurus as follows:
echo
echo  cd emacs-21/arm/src
echo  strip temacs
echo  LC_ALL=C ./temacs -batch -l loadup dump
echo
echo Then execute stage1.sh on host machine.

