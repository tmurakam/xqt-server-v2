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
# deffile �򰷤����饹
#	

require "ifstack.rb"

# ���������򰷤����饹
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

    # section �ͤμ��� (array ��ʬ�䡢���Ժ��)
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

    # control �ѥ�᡼����������� (control ���������Τ�)
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

    # ������������
    def getSection(name, pkgname = nil)
	@sections.each do |section|
	    if (section.name == name && section.pkgname == pkgname)
		return section
	    end
	end
	return nil
    end

    # ������������
    def newSection(name, pkgname = nil)
	section = getSection(name, pkgname)
	return section if (section)
	
	section = Section.new(name, pkgname)
	@sections.push(section)
	return section
    end

    # ���֥ѥå�����̾���������
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

    # ����ե�����Υ���
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

	    # �����ȤΥ����å�
	    next if (line =~ /^#/)

	    # ��³�Խ���
	    if (prev != nil)
		line = prev + " " + line.strip
		prev = nil
	    end
	    if (line =~ /^(.*)\\\s*$/)
		prev = $1
		next
	    end
	    
	    # ��������
	    next if (!ifs.parse(line))

	    if (line =~ /^%define\s+(\S+)\s+(.*)$/)
		# %define ����
		@defines[$1] = expand_str($2).strip

	    elsif (line =~ /^%define\s+(\S+)$/)
		# %define ����
		@defines[$1] = ""

	    elsif (line =~ /%env\s+(\S+)\s+(.*)$/)
		# %env ����
		ENV[$1] = expand_str($2)
		
	    elsif (line =~ /^%(\w+)\s+(\S+)/)
		# %section ���� (pkgname �Ĥ�)
		section = $1
		pkgname = expand_str($2)
		cursect = @sections.newSection(section, pkgname)
		
	    elsif (line =~ /^%(\w+)/)
		# %section ����
		section = $1
		cursect = @sections.newSection(section, nil)
		
	    else
		# ���̽���
		line = expand_str(line)
		cursect.push_line(line)
	    end
	end

	createDevelPkgSection
    end

    # develpkg �������
    # �ǥե���Ȥ� control ��������󤫤鿷��������������������
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
	
    # define �ͤ򥻥åȤ���
    def setDefine(k, v)
	@defines[k] = v
    end

    # define �ͤ򥻥åȤ��� (ʣ��)
    def setDefines(hash)
	hash.each do |k, v|
	    @defines[k] = v
	end
    end

    # define �ͤμ���
    def getDefine(k)
	return @defines[k]
    end

    # section �ͤμ���
    def getSection(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.sectval
    end

    # section �ͤμ��� (array ��ʬ�䡢���Ժ��)
    def getSectValues(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.getValues
    end

    # control �ͤμ���
    def getControlParam(name, pkgname = nil)
	s = @sections.getSection("control", pkgname)
	return nil if (s == nil)

	return s.getControlParam(name)
    end

    # �ѥå�����̾�����μ���
    # ���������Υѥå�����̾���������������
    def getPackageNames(name)
	return @sections.getPackageNames(name)
    end

    # �ʲ��� private method

    # define �ȴĶ��ѿ���Ÿ��
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

    # debug ��...
    def showsection(section)
	print @sectval[section], "\n"
    end
end
