#
# DefFile ���饹
#
module DefFile
    def initialize
	@sections = Hash.new
	@defines = Hash.new
    end

    # define �μ�ư���
    def SetDefine(k, v)
	@defines[k] = v
    end

    def GetDefine(k)
	return @defines[k]
    end

    # define ��Ÿ��
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

    # pkgdef �ե�����Υ���
    def Load(file)
	section = nil

	prev_line = nil

	File.open(file).each do |line|
	    line.chop!

	    # Skip comments
	    next if (line =~ /^#/)

	    # ��³��
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
		# %define ����
		@defines[$1] = $2

	    elsif (line =~ /^%env\s+(\S+)\s+(.*)/)
		# %env ����
		ENV[$1] = $2
		
	    elsif (line =~ /^%(\w+)/)
		# ���������̾
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
