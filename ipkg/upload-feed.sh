#!/bin/sh
rsync -auvz -e ssh Packages *.ipk shell.sourceforge.jp:/home/groups/x/xq/xqt/htdocs/feed-testing/
