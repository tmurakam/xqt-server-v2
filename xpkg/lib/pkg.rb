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

    def initialize
	@df = DefFile.new
    end

    def loaddef(deffile, target, destdir, configdir)
	@destdir = destdir
        @deffile = deffile

	# グローバル設定
	@df.setDefine("target", target)
	@df.setDefine("destdir", destdir)
	@df.setDefine("configdir", configdir)

	@df.setTarget(target)
	
	# pkgdef ファイルロード
        if (FileTest.exist?("#{configdir}/config-#{target}.def"))
	    @df.load_from_file("#{configdir}/config-#{target}.def")
	else
	    @df.load_from_file("#{configdir}/config.def")
	end
	@df.load_from_file(deffile) if (deffile != nil)
    end

    def getBuildDir
	builddir = @df.getDefine("builddir")

	opt = nil
	if (builddir =~ /^(.*):(.*)$/)
	    builddir = $1
	    opt = $2
	end
	return builddir, opt
    end

    # スクリプト実行
    def execSectionScript(section)
	sectval = @df.getSection(section)

	# clean 時の特別処理 (ちょっと汚い、、、)
	if (sectval == nil)
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

    # ソースの展開
    def getSource(distfiledir, dlOnly = false)
	sites = @df.getSectValues("site")
	sources = @df.getSectValues("source")
	builddir = getBuildDir()

	GetSource(sites, sources, distfiledir, builddir, dlOnly)
    end

    # clean する
    def clean
	execSectionScript("clean")
    end

    # cleanup する
    def cleanup
	builddir, opt = getBuildDir()
	if (builddir != nil && FileTest.exist?(builddir) && opt != "noclean")
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

