#!/usr/bin/ruby

# xpkg: Package build system for X/Qt project
#
# Copyright (C) 2003-2005 Takuya Murakami
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#
# common part
#

$pkglibdir = "/usr/lib/xpkg"
$sysconfdir = "/etc/xpkg"
$version = "1.0.3"

$copyright = "Copyright (C) 2003-2005 X/Qt Server Project (http://xqt.sourceforge.jp)\n" +
    "This program may be freely redistributed under the terms of the GNU GPL\n\n"

$xpkgtop = nil
$verbose = false

# load personal configuration file
inifile = ENV["HOME"] + "/.xpkg"
if FileTest.exist?(inifile)
    load "~/.xpkg"
end

$LOAD_PATH.push($pkglibdir)
$LOAD_PATH.push($sysconfdir)

# load global configuration file
require "config"

# reload personal configuration file
# (personal one has higher priority)
if FileTest.exist?(inifile)
    load "~/.xpkg"
end

# load optparse
require "optparse"
