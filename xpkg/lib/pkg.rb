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
# pkg.rb
#

require "deffile"
require "getsrc"
require "execscript"

class Pkg
    include GetSource
    include ExecScript

    attr_reader :df, :deffile

    # Initialize
    def initialize
	@df = DefFile.new
    end

    # Load def files
    def loaddef(deffile, target, destdir, configdir)
	@destdir = destdir
        @deffile = deffile

	# global defines
	@df.setDefine("target", target)
	@df.setDefine("destdir", destdir)
	@df.setDefine("configdir", configdir)

	@df.setDefine("username", `whoami`.chop)

	@df.setTarget(target)
	
	# load pkgdef files.
        if (FileTest.exist?("#{configdir}/config-#{target}.def"))
	    @df.load_from_file("#{configdir}/config-#{target}.def")
	else
	    @df.load_from_file("#{configdir}/config.def")
	end
	@df.load_from_file(deffile) if (deffile != nil)
    end

    def getBuildDir
	builddir = @df.getDefine("builddir")

	if (builddir == nil)
	    name = @df.getDefine("name")
	    ver = @df.getDefine("ver")
	    if (name == nil || ver == nil)
		puts "Error: You must specify builddir or name/ver in pkgdef."
		exit 1
	    end
	    builddir = name + "-" + ver
	end

	opt = nil
	if (builddir =~ /^(.*):(.*)$/)
	    builddir = $1
	    opt = $2
	end
	return builddir, opt
    end

    # Execute scripts
    def execSectionScript(section)
	sectval = @df.getSection(section)

	if (sectval == nil)
            # special handling for make clean (dirty hack...)
	    if (section == "clean")
		sectval = "make clean\n"
	    else
		return # do nothing
	    end
	end
	
	builddir, opt = getBuildDir()
	builddir = "." if (builddir == nil)
	ExecScript(sectval, builddir)
    end

    # Extract source tarball
    def getSource(distfiledir, dlOnly = false)
	sites = @df.getSectValues("site")
	sources = @df.getSectValues("source")
	builddir = getBuildDir()

	GetSource(sites, sources, distfiledir, builddir, dlOnly)
    end

    # Execute clean
    def clean
	execSectionScript("clean")
    end

    # Execute cleanup
    def cleanup
	builddir, opt = getBuildDir()
	if (builddir != nil && builddir != "." && FileTest.exist?(builddir) && opt != "noclean")
	    ExecCmd("/bin/rm -rf #{builddir}")
	end

	if (FileTest.exist?(@destdir))
	    ExecCmd("/bin/rm -rf #{@destdir}")
	end
	if (FileTest.exist?("pkgtmp"))
	    ExecCmd("/bin/rm -rf pkgtmp")
	end
    end
end

