#
# ipkg module
#
module Ipkg
    # IPKG 作成
    def BuildPkg
	# パッケージディレクトリ作成
	@pkgdir = Dir.pwd + "/.dist"

	if (FileTest.exist?(@pkgdir))
	    system("/bin/rm -rf #{@pkgdir}")
	end	
	Dir.mkdir(@pkgdir)

	SetPkgDir(@pkgdir)

	# ファイルコピー
	CopyFiles()

	# 不要ファイルの削除
	ProcessFiles("excludes", "/bin/rm -rf")

	# strip 実行
	ProcessFiles("strip", "/opt/Embedix/tools/bin/arm-linux-strip")

	# control ファイル群コピー
	PutControlFile("control", 0644)
	PutControlFile("preinst", 0755)
	PutControlFile("postinst", 0755)
	PutControlFile("postrm", 0755)
	PutControlFile("conffiles", 0644)

	# exec セクション実行
	ExecSection("exec")

	# パッケージ生成
	system("ipkg-build .dist")
    end

    # ファイルのコピー
    def CopyFiles
	return if (@sections["files"] == nil)

	srcdir = ENV["BUILD_ROOT"]

	@sections["files"].each do |f|
	    if (f =~ /^(.*)\s+(.*)$/)
		srcfiles = $1
		dest = @pkgdir + "/" + $2
	    else 
		srcfiles = f
		dest = @pkgdir + "/" + File.dirname(f) + "/"
	    end
	    
	    srcfiles = srcdir + "/" + srcfiles

	    if (dest =~ /\/$/) 
		system("mkdir -p #{dest}")
	    else
		system("mkdir -p #{File.dirname(dest)}")
	    end
	    
	    cmdline = "cp -d -r #{srcfiles} #{dest}"
	    p cmdline
	    system(cmdline) if (!$debug)
	end
    end

    # excludes/strip などのファイル処理
    def ProcessFiles(section, cmd)
	return if (@sections[section] == nil)
	
	@sections[section].each do |ex|
	    # does not allow full path
	    if (ex =~ /^\/(.*)/)
		ex = $1
	    end

	    if (ex =~ /^!(.*)/)
		cmdline = "cd #{@pkgdir}; #{cmd} #{$1}"
	    else
		cmdline = "cd #{@pkgdir}; find . -name '#{ex}' -exec #{cmd} {} \\;";
	    end
	    p cmdline
	    system(cmdline) if (!$debug)
	end
    end	

    def PutControlFile(name, attr)
	return if (@sections[name] == nil)

	controldir = @pkgdir + "/CONTROL"
	if (!FileTest.exist?(controldir))
	    Dir.mkdir(controldir)
	end

	file = controldir + "/" + name
	open(file, "w") do |fh|
	    @sections[name].each do |line|
		fh.puts line
	    end
	end

	File.chmod(attr, file)
    end
end


