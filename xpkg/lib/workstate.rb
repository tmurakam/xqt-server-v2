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
# Work State class
#

class WorkState
    StateFile = ".xpkg.state"

    def initialize
	@state = Array.new
	begin
	    open(StateFile, "r") do |fh|
		fh.each do |line|
		    line.chop!
		    @state.push(line)
		end
	    end
	rescue
	    # do nothing
	end
    end

    def clear
	@state.clear
	if (FileTest.exist?(StateFile))
	    File.unlink(StateFile)
	end
    end

    def done?(cmd)
	return @state.include?(cmd)
    end

    def setdone(cmd)
	if (!@state.include?(cmd))
	    @state.push(cmd)
	end
    end

    def save
	open(StateFile, "w") do |fh|
	    @state.each do |s|
		fh.puts s
	    end
	end
    end
end

    
