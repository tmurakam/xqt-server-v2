#
# DefFile クラス
#
module DefFile
    def initialize
	@sections = Hash.new
	@defines = Hash.new
    end

    # define の手動定義
    def SetDefine(k, v)
	@defines[k] = v
    end

    def GetDefine(k)
	return @defines[k]
    end

    # define の展開
    def expand_define(str)
	str.gsub!(/%{(\w+)}/) {|key|
	    if (@defines[$1] != nil)
		@defines[$1]
	    else
		""
	    end
	}
	return str
    end

    # pkgdef ファイルのロード
    def Load(file)
	section = nil

	prev_line = nil

	File.open(file).each do |line|
	    line.chop!

	    # Skip comments
	    next if (line =~ /^#/)

	    # 継続行
	    if (prev_line != nil)
		line = prev_line + " " + line.strip
		prev_line = nil
	    end
	    if (line =~ /^(.*)\\\s*$/)
		prev_line = $1
		next
	    end

	    line = expand_define(line)

	    if (line =~ /^%define\s+(\S+)\s+(.*)/)
		# %define 処理
		@defines[$1] = $2

	    elsif (line =~ /^%env\s+(\S+)\s+(.*)/)
		# %env 処理
		ENV[$1] = $2
		
	    elsif (line =~ /^%(\w+)/)
		# セクション名
		section = $1

	    else
		if (@sections[section] == nil)
		    @sections[section] = Array.new
		end
		@sections[section].push(line)
	    end				
	end

	# remove null lines
	@sections.keys.each do |key|
	    while (@sections[key].size > 0)
		last = @sections[key].pop
		if (last !~ /^\s*$/)
		    @sections[key].push(last)
		    break
		end
	    end
	end
    end
end
