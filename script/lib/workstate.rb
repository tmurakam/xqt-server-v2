#
# Work State епеще╣
#

class WorkState
    StateFile = ".mkpkg.state"

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

    
