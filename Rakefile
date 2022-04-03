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
  Rake::Task["XKCP/README.markdown"].invoke
  puts "Extracting XKCP license from \"XKCP/README.markdown\" and saving it to \"LICENSE.XKCP\"."
  license = File.binread("XKCP/README.markdown")
                .scan(/# Under which license is the XKCP.*?(?=^#)/m).first
  raise "No license extracted" unless license
  File.binwrite("LICENSE.XKCP", license.strip + "\n")
end.instance_eval do
  def needed?
    !File.exist?("LICENSE.XKCP") || File.exist?("XKCP/README.markdown") &&
        File.mtime("LICENSE.XKCP") < File.mtime("XKCP/README.markdown")
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
