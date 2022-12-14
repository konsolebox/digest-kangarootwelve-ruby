# build, clean, clobber, release[remote]

require 'bundler/gem_tasks'

# clean

CLEAN.include "ext/digest/kangarootwelve/Makefile"
CLEAN.include "ext/digest/kangarootwelve/**/*.o"
CLEAN.include "ext/digest/kangarootwelve/**/*.so"

# clobber

if File.exist?(".git")
  CLOBBER.include "LICENSE.XKCP"
  CLOBBER.include "ext/digest/kangarootwelve/XKCP"
  CLOBBER.include "ext/digest/kangarootwelve/targets"
end

# initialize_xkcp

desc "Initialize and update XKCP submodule"
task :initialize_xkcp => ".git" do |t|
  puts "Initializing and updating XKCP submodule"
  system "git submodule init && git submodule update -f"
end

file "XKCP/Makefile.build" => :initialize_xkcp
file "XKCP/README.markdown" => :initialize_xkcp

# import_xkcp_license

task :import_xkcp_license do
  Rake::Task["XKCP/LICENSE"].invoke
  puts "Importing XKCP/LICENSE as LICENSE.XKCP."
  File.binwrite("LICENSE.XKCP", File.binread("XKCP/LICENSE"))
end.instance_eval do
  def needed?
    !File.exist?("LICENSE.XKCP") || File.exist?("XKCP/LICENSE") &&
        File.mtime("LICENSE.XKCP") < File.mtime("XKCP/LICENSE")
  end
end

# import_xkcp_files_lazy

task :import_xkcp_files_lazy do
  Rake::Task[:import_xkcp_files].invoke
end.instance_eval do
  def needed?
    %w[XKCP targets].any?{ |dir| !File.exist?("ext/digest/kangarootwelve/#{dir}") }
  end
end

# build

Rake::Task[:build].prerequisites.unshift :import_xkcp_files
Rake::Task[:build].prerequisites.unshift :import_xkcp_license
Rake::Task[:build].prerequisites.unshift :clobber

# compile, compile:digest/kangarootwelve

require 'rake/extensiontask'
Rake::ExtensionTask.new('digest/kangarootwelve', Bundler::GemHelper.gemspec)
Rake::Task[:compile].prerequisites.unshift :import_xkcp_files_lazy

# compile_lazy

task :compile_lazy do
  Rake::Task[:compile].invoke
end.instance_eval do
  def needed?
    !File.exist?("lib/digest/kangarootwelve.so")
  end
end

# test

require 'rake/testtask'

Rake::TestTask.new(:test => :compile_lazy) do |t|
  t.test_files = FileList['test/test.rb']
  t.verbose = true
end

# default

task :default => [:compile, :test]

# Run `rake --tasks` or `rake --tasks --all` for a list of tasks.
