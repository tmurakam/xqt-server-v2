#
# Pkgdef クラス
#
require "deffile"
require "getsrc"
require "ipkg"

class Pkgdef
    include DefFile
    include Ipkg
    include GetSource

    # スクリプト実行
    def ExecSection(sect)
	return if (@sections[sect] == nil)

	if ($debug)
	    puts @sections[sect].join("\n")
	    return
	end

	builddir = @defines["builddir"]
	open("#{builddir}/tmp.sh", "w") do |fh|
	    @sections[sect].each do |line|
		fh.puts line
	    end
	end
	system("cd #{builddir} && /bin/sh tmp.sh")
    end

    # ソース取得と展開処理
    def getSource
	GetSource(@sections["source"])
    end

    def Dump
	p @sections
    end
end


