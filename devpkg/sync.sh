#!/bin/sh
rsync -auvz --delete -e ssh ./ shell.sourceforge.jp:/home/groups/x/xq/xqt/htdocs/devpkg
