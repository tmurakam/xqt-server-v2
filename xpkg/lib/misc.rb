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
# Misc module
#

module Misc
    # find top directory
    def gettopdir
	# check XPKGTOP environment var.
	dir = ENV["XPKGTOP"]
	return dir if (dir != nil)

	dir = $xpkgtop
	return dir if (dir != nil)

	# check _xpkgtop file
	dir = Dir.pwd
	while dir != "/" do
	    if (FileTest.exist?(dir + "/_xpkgtop"))
		return dir
	    end

	    dir = File.expand_path("..", dir)
	end

	return nil
    end

    # setup directory parameters
    def setupdirs(target)
	pwd = Dir.pwd
	@destdir = pwd + "/destdir"

	topdir = gettopdir
	if (topdir == nil)
	    @distfiledir = pwd
	    @pkgdir      = pwd
	    @develpkgdir      = pwd
	else
	    @distfiledir = "#{topdir}/XPKG/distfiles"
	    @pkgdir      = "#{topdir}/XPKG/packages"
	    @develpkgdir = "#{topdir}/XPKG/develpkgs"

	    if ($use_multi_config == true)
		@pkgdir      = "#{topdir}/XPKG/#{target}/packages"
		@develpkgdir = "#{topdir}/XPKG/#{target}/develpkgs"
	    end
	end
    end
end


