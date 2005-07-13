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
# IPKG
#

class IpkgBuild < PkgBuild

    def initialize
	@pkgsuffix = ".ipk"
	@devpkgsuffix = ".ipk"
    end

    def SetIpkgParam(ipkg_build)
	@ipkg_build = ipkg_build
    end

    # private method
    private

    # setup ipkg file
    def SetupFiles(pkg)
	super

	# create control files
	GenControl(pkg, "control",   0644);
	GenControl(pkg, "preinst",   0755);
	GenControl(pkg, "postinst",  0755);
	GenControl(pkg, "postrm",    0755);
	GenControl(pkg, "conffiles", 0644);
    end

    def GenControl(pkg, section, perm)
	data = @df.getSection(section, pkg)
	return if (data == nil || data == "")

	if (! FileTest.exist?(@pkgtmpdir + "/CONTROL"))
	    Dir.mkdir(@pkgtmpdir + "/CONTROL")
	end

	path = @pkgtmpdir + "/CONTROL/" + section
	open(path, "w") do |fh|
	    fh.print data
	end
	File.chmod(perm, path)
    end

    # generate package file
    def GenPkgFile(pkg)
	if (isDevelPkg?(pkg))
	    cmd = "cd #{@develpkgdir} && #{$fakeroot} #{@ipkg_build} pkgtmp"
	else
	    cmd = "cd #{@pkgdir} && #{$fakeroot} #{@ipkg_build} pkgtmp"
	end
	ExecCmd(cmd)
    end

    # install packages locally
    def InstallLocalOne(pkg, dest)
	fname = getPkgFilename(pkg)
	@installer.InstallIpkg("#{@pkgdir}/#{fname}")
    end

    # install development package
    def InstallDevelPackageOne(pkg, dest)
	fname = getPkgFilename(pkg)
	@installer.InstallIpkgDevel("#{@develpkgdir}/#{fname}")
    end
end
