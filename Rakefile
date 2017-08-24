# build, clean, clobber, install, install:local, release[remote]
require 'bundler/gem_tasks'

# clean, clobber, compile, compile:digest/kangarootwelve
require 'rake/extensiontask'
Rake::ExtensionTask.new('digest/kangarootwelve', Bundler::GemHelper.gemspec)

# test
require 'rake/testtask'
Rake::TestTask.new do |t|
  t.test_files = FileList['test/test.rb']
  t.verbose = true
end

# Run `rake --tasks` or `rake --tasks --all` for a list of tasks.
