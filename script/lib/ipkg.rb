#
# ipkg module
#
module Ipkg
    # IPKG 作成
    def BuildIpkg
	@destdir = GetDefine("destdir")
	ipkgdir  = GetDefine("ipkgdir")
	@tmpdir = ipkgdir + "/.ipktmp"

	# パッケージ作成用ディレクトリ
	if (FileTest.exist?(@tmpdir))
	    system("/bin/rm -rf #{@tmpdir}")
	end	
	Dir.mkdir(@tmpdir)

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

	# preipkg セクション実行
	ExecSection("preipkg")

	# パッケージ生成
	system("cd #{ipkgdir} && #{$ipkg_build} .ipktmp")
    end

    # ファイルのコピー
    def CopyFiles
	return if (@sections["files"] == nil)

	builddir = GetDefine("builddir")

	@sections["files"].each do |f|
	    f.strip

	    # "!xxx" のように先頭に '!' がついている場合はカレントディレクトリからコピー。
	    # ついていない場合は %{destdir} からコピー
	    if (f =~ /^!(.*)/)
		f = $1
		srcdir = "."
	    else
		srcdir = @destdir
	    end

	    # 先頭の '/' を抜く
	    f.sub!(/^\//, "")

	    # "usr/etc/* etc/" のような場合は、第２引数の destination にコピー
	    # 第２引数がない場合は、同一の構造を保つ
	    if (f =~ /^(\S+)\s+\/?(\S+)$/)
		srcfiles = "#{srcdir}/#{$1}"
		dest     = "#{@tmpdir}/#{$2}"
	    else
		srcfiles = "#{srcdir}/#{f}"
		dest     = "#{@tmpdir}/#{File.dirname(f)}/"
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
	    if (ex =~ /^!(.*)/)
		ex = $1
		ex.sub!(/^\//, "")
		cmdline = "cd #{@tmpdir}; #{cmd} #{ex}"
	    else
		ex.sub!(/^\//, "")
		cmdline = "cd #{@tmpdir}; find . -name '#{ex}' -exec #{cmd} {} \\;";
	    end
	    p cmdline
	    system(cmdline) if (!$debug)
	end
    end	

    def PutControlFile(name, attr)
	return if (@sections[name] == nil)

	controldir = @tmpdir + "/CONTROL"
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


