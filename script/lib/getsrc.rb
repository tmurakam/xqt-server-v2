#
# ダウンロード支援モジュール
#

module GetSource
    def SetDistFileDir(distfiledir)
	@distfiledir = distfiledir
	if (!FileTest.exist?(@distfiledir))
	    Dir.mkdir(@distfiledir)
	end
    end

    def GetSource(sources)
	sources.each do |source|
	    path, patchlevel = download(source)

	    if (patchlevel == nil)
		extract(path)
	    else
		patch(path, patchlevel)
	    end
	end
    end

    private

    def download(filespec)
	patchlevel = nil

	# patchlevel のチェック
	if (filespec =~ /(.*);patch=(\d+)/)
	    filespec = $1
	    patchlevel = $2
	end

	# file名部分を取り出しておく
	filespec =~ /([^\/]+)$/
	filename = $1

	if (filespec =~ /^(?:ht|f)tp(s?):/)
	    # ダウンロード済みかチェック
	    path = @distfiledir + "/" + filename
	    if (!FileTest.exist?(path))
		# ダウンロードする
		cmdline = "(cd #{@distfiledir}; wget #{filespec})"
		system(cmdline)
	    end
	else
	    # ローカルファイル
	    path = filespec
	end

	path = File.expand_path(path)
	return path ,patchlevel
    end

    def extract(path)
	cmd = nil
	if (path =~ /\.tgz$/ || path =~ /\.tar\.gz$/)
	    cmd = "tar xvzf"
	elsif (path =~ /\.tbz2?$/ || /\.tar\.bz2?$/)
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


