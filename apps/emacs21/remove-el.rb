#!/usr/bin/ruby

# .elc が存在する場合、対応する .el を削除する

`find #{ARGV[0]} -name "*.el" -print`.each do |file|
    file.chop!
    compiled = file.sub(/\.el$/, ".elc")
    if (FileTest.exists?(compiled))
	puts "Remove #{file}"
	File.unlink(file)
    end
end	
