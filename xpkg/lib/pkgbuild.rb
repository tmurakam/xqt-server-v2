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
# �ѥå�������������
#

# ���쥯�饹
class PkgBuild
    include ExecScript

    def initialize
	@pkgsuffix = nil
	@devpkgsuffix = nil
    end

    def SetParam(df, srcdir, pkgdir, develpkgdir, abspath)
	@df = df
	@srcdir = srcdir
	@pkgdir = pkgdir
	@develpkgdir = develpkgdir
	@abspath = abspath

	@installer = PkgInstall.new
	@installer.SetParamFromDef(@df)
    end

    # �ѥå�����������
    def Build
	pkgnames = @df.getPackageNames("control")
	pkgnames.each do |subpkg|
	    p = @df.getControlParam("Package", subpkg)
	    puts "-- Creating Package: #{p}"

	    SetupFiles(subpkg)
	    GenPkgFile(subpkg)
	    puts
	end
    end

    # �ѥå������Υ����륤�󥹥ȡ���
    def InstallLocal
	root = @df.getDefine("target_root")
	if (root == nil)
	    STDERR.puts "You must set target_root in deffile."
	    exit 1
	end

	if (!FileTest.exist?(root))
	    system "mkdir -p #{root}"
	end

	pkgnames = @df.getPackageNames("control")
	pkgnames.each do |subpkg|
	    p = @df.getControlParam("Package", subpkg)
	    next if (p =~ /-devel$/)  # ��ȯ�ѥå������ϥ����å�

	    puts "-- Installing Package: #{p}"
	    InstallLocalOne(subpkg, root)
	end
    end

    # ��ȯ�ѥå������Υ��󥹥ȡ���
    def InstallDevelPackage
	root = @df.getDefine("target_prefix")
	if (root == nil)
	    STDERR.puts "You must set target_prefix in deffile."
	    exit 1
	end

	if (!FileTest.exist?(root))
	    system "mkdir -p #{root}"
	end

	pkgnames = @df.getPackageNames("control")
	pkgnames.each do |subpkg|
	    p = @df.getControlParam("Package", subpkg)
	    next if (p !~ /-devel$/)  # ��ȯ�ѥå������ʳ��ϥ����å�

	    puts "-- Installing Package: #{p}"
	    InstallDevelPackageOne(subpkg, root)
	end
    end

    # �ʲ��� private method
    private

    # �ѥå������Υե�����̾������
    def getPkgFilename(subpkg)
	pkg = nil
	arch = nil
	ver = nil

	pkg  = @df.getControlParam("Package", subpkg)
	arch = @df.getControlParam("Architecture", subpkg)
	ver  = @df.getControlParam("Version", subpkg)

	if (pkg != nil && arch != nil && ver != nil) 
	    fname = pkg + "_" + ver + "_" + arch

	    if (isDevelPkg?(subpkg))
		fname += @devpkgsuffix
	    else
		fname += @pkgsuffix
	    end
	    return fname
	end
	return nil
    end

    # �ѥå��������̤μ���
    def isDevelPkg?(subpkg)
	pkg = @df.getControlParam("Package", subpkg)
	if (pkg =~ /-devel$/)
	    return true
	end
	return false
    end

    # �ե�����Υ��åȥ��å�
    def SetupFiles(subpkg)
	@files     = @df.getSectValues("files", subpkg)
	@excludes  = @df.getSectValues("excludes", subpkg)
	@strip     = @df.getSectValues("strip", subpkg)

	# �ƥ�ݥ��ǥ��쥯�ȥ������
	if (isDevelPkg?(subpkg))
	    @pkgtmpdir = @develpkgdir + "/pkgtmp"
	else
	    @pkgtmpdir = @pkgdir + "/pkgtmp"
	end
	if (FileTest.exist?(@pkgtmpdir))
	    system "/bin/rm -rf #{@pkgtmpdir}"
	end
	ExecCmd("mkdir -p #{@pkgtmpdir}")

	CopyFiles()
	Excludes()
	Strip()
    end

    # �����ե����륳�ԡ�����
    def CopyFiles
	return if (@files == nil)
	@files.each do |filespec|
	    # !file �ξ�硢���ԡ����� abspath ����
	    if (filespec =~ /^\!(.*)/)
		filespec = $1
		srcdir = @abspath
	    else
		srcdir = @srcdir
	    end
	    
	    # "file dir" ����ξ��ϡ�dir �˥��󥹥ȡ���
	    # "file" ����ξ���Ʊ������Хǥ��쥯�ȥ�˥��󥹥ȡ���
	    if (filespec =~ /(\S+)\s+(\S+)/ || filespec =~ /(\S+):(\S+)/)
		srcfiles = srcdir + "/" + $1
		dest = @pkgtmpdir + "/" + $2 + "/"
	    else
		srcfiles = srcdir + "/" + filespec
		dest = @pkgtmpdir + "/" + File.dirname(filespec) + "/"
	    end

	    if (!FileTest.exist?(dest))
		ExecCmd("mkdir -p #{dest}")
	    end

	    cmd = "cp -d -r #{srcfiles} #{dest}"
	    ExecCmd(cmd)
	end
    end

    # ���ץե�����κ��
    def Excludes
	return if (@excludes == nil)
	@excludes.each do |file|
	    if (file =~ /^\// || file.include?("/"))
		# path
		cmd = "/bin/rm -rf #{@pkgtmpdir}#{file}"
	    else
		# file spec
		cmd = "find #{@pkgtmpdir} -name '#{file}' -exec /bin/rm -rf {} \\;"
	    end
	    ExecCmd(cmd, true)
	end
    end
    
    # strip ����
    def Strip
	return if (@strip == nil)
	strip = ENV["STRIP"]
	if (strip == nil)
	    strip = "/usr/bin/strip"
	end
	@strip.each do |file|
	    cmd = "#{strip} #{@pkgtmpdir}/#{file}"
	    ExecCmd(cmd, true)
	end
    end
    
    # tarball �κ���
    def GenPkgFile(pkg)
	# pure virtual
    end

    def InstallLocalOne(pkg, dest)
	# pure virtual
    end

    def InstallDevelPackageOne(pkg, dest)
	# pure virtual
    end

end

# ���̥ѥå������ӥ�� (ipkg �ʳ��Υ�����)
class GenPkgBuild < PkgBuild
    def initialize
	@pkgsuffix = ".xpkg"
	@devpkgsuffix = ".xdevpkg"
    end

    # tarball �κ���
    def GenPkgFile(subpkg)
	fname = getPkgFilename(subpkg)
	dest = @pkgdir
	if (isDevelPkg?(subpkg))
	    dest = @develpkgdir
	end
        cmd = "cd #{@pkgtmpdir} && #{$fakeroot} tar cvzf #{dest}/#{fname} ."
	ExecCmd(cmd)
    end

    def InstallLocalOne(subpkg, dest)
	fname = getPkgFilename(subpkg)
	@installer.InstallGenPkg("#{@pkgdir}/#{fname}")
    end

    def InstallDevelPackageOne(subpkg, dest)
	fname = getPkgFilename(subpkg)
	@installer.InstallGenPkgDevel("#{@develpkgdir}/#{fname}")
    end
end

