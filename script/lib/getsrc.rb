#
# ��������ɻٱ�⥸�塼��
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

	# patchlevel �Υ����å�
	if (filespec =~ /(.*);patch=(\d+)/)
	    filespec = $1
	    patchlevel = $2
	end

	# �ǥ��쥯�ȥ�̾�ȥե�����̾��ʬΥ���Ƥ���
	dirname  = File.dirname(filespec)
	basename = File.basename(filespec)

	sitelist = nil
	if (filespec =~ /^local:(?:\/\/)?(.*)/)
	    # ������ե�����
	    path = $1
	else
	    # �����ȥꥹ�Ȥ����
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

    # �ͥåȥ����ͳ�ǥ��������
    def downfile(filename, sitelist)
	# ��������ɥǥ��쥯�ȥ�Υ����å�
	distfiledir = GetDefine("distfiledir")
	if (!FileTest.exist?(distfiledir))
	    Dir.mkdir(distfiledir)
	end

	path = distfiledir + "/" + filename

	# �ե���������å�
	return path if (FileTest.exist?(path))

	# ��������ɤ���
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


