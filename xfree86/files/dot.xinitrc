#!/bin/sh
export LANG=ja_JP.eucJP
export DISPLAY=:0.0

# default: use anthy
export XMODIFIERS=@im=uim-anthy

# start uim-xim
if [ -x /opt/QtPalmtop/bin/uim-xim ]; then
    exec /opt/QtPalmtop/bin/uim-xim &
fi

rxvt &
blackbox
