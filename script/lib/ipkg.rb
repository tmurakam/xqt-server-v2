#
# ipkg module
#
module Ipkg
    # IPKG ����
    def BuildIpkg
	@destdir = GetDefine("destdir")
	ipkgdir  = GetDefine("ipkgdir")
	@tmpdir = ipkgdir + "/.ipktmp"

	# �ѥå����������ѥǥ��쥯�ȥ�
	if (FileTest.exist?(@tmpdir))
	    system("/bin/rm -rf #{@tmpdir}")
	end	
	Dir.mkdir(@tmpdir)

	# �ե����륳�ԡ�
	CopyFiles()

	# ���ץե�����κ��
	ProcessFiles("excludes", "/bin/rm -rf")

	# strip �¹�
	ProcessFiles("strip", "/opt/Embedix/tools/bin/arm-linux-strip")

	# control �ե����뷲���ԡ�
	PutControlFile("control", 0644)
	PutControlFile("preinst", 0755)
	PutControlFile("postinst", 0755)
	PutControlFile("postrm", 0755)
	PutControlFile("conffiles", 0644)

	# preipkg ���������¹�
	ExecSection("preipkg")

	# �ѥå���������
	system("cd #{ipkgdir} && #{$ipkg_build} .ipktmp")
    end

    # �ե�����Υ��ԡ�
    def CopyFiles
	return if (@sections["files"] == nil)

	builddir = GetDefine("builddir")

	@sections["files"].each do |f|
	    f.strip

	    # "!xxx" �Τ褦����Ƭ�� '!' ���Ĥ��Ƥ�����ϥ����ȥǥ��쥯�ȥ꤫�饳�ԡ���
	    # �Ĥ��Ƥ��ʤ����� %{destdir} ���饳�ԡ�
	    if (f =~ /^!(.*)/)
		f = $1
		srcdir = "."
	    else
		srcdir = @destdir
	    end

	    # ��Ƭ�� '/' ��ȴ��
	    f.sub!(/^\//, "")

	    # "usr/etc/* etc/" �Τ褦�ʾ��ϡ��裲������ destination �˥��ԡ�
	    # �裲�������ʤ����ϡ�Ʊ��ι�¤���ݤ�
	    if (f =~ /^(\S+)\s+\/?(\S+)$/)
		srcfiles = "#{srcdir}/#{$1}"
		dest     = "#{@tmpdir}/#{$2}"
	    else
		srcfiles = "#{srcdir}/#{f}"
		dest     = "#{@tmpdir}/#{File.dirname(f)}/"
	    end

	    # �裲������ '/' �ǽ���äƤ�����ϥǥ��쥯�ȥ�̾��
	    # �����Ǥʤ����ϥե��������Ȥߤʤ�
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

    # excludes/strip �ʤɤΥե��������
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


