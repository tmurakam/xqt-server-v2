# xpkg: Package build system for X/Qt project
#
# Copyright (C) 2003-2005 Takuya Murakami
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#
# deffile を扱うクラス
#	

require "ifstack.rb"

# セクションを扱うクラス
class Section
    attr_reader :name, :pkgname
    attr_reader :sectval

    def initialize(name, pkgname)
	@name = name
	@pkgname = pkgname
	@sectval = ""
    end

    def push_line(line)
	@sectval += line + "\n"
    end

    def setval(val)
	@sectval = val
    end

    # section 値の取得 (array に分割、空行削除)
    def getValues
	ary = Array.new
	@sectval.each do |line|
	    line.strip!
	    if (line != "")
		ary.push(line)
	    end
	end
	return ary
    end

    # control パラメータを取得する (control セクションのみ)
    def getControlParam(name)
	@sectval.each do |line|
	    line.strip!
	    if (line =~ /^#{name}:(.*)/)
		param = $1
		return param.strip
	    end
	end
	return nil
    end

    def dump
	STDERR.print @sectval
    end
end

class Sections
    def initialize
	@sections = Array.new
    end

    # セクション取得
    def getSection(name, pkgname = nil)
	@sections.each do |section|
	    if (section.name == name && section.pkgname == pkgname)
		return section
	    end
	end
	return nil
    end

    # セクション作成
    def newSection(name, pkgname = nil)
	section = getSection(name, pkgname)
	return section if (section)
	
	section = Section.new(name, pkgname)
	@sections.push(section)
	return section
    end

    # サブパッケージ名一覧を取得
    def getPackageNames(name)
	pkgnames = Array.new
	@sections.each do |section|
	    if (section.name == name)
		pkgnames.push(section.pkgname)
	    end
	end
	return pkgnames
    end

end

class DefFile
    def initialize
	@defines = Hash.new
	@sections = Sections.new
    end

    def setTarget(target)
	@target = target
    end

    # 定義ファイルのロード
    def load_from_file(deffile)
	ifs = IfStack.new(@target)
	prev = nil
	cursect = @sections.newSection("header", nil)

	@filename = deffile
	@lineno = 0

	STDERR.puts "Loading #{deffile}" if ($verbose)

	open(deffile).each do |line|
	    line.chop!
	    @lineno = @lineno + 1

	    # コメントのスキップ
	    next if (line =~ /^#/)

	    # 継続行処理
	    if (prev != nil)
		line = prev + " " + line.strip
		prev = nil
	    end
	    if (line =~ /^(.*)\\\s*$/)
		prev = $1
		next
	    end
	    
	    # 条件節処理
	    next if (!ifs.parse(line))

	    if (line =~ /^%define\s+(\S+)\s+(.*)$/)
		# %define 処理
		@defines[$1] = expand_str($2).strip

	    elsif (line =~ /^%define\s+(\S+)$/)
		# %define 処理
		@defines[$1] = ""

	    elsif (line =~ /%env\s+(\S+)\s+(.*)$/)
		# %env 処理
		ENV[$1] = expand_str($2)
		
	    elsif (line =~ /^%(\w+)\s+(\S+)/)
		# %section 処理 (pkgname つき)
		section = $1
		pkgname = expand_str($2)
		cursect = @sections.newSection(section, pkgname)
		
	    elsif (line =~ /^%(\w+)/)
		# %section 処理
		section = $1
		cursect = @sections.newSection(section, nil)
		
	    else
		# 一般処理
		line = expand_str(line)
		cursect.push_line(line)
	    end
	end

	createDevelPkgSection
    end

    # develpkg 指定処理
    # デフォルトの control セクションから新規セクションを生成する
    def createDevelPkgSection
	dpkg = getDefine("develpkg")
	if (dpkg)

	    default_sect = @sections.getSection("control", nil)

	    control = ""
	    default_sect.sectval.each do |v|
		v.chop!
		if (v =~ /Package:(.*)\s*/)
		    v = "Package:#{$1}-devel"
		elsif (v =~ /Description:(.*)/)
		    v = "Description: #{$1} (development package)"
		end
		control = control + v + "\n"
	    end

	    sect = @sections.newSection("control", "develpkg")
	    sect.setval(control)

	    sect = @sections.newSection("files", "develpkg")
	    sect.push_line("*")
	end
    end
	
    # define 値をセットする
    def setDefine(k, v)
	@defines[k] = v
    end

    # define 値をセットする (複数)
    def setDefines(hash)
	hash.each do |k, v|
	    @defines[k] = v
	end
    end

    # define 値の取得
    def getDefine(k)
	return @defines[k]
    end

    # section 値の取得
    def getSection(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.sectval
    end

    # section 値の取得 (array に分割、空行削除)
    def getSectValues(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.getValues
    end

    # control 値の取得
    def getControlParam(name, pkgname = nil)
	s = @sections.getSection("control", pkgname)
	return nil if (s == nil)

	return s.getControlParam(name)
    end

    # パッケージ名一覧の取得
    # セクションのパッケージ名一覧から取得する
    def getPackageNames(name)
	return @sections.getPackageNames(name)
    end

    # 以下は private method

    # define と環境変数の展開
    def expand_str(s)
	s.gsub!(/%\{(\w+)\}/) { 
	    if (@defines[$1] == nil)
		puts "Error: Undefined symbol '#{$1}' in #{@filename} line #{@lineno}"
		exit 1
	    else
		"#{@defines[$1]}"
	    end
	}
	s.gsub!(/\$\{(\w+)\}/) { 
	    if (ENV[$1] == nil)
		puts "Error: Undefined environment var '#{$1}' in #{@filename} line #{@lineno}"
		exit 1
	    else
		"#{ENV[$1]}"
	    end
	}
	return s
    end

    # debug 用...
    def showsection(section)
	print @sectval[section], "\n"
    end
end
