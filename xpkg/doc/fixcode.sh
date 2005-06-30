#!/bin/sh

# man page の漢字コードを修正する

if [ -e /etc/sysconfig/i18n ]; then
    . /etc/sysconfig/i18n
fi

if [ $LANG = "ja_JP.UTF-8" ]; then
    for i in $@; do
	echo converting $i
	iconv --from-code euc-jp --to-code utf-8 $i > $i.new
	/bin/rm $i
	/bin/mv $i.new $i
    done
fi

