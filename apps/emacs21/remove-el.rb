#!/usr/bin/ruby

# .elc が存在する場合、対応する .el を削除する

filelist = `find #{ARGV[0]} -name "*.el" -print`
filelist.split(/\n/).each do |file|
	compiled = file.sub(/\.el$/, ".elc")
	if (FileTest.exists?(compiled))
		File.unlink(file)
	end
end	
