#!/usr/bin/ruby

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
# xpkg dependency resolve library
# 

class PkgDep < Pkg

    attr_reader :names, :depends, :level

    def initialize
	super
	@level = 0
    end

    def setup
	@names = Array.new
	@depends = Array.new

	@df.getPackageNames("control").each do |subpkg|
	    @names.push(@df.getControlParam("Package", subpkg))

	    dep = @df.getControlParam("Depends", subpkg)
	    if (dep)
		dep.split("[ ,]+").each do |d|
		    d.strip!
		    @depends.push(d.strip) if (d != "")
		end
	    end
	end

	# resolve self dependency
	remove_depends(@names)
    end

    def remove_depends(names)
	names.each do |n|
	    @depends.delete(n)
	end
    end

    def setlevel(l)
	@level = l
    end

    def dump
	puts "name: #{@names.join(":")}"
	puts "depends: #{@depends.join(":")}"
    end
end

class PkgDepends
    def initialize
	@pkgs = Hash.new
    end

    def loaddeffiles
	@deflist = `find . -name "pkgdef" -print`.split
	
	@deflist.each do |f|
	    puts "Loading : #{f}" if ($verbose)
	    pkg = PkgDep.new
	    pkg.loaddef(f, "slz", "", $sysconfdir)
	    pkg.setup()
	    @pkgs[f] = pkg
	end
    end

    def resolvedeps
	resolved = Array.new
	loop = 0

	# first, purge unknown package names
	allnames = Array.new
	@deflist.each do |f|
	    p = @pkgs[f]
	    allnames.concat(p.names)
	end
	@deflist.each do |f|
	    p = @pkgs[f]
	    unknown = p.depends - allnames
	    p.remove_depends(unknown)
	end

	while (@deflist.length > 0) do
=begin
	    puts "loop:#{loop}, resolved=#{resolved.length}, remain=#{@deflist.length}"
	    puts
	    puts "resolved: "
	    resolved.each do |f|
		@pkgs[f].dump
	    end
	    puts
	    puts "remain:"
	    @deflist.each do |f|
		@pkgs[f].dump
	    end
	    puts
=end

	    # first, remove resolved package
	    @deflist.each do |f|
		p = @pkgs[f]

		if (p.depends.length == 0)
		    # ok, resolved...
		    resolved.push(f)
		    @deflist.delete(f)

		    p.setlevel(loop)
		end
	    end

	    # second, update dependency
	    @deflist.each do |f|
		p = @pkgs[f]

		resolved.each do |r|
		    p.remove_depends(@pkgs[r].names)
		end
	    end

	    loop = loop + 1
	end

	@deflist = resolved
    end

    def getdirlist
	dirlist = Array.new

	@deflist.each do |f|
	    dirlist.push(f.gsub(%r|^./(.*)/pkgdef$|, "\\1"))
	end
	return dirlist
    end

    def resolve
	loaddeffiles
	resolvedeps
    end

    # handles dirlist/skiplist
    def filter_dir(list, isSkip)
	return if (list == nil)
	newlist = Array.new

	@deflist.each do |f|
	    match = list.find{|i| f =~ /#{i}\/pkgdef/}

	    if ((!isSkip && match) || (isSkip && !match))
		newlist.push(f)
	    end
	end
	@deflist = newlist
    end

    def dump
	@deflist.each do |f|
	    p = @pkgs[f]
	    puts "#{p.level}:#{f}"
	end
    end

    def test
	resolve_list
	@deflist.each do |f|
	    puts "#{@pkgs[f].level}: #{f}"
	end
    end
end
