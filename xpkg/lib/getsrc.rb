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
# ��������������ɽ����⥸�塼��
#

module GetSource
    # ��������������
    def GetSource(sites, src, distfiledir, dest, dlOnly)
	@sites = sites
	@distfiledir = distfiledir
	@dest = dest

	if (!FileTest.exist?(distfiledir))
	    system("mkdir -p #{distfiledir}")
	end

	cmd = nil

	# ������ tarball ��Ÿ��
	src.each do |file|
	    patchlevel = nil
	    if (file =~ /^(.*);patch=(.*)$/)
		file = $1
		patchlevel = $2
	    end

	    if (file =~ /^local:(.*)/) 
		file = $1
		fullpath = "../" + file   # ad hoc...

		if (!FileTest.exist?(file))
		    STDERR.puts "no file: #{file}"
		    exit 1
		end
	    else
		fullpath = Download(file)
	    end

	    if (!dlOnly)
		Extract(fullpath, patchlevel)
	    end
	end
    end

    # �ʲ��� private method
    private

    def Download(file)
	sitelist = nil
	if (file =~ /^(.*tp:\/\/.*)\/([^\/]+)$/)
	    file = $2

	    sitelist = Array.new
	    sitelist.push($1)
	else
	    sitelist = @sites.dup
	end
	if ($site_default != nil)
	    sitelist.unshift($site_default)
	end
	if ($site_fallback != nil)
	    sitelist.push($site_fallback)
	end

	fullpath = @distfiledir + "/" + file

	# �ե�������������ɤ���
	sitelist.each do |site|
	    break if (FileTest.exist?(fullpath))

	    if (site =~ /[^\/]$/)
		site = site + "/"
	    end
	    cmd = "wget -P #{@distfiledir} #{site}#{file}"
	    ExecCmd(cmd, true)
	end
	if (!FileTest.exist?(fullpath))
	    STDERR.puts "Can't download file: #{file}"
	    exit 1
	end

	return fullpath
    end

    def Extract(file, patchlevel)
	# ����Ÿ�����ޥ��
	cat = "cat"
	if (file =~ /gz$/)
	    cat = "zcat"
	elsif (file =~ /bz2?$/)
	    cat = "bzcat"
	end

	# �ѥå���Ŭ�Ѥޤ��ϥե�����Ÿ��
	cmd = nil
	if (patchlevel != nil)
	    cmd = "cd #{@dest}; #{cat} #{file} | patch -p#{patchlevel}"
	elsif (file =~ /\.tar\.[bg]z2?$/ || file =~ /\.tgz$/ || file =~ /\.tar$/)
	    cmd = "#{cat} #{file} | tar xvf -"
	else
	    cmd = "cp #{file} ."
	end

	if (cmd) 
	    ExecCmd(cmd)
	end
    end
end
