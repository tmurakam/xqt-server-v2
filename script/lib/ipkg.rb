#
# ipkg module
#
module Ipkg
    # IPKG 作成
    def BuildIpkg
	@destdir = GetDefine("destdir")
	@ipkdir = Dir.pwd + "/.ipktmp"

	# パッケージ作成用ディレクトリ
	if (FileTest.exist?(@ipkdir))
	    system("/bin/rm -rf #{@ipkdir}")
	end	
	Dir.mkdir(@ipkdir)

	SetPkgDir(@ipkdir)

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
	system("ipkg-build .ipktmp")
    end

    # ファイルのコピー
    def CopyFiles
	return if (@sections["files"] == nil)

	builddir = GetDefine("builddir")

	@sections["files"].each do |f|
	    # "!xxx" のように先頭に '!' がついている場合はカレントディレクトリからコピー。
	    # ついていない場合は %{destdir} からコピー
	    if (f =~ /^!(.*)/)
		f = $1
		srcdir = "."
	    else
		srcdir = @destdir
	    end

	    # "usr/etc/* etc/" のような場合は、第２引数の destination にコピー
	    # 第２引数がない場合は、同一の構造を保つ
	    if (f =~ /^(.*)\s+(.*)$/)
		srcfiles = "#{srcdir}/#{$1}"
		dest     = "#{@ipkdir}/#{$2}"
	    else
		srcfiles = "#{srcdir}/#{f}"
		dest     = "#{@ipkdir}/#{File.dirname(f)}/"
	    end

	    # 第２引数が '/' で終わっている場合はディレクトリ名。
	    # そうでない場合はファイル指定とみなす
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
		cmdline = "cd #{@ipkdir}; #{cmd} #{$1}"
	    else
		cmdline = "cd #{@ipkdir}; find . -name '#{ex}' -exec #{cmd} {} \\;";
	    end
	    p cmdline
	    system(cmdline) if (!$debug)
	end
    end	

    def PutControlFile(name, attr)
	return if (@sections[name] == nil)

	controldir = @ipkdir + "/CONTROL"
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


