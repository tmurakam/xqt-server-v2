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
# ifstack : %if 〜 %elsif 〜 %else 〜 %endif の処理
#
# parse メソッドに行を食わせると、現在 if 節に合致しているかどうかを返す
#

class IfStack
    # ステート
    NotMatch = 0	# if 節合致していない
    Match = 1		# 合致している
    Matched = 2		# 以前合致した（現在は合致していない)

    def initialize(target)
	# @stack は %if 状態のスタック
	@stack = Array.new
	@target = target
    end

    # 合致チェック
    def match?
	@stack.each do |s|
	    return false if (s != Match)
	end
	return true
    end

    # アーキテクチャのチェック
    def checktarget(cond)
	#
	# ruby の式に変換して eval で評価する
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

    # 行のパースと判定
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
