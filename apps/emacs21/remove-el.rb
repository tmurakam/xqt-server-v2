#!/usr/bin/ruby

# .elc ��¸�ߤ����硢�б����� .el ��������

`find #{ARGV[0]} -name "*.el" -print`.each do |file|
    file.chop!
    compiled = file.sub(/\.el$/, ".elc")
    if (FileTest.exists?(compiled))
	puts "Remove #{file}"
	File.unlink(file)
    end
end	
