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
# パッケージ生成処理
#

# 基底クラス
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

    # パッケージの生成
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

    # パッケージのローカルインストール
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
	    next if (p =~ /-devel$/)  # 開発パッケージはスキップ

	    puts "-- Installing Package: #{p}"
	    InstallLocalOne(subpkg, root)
	end
    end

    # 開発パッケージのインストール
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
	    next if (p !~ /-devel$/)  # 開発パッケージ以外はスキップ

	    puts "-- Installing Package: #{p}"
	    InstallDevelPackageOne(subpkg, root)
	end
    end

    # 以下は private method
    private

    # パッケージのファイル名を得る
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

    # パッケージ種別の取得
    def isDevelPkg?(subpkg)
	pkg = @df.getControlParam("Package", subpkg)
	if (pkg =~ /-devel$/)
	    return true
	end
	return false
    end

    # ファイルのセットアップ
    def SetupFiles(subpkg)
	@files     = @df.getSectValues("files", subpkg)
	@excludes  = @df.getSectValues("excludes", subpkg)
	@strip     = @df.getSectValues("strip", subpkg)

	# テンポラリディレクトリを生成
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

    # 生成ファイルコピー処理
    def CopyFiles
	return if (@files == nil)
	@files.each do |filespec|
	    # !file の場合、コピー元は abspath から
	    if (filespec =~ /^\!(.*)/)
		filespec = $1
		srcdir = @abspath
	    else
		srcdir = @srcdir
	    end
	    
	    # "file dir" 指定の場合は、dir にインストール
	    # "file" 指定の場合は同一の相対ディレクトリにインストール
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

    # 不要ファイルの削除
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
    
    # strip 処理
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
    
    # tarball の作成
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

# 一般パッケージビルド (ipkg 以外のケース)
class GenPkgBuild < PkgBuild
    def initialize
	@pkgsuffix = ".xpkg"
	@devpkgsuffix = ".xdevpkg"
    end

    # tarball の作成
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

