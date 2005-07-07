module Misc
    # top ディレクトリをサーチ
    def gettopdir
	# check XPKGTOP environment var.
	dir = ENV["XPKGTOP"]
	return dir if (dir != nil)

	dir = $xpkgtop
	return dir if (dir != nil)

	# check _xpkgtop file
	dir = Dir.pwd
	while dir != "/" do
	    if (FileTest.exist?(dir + "/_xpkgtop"))
		return dir
	    end

	    dir = File.expand_path("..", dir)
	end

	return nil
    end

    # ディレクトリパラメータを設定する
    def setupdirs(target)
	pwd = Dir.pwd
	@destdir = pwd + "/destdir"

	topdir = gettopdir
	if (topdir == nil)
	    @distfiledir = pwd
	    @pkgdir      = pwd
	    @develpkgdir      = pwd
	else
	    @distfiledir = "#{topdir}/XPKG/distfiles"
	    @pkgdir      = "#{topdir}/XPKG/packages"
	    @develpkgdir = "#{topdir}/XPKG/develpkgs"

	    if ($use_multi_config == true)
		@pkgdir      = "#{topdir}/XPKG/#{target}/packages"
		@develpkgdir = "#{topdir}/XPKG/#{target}/develpkgs"
	    end
	end
    end
end


