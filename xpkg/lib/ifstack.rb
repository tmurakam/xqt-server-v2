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
# ifstack : handles %if - %elsif - %else - %endif
#
# Handle each lines with parse method, it return if the current line matches the condition.
#

class IfStack
    # state constant
    NotMatch = 0	# does not match the condition
    Match = 1		# match the condition
    Matched = 2		# matched the condition in the past (not match currently)

    def initialize(target)
	# @stack is a stack of %if states
	@stack = Array.new
	@target = target
    end

    # check if the line matches the condition
    def match?
	@stack.each do |s|
	    return false if (s != Match)
	end
	return true
    end

    # check target architecture
    def checktarget(cond)
	#
	# convert into ruby equation and evaluate with 'eval'.
	#
	a = cond.split(' ')
	b = Array.new
	a.each do |w|
	    if (w =~ /^[a-zA-Z]\w+/)
		if (w == @target)
		    w = "true"
		else
		    w = "false"
		end
	    end
	    b.push(w)
	end
	cond = b.join(' ')

	if (eval(cond))
	    return Match
	end
	return NotMatch
    end

    # Parse each line and match the condition
    def parse(line)

	if (line =~ /^(\S+)\s*(.*)$/)
	    case ($1)
	    when "%if"
		@stack.push( checktarget($2) )
		return false

	    when "%elsif", "%elif"
		s = @stack.pop
		if (s == NotMatch)
		    @stack.push( checktarget($2) )
		else
		    @stack.push( Matched )
		end
		return false

	    when "%else"
		s = @stack.pop
		if (s == NotMatch)
		    @stack.push(Match)
		else
		    @stack.push(Matched)
		end
		return false

	    when "%endif"
		@stack.pop
		return false
	    end
	end

	return match?
    end
end
