#!/usr/bin/ruby

# .elc ��¸�ߤ����硢�б����� .el ��������

filelist = `find #{ARGV[0]} -name "*.el" -print`
filelist.split(/\n/).each do |file|
	compiled = file.sub(/\.el$/, ".elc")
	if (FileTest.exists?(compiled))
		File.unlink(file)
	end
end	
