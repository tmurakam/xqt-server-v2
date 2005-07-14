# xpkg-install: Package installer for X/Qt project
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

require "execscript"

class PkgInstall
    include ExecScript

    def SetParam(prefix, target_root, target_prefix)
	@prefix = prefix
	@target_root = target_root
	@target_prefix = target_prefix
    end

    def SetParamFromDef(df)
	@prefix = df.getDefine("prefix")
	@target_root = df.getDefine("target_root")
	@target_prefix = df.getDefine("target_prefix")
    end

    def dump
	puts "prefix:#{@prefix}"
	puts "target_root:#{@target_root}"
	puts "target_prefix:#{@target_prefix}"
    end

    def CleanTmpdir
	if (FileTest.exist?(".tmp"))
	    ExecCmd("/bin/rm -rf .tmp")
	end
    end

    def SetupTmpdir
	CleanTmpdir()
	Dir.mkdir(".tmp")
    end

    #
    # Install package files
    #
    def InstallGenPkg(file)
	ExecCmd("tar xvzf #{file} -C #{@target_root}")
    end

    def InstallGenPkgDevel(file)
	SetupTmpdir()
	ExecCmd("tar xzf #{file} -C .tmp .#{@prefix}")
	ExecCmd("tar cf - -C .tmp#{@prefix} . | tar xvf - -C #{@target_prefix}")
	ExecCmd("/bin/rm -rf .tmp")
	CleanTmpdir()
    end

    def InstallIpkg(file)
	ExecCmd("tar xzf #{file} -O ./data.tar.gz" +
		" | tar xvzf - -C #{@target_root}")
    end

    def InstallIpkgDevel(file)
	SetupTmpdir()
	ExecCmd("tar xzf #{file} -O ./data.tar.gz" +
		" | tar xzf - -C .tmp .#{@prefix}")
	ExecCmd("tar cf - -C .tmp#{@prefix} . | tar xvf - -C #{@target_prefix}")
	CleanTmpdir()
    end

    # Automatic install package file depends on file extention
    def InstallFileAuto(file)
	file =~ /(\.[a-zA-Z0-9]+)$/
	ext = $1

	case ext
	when ".xpkg"
	    InstallGenPkg(file)

	when ".xdevpkg"
	    InstallGenPkgDevel(file)

	when ".ipk", ".ipkg"
	    if (file =~ /\-devel_/)
		InstallIpkgDevel(file)
	    else
		InstallIpkg(file)
	    end

	else
	    puts "Unknown file type : #{file}"
	    exit 1
	end
    end
end


# Package feed class
class PkgFeed
    include ExecScript

    def initialize(url, pkgdir)
	@url = url
	@pkgdir = pkgdir
    end

    # Download Index file
    def FetchIndex
	if (@url == nil)
	    puts "You must specify $feed_url or $feed_devel_url."
	    exit 1
	end

	if (!FileTest.exist?(@pkgdir))
	    puts "#{@pkgdir} does not exist!"
	    exit 1
	end
	Dir.chdir(@pkgdir)

	# download Package file
	if (FileTest.exist?("Packages"))
	    File.rename("Packages", "Packages.old")
	end
	ExecCmd("wget #{@url}/Packages")
    end

    # Load Index file
    def LoadIndex
	@filelist = Array.new

	open("Packages").each do |f|
	    f.each do |line|
		line.chop!
		if (line =~ /Filename:\s*(\S+)/)
		    @filelist.push($1)
		end
	    end
	end
    end

    # Update packages
    #  Download missing files and install
    def UpdatePackages(installer)
	@filelist.each do |f|
	    next if (FileTest.exist?(f))
	    puts "#{f} does not exist. update..." if ($verbose)
	    ExecCmd("wget #{@url}/#{f}")

	    installer.InstallFileAuto(f) if (installer)
	end
    end

    def dump
	@filelist.each do |f|
	    puts f
	end
    end
end



