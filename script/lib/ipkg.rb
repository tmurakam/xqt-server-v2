#
# ipkg module
#
module Ipkg
    # IPKG ����
    def BuildPkg
	# �ѥå������ǥ��쥯�ȥ����
	@pkgdir = Dir.pwd + "/.dist"

	if (FileTest.exist?(@pkgdir))
	    system("/bin/rm -rf #{@pkgdir}")
	end	
	Dir.mkdir(@pkgdir)

	SetPkgDir(@pkgdir)

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

	# exec ���������¹�
	ExecSection("exec")

	# �ѥå���������
	system("ipkg-build .dist")
    end

    # �ե�����Υ��ԡ�
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

    # excludes/strip �ʤɤΥե��������
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


