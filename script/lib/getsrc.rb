#
# ダウンロード支援モジュール
#

module GetSource
    def GetSource(sources, sites)
	return if (sources == nil)
	sources.each do |source|
	    path, patchlevel = download(source, sites)

	    if (patchlevel == nil)
		extract(path)
	    else
		patch(path, patchlevel)
	    end
	end
    end

    private

    def download(filespec, sites)
	patchlevel = nil

	# patchlevel のチェック
	if (filespec =~ /(.*);patch=(\d+)/)
	    filespec = $1
	    patchlevel = $2
	end

	# ディレクトリ名とファイル名を分離しておく
	dirname  = File.dirname(filespec)
	basename = File.basename(filespec)

	sitelist = nil
	if (filespec =~ /^local:(?:\/\/)?(.*)/)
	    # ローカルファイル
	    path = $1
	else
	    # サイトリストを決める
	    sitelist = nil
	    if (dirname =~ /^(?:ht|f)tps?:\/\//)
		sitelist = [dirname]
	    else
		sitelist = sites.dup
	    end

	    fallback = GetDefine("fallback_url")
	    if (fallback != nil) 
		    sitelist.push(fallback)
	    end
	    path = downfile(basename, sitelist)
	end

	path = File.expand_path(path)
	return path ,patchlevel
    end

    # ネットワーク経由でダウンロード
    def downfile(filename, sitelist)
	# ダウンロードディレクトリのチェック
	distfiledir = GetDefine("distfiledir")
	if (!FileTest.exist?(distfiledir))
	    Dir.mkdir(distfiledir)
	end

	path = distfiledir + "/" + filename

	# ファイルチェック
	return path if (FileTest.exist?(path))

	# ダウンロードする
	sitelist.each do |site|
	    cmdline = "(cd #{distfiledir}; wget #{site}/#{filename})"
	    system(cmdline)
	    
	    return path if (FileTest.exist?(path))
	end

	return nil
    end

    def extract(path)
	cmd = nil
	if (path =~ /\.tgz$/ || path =~ /\.tar\.gz$/)
	    cmd = "tar xvzf"
	elsif (path =~ /\.tbz2?$/ || path =~ /\.tar\.bz2?$/)
	    cmd = "tar xvjf"
	else
	    STDERR.puts "How should I do? : #{path}"
	    return
	end

	system(cmd + " " + path)
    end

    def patch(path, patchlevel)
	cat = "cat"
	if (path =~ /\.g?z$/)
	    cat = "zcat"
	elsif (path =~ /\.bz2?$/)
	    cat = "bzcat"
	end

	cmdline = "(cd #{@defines["builddir"]} && #{cat} #{path} | patch -p#{patchlevel})"
	system(cmdline)
    end
end


