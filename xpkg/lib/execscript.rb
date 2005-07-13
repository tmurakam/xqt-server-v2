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
# Execute scripts
#

module ExecScript
    # execute command
    def ExecCmd(cmd, ignore_err = false)
	STDERR.puts(cmd)
	return if ($debug)

	if (!system(cmd))
	    msg = "command execution error: #{$?.to_i() / 256}"
	    
	    if (ignore_err)
		STDERR.puts "Warning: #{msg}"
	    else
		raise "Error: #{msg}"
	    end
	end
	return
    end

    # execute script
    def ExecScript(script, dir)
	if ($debug)
	    puts script
	    return
	end

	# change dir
	if (!FileTest.exist?(dir))
	    Dir.mkdir(dir)
	end

	pwd = Dir.pwd
	Dir.chdir(dir)

	# create script file and execute it
	open("tmp.sh", "w") do |fh|
	    fh.print script
	end
	File.chmod(0755, "tmp.sh")
	ExecCmd("/bin/sh -v tmp.sh")
	File.unlink("tmp.sh")

	Dir.chdir(pwd)
    end
end

