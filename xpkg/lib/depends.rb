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

    attr_reader :deffile, :dir, :names, :depends, :level

    def initialize
	super
	@level = 0
	@names = Array.new
	@depends = Array.new
    end

    def load(f)
	@deffile = f
	@dir = f.gsub(%r|^./(.*)/pkgdef.*|, "\\1")

	loaddef(@deffile, "slz", "", $sysconfdir)

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
	@pkgs = Array.new
    end

    def loaddeffiles
	deflist = `find . -name "pkgdef" -print`.split

	`find . -name "pkgdef.list" -print`.split.each do |list|
	    dir = list.gsub(%r|pkgdef.list$|, "")

	    IO.readlines(list).each do |line|
		line.chop!
		line.strip!
		next if (file == "" || file =~ /^#/)

		deflist.push(dir + file)
	    end
	end
	
	deflist.each do |f|
	    puts "Loading : #{f}" if ($verbose)
	    pkg = PkgDep.new
	    pkg.load(f)
	    pkg.loaddef(f, "slz", "", $sysconfdir)
	    @pkgs.push(pkg)
	end
    end

    def resolvedeps
	# first, purge unknown package names
	allnames = Array.new
	@pkgs.each do |p|
	    allnames.concat(p.names)
	end
	@pkgs.each do |p|
	    unknown = p.depends - allnames
	    p.remove_depends(unknown)
	end

	resolved = Array.new
	loop = 0

	while (@pkgs.length > 0) do
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
	    @pkgs.each do |p|
		if (p.depends.length == 0)
		    # ok, resolved...
		    resolved.push(p)
		    @pkgs.delete(p)

		    p.setlevel(loop)
		end
	    end

	    # second, update dependency
	    @pkgs.each do |p|
		resolved.each do |r|
		    p.remove_depends(r.names)
		end
	    end

	    loop = loop + 1
	end

	@pkgs = resolved
    end

    def getdirlist
	dirlist = Array.new
	@pkgs.each do |p|
	    dirlist.push(p.dir)
	end
	return dirlist.uniq
    end

    def resolve
	loaddeffiles
	resolvedeps
    end

    # handles dirlist/skiplist
    def filter_dir(list, isSkip)
	return if (list == nil)
	newlist = Array.new

	@pkgs.each do |p|
	    match = list.find{|i| p.deffile.include?(i)}

	    if ((!isSkip && match) || (isSkip && !match))
		newlist.push(p)
	    end
	end
	@pkgs = newlist
    end

    def dump
	@pkgs.each do |p|
	    puts "#{p.level}:#{p.dir}"
	end
    end

    def test
	resolve_list
	@pkgs.each do |p|
	    puts "#{p.level}: #{p.dir}"
	end
    end
end
