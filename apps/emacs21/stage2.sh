#!/bin/sh
../../script/mkpkg build stage2.pkgdef
../../script/mkpkg install stage2.pkgdef
../../script/mkpkg ipkg stage2.pkgdef
