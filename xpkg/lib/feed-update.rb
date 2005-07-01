#!/usr/bin/ruby

# feed-update.rb: feed updater for X/Qt project
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

def analyze_ipkg(file)
    size = 0
    md5sum = 'unknown'
    control = ''
    license = ''
    section = ''
    nocard = false
    desc = ''

    begin
	# get file size / md5sum
	size = FileTest.size(file)
	md5sum = `md5sum #{file} | awk '{print $1}'`
	md5sum.chop!

	# load/parse control file
	is_desc = false
	`tar -xzOf #{file} ./control.tar.gz | tar -xzOf - ./control`.each do |line|
	    line.chop!

	    next if (line =~ /^\s*$/)
	    if (line =~ /^Description:/) 
		is_desc = true
	    end
	    if (!is_desc)
		control = control + line + "\n"
	    else
		desc = desc + line + "\n"
	    end

	    # check license
	    if (line =~ /^License:\s+(.*)$/)
		license = $1
	    end
	    if (line =~ /^Section:\s+(.*)$/)
		section = $1
	    end
	end

	control = control + "Filename: #{file}\n"
	control = control + "Size: #{@size}\n"
	control = control + "MD5Sum: #{md5sum}\n"
	control = control + desc

    rescue
	# nothing to do
    end

    return control
end

class FileInfo
    attr_reader :fname, :name, :ver

    def initialize(fname)
	@fname = fname
	if (fname =~ /^(.*)_(.*)_(.*)\..*/)
	    @name = $1
	    @ver = $2
	    @arch = $3
	end
    end

    def compare_version(target)
	ver1s = @ver.split(/[\.\-]/)
	ver2s = target.ver.split(/[\.\-]/)

	while (true)
	    v1 = ver1s.shift
	    v2 = ver2s.shift

	    return  0 if (v1 == nil && v2 == nil)
	    return  1 if (v1 != nil && v2 == nil)
	    return -1 if (v1 == nil && v2 != nil)

	    if (v1 != v2)
		if (v1.to_i < v2.to_i)
		    return -1
		end
		return 1
	    end
	end
	return 0
    end

    def remove
	puts "remove file: #{@fname}"
	if (!FileTest.exist?("old"))
	    Dir.mkdir("old")
	end
	system("/bin/mv #{@fname} old/")
    end
end

def PurgePackages
    # check all packages...
    files = Dir.glob("*.xpkg *.xdevpkg *.ipk")

    filelist = Hash.new

    # flush old files
    puts "Purging old files..."
    files.each do |f|
	f = FileInfo.new(f)
	
	name = f.name
	if (filelist[name] == nil)
	    filelist[name] = f
	else
	    if (f.compare_version(filelist[name]) > 0)
		# remove old one
		filelist[name].remove()
		filelist[name] = f
	    else
		f.remove()
	    end
	end
    end
end

def GenPackages
    open("Packages", "w") do |fh|
        # for xpkg system
        Dir.glob("*.xpkg *.xdevpkg") do |k|
            fh.puts "Filename: #{k}"
            fh.puts
        end

        # for ipkg system
        Dir.glob("*.ipk").sort.each do |ipk|
            fh.puts analyze_ipkg(ipk)
            fh.puts
        end
    end
end

# start from here..
PurgePackages()
GenPackages()
puts "done."
