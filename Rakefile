require 'bundler/gem_tasks'
require 'rake/testtask'

# clean, clobber, compile, and compile:digest/kangarootwelve
require 'rake/extensiontask'
Rake::ExtensionTask.new('digest/kangarootwelve', Bundler::GemHelper.gemspec)

# test
Rake::TestTask.new do |t|
  t.test_files = FileList['test/test.rb']
  t.verbose = true
end

# clean
task :clean do
  list = FileList.new('test/*.tmp', 'test/*.temp')
  rm_f list unless list.empty?
end

# Run `rake --tasks` for a list of tasks.
