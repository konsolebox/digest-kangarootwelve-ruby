# build, clean, clobber, install, install:local, release[remote]
require 'bundler/gem_tasks'

# clean, clobber, compile, compile:digest/kangarootwelve
require 'rake/extensiontask'
Rake::ExtensionTask.new('digest/kangarootwelve', Bundler::GemHelper.gemspec)

# initialize_xkcp
desc "Initialize and update XKCP submodule"
task :initialize_xkcp do |t|
  puts "Initializing and updating XKCP submodule"
  system "git submodule init && git submodule update -f"
end

# import_xkcp_files
desc "Import needed XKCP files and prepare target directories"
task :import_xkcp_files => [:initialize_xkcp, :clean] do |t|
  Dir.chdir File.dirname(__FILE__) do
    Rake.ruby "./import-xkcp-files.rb"
  end
end

CLOBBER.include "ext/digest/kangarootwelve/XKCP"
CLOBBER.include "ext/digest/kangarootwelve/targets"

# import_xkcp_files_lazy
task :import_xkcp_files_lazy do
  Dir.chdir File.dirname(__FILE__) do
    if !File.exist?("./ext/digest/kangarootwelve/XKCP") || \
        !File.exist?("./ext/digest/kangarootwelve/targets")
      Rake::Task[:import_xkcp_files].invoke
    end
  end
end

Rake::Task[:build].prerequisites.unshift :import_xkcp_files_lazy
Rake::Task[:compile].prerequisites.unshift :import_xkcp_files_lazy

# compile_lazy
task :compile_lazy do
  compile_task = Rake::Task[:compile]
  compile_task.invoke unless compile_task.already_invoked || \
      File.exist?(File.expand_path("../lib/digest/xxhash.so", __FILE__))
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
