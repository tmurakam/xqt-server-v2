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
# deffile class
#	

require "ifstack.rb"

# Section class
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

    # Get section values (split lines into array, remove null line)
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

    # Get control parameters (only control section)
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

# Section manager class
class Sections
    def initialize
	@sections = Array.new
    end

    # get section
    def getSection(name, pkgname = nil)
	@sections.each do |section|
	    if (section.name == name && section.pkgname == pkgname)
		return section
	    end
	end
	return nil
    end

    # create new section
    def newSection(name, pkgname = nil)
	section = getSection(name, pkgname)
	@sections.delete_if {|x| x == section} if (section != nil)
	
	section = Section.new(name, pkgname)
	@sections.push(section)
	return section
    end

    # Get subpackage list
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

# Deffile class
class DefFile
    def initialize
	@defines = Hash.new
	@sections = Sections.new
    end

    def setTarget(target)
	@target = target
    end

    # load deffile from file
    def load_from_file(deffile)
	ifs = IfStack.new(@target)
	prev = nil
#	cursect = @sections.newSection("header", nil)
	cursect = @sections.newSection("control", nil)

	@filename = deffile
	@lineno = 0

	STDERR.puts "Loading #{deffile}" if ($verbose)

	open(deffile).each do |line|
	    line.chop!
	    @lineno = @lineno + 1

	    # skip comment
	    next if (line =~ /^#/)

	    # continuous line
	    if (prev != nil)
		line = prev + " " + line.strip
		prev = nil
	    end
	    if (line =~ /^(.*)\\\s*$/)
		prev = $1
		next
	    end
	    
	    # expand defines
	    line = expand_str(line)

	    # conditions
	    next if (!ifs.parse(line))

	    if (line =~ /^%define\s+(\S+)\s+(.*)$/)
		# %define
		@defines[$1] = $2.strip

	    elsif (line =~ /^%define\s+(\S+)$/)
		# %define
		@defines[$1] = ""

	    elsif (line =~ /%env\s+(\S+)\s+(.*)$/)
		# %env
		ENV[$1] = $2
		
	    elsif (line =~ /^%(\w+)\s+(\S+)/)
		# %section (with sub package name)
		section = $1
		pkgname = $2
		cursect = @sections.newSection(section, pkgname)
		
	    elsif (line =~ /^%(\w+)/)
		# %section
		section = $1
		cursect = @sections.newSection(section, nil)
		
	    else
		# generic
		cursect.push_line(line)
	    end
	end

	createDevelPkgSection
    end

    # Automatically create develpkg section from control section
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
	
    # set define value
    def setDefine(k, v)
	@defines[k] = v
    end

    # set define values from hash
    def setDefines(hash)
	hash.each do |k, v|
	    @defines[k] = v
	end
    end

    # get define value
    def getDefine(k)
	return @defines[k]
    end

    # get section value
    def getSection(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.sectval
    end

    # get section value (split into array, remove null line)
    def getSectValues(section, pkgname = nil)
	s = @sections.getSection(section, pkgname)
	return nil if (s == nil)
	
	return s.getValues
    end

    # get control parameters
    def getControlParam(name, pkgname = nil)
	s = @sections.getSection("control", pkgname)
	return nil if (s == nil)

	return s.getControlParam(name)
    end

    # get package name list
    #   extrace from package name of each sections.
    def getPackageNames(name)
	return @sections.getPackageNames(name)
    end

    # private method

    # expand define and enviroment variables
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

    # debug ...
    def showsection(section)
	print @sectval[section], "\n"
    end
end
