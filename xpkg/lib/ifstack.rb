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
# ifstack : %if �� %elsif �� %else �� %endif �ν���
#
# parse �᥽�åɤ˹Ԥ򿩤碌��ȡ����� if ��˹��פ��Ƥ��뤫�ɤ������֤�
#

class IfStack
    # ���ơ���
    NotMatch = 0	# if ����פ��Ƥ��ʤ�
    Match = 1		# ���פ��Ƥ���
    Matched = 2		# �������פ����ʸ��ߤϹ��פ��Ƥ��ʤ�)

    def initialize(target)
	# @stack �� %if ���֤Υ����å�
	@stack = Array.new
	@target = target
    end

    # ���ץ����å�
    def match?
	@stack.each do |s|
	    return false if (s != Match)
	end
	return true
    end

    # �������ƥ�����Υ����å�
    def checktarget(cond)
	#
	# ruby �μ����Ѵ����� eval ��ɾ������
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

    # �ԤΥѡ�����Ƚ��
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
