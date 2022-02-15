require 'mkmf'

targets_list_file = File.expand_path('../targets/list', __FILE__)
targets = File.readlines(targets_list_file).map(&:chomp)
target = (with_config('target') || 'compact').downcase
raise "Invalid target '#{target}'." unless targets.include? target
target_dir = File.join(File.dirname(__FILE__), 'targets', target)
target_defs_file = File.join(target_dir, target, "defs")
$defs.concat File.readlines(target_defs_file).map{ |e| "-D#{e.chomp}" } if File.exist? target_defs_file
$defs.push('-Wall') if enable_config('all-warnings')
$srcs = Dir.glob(File.join(target_dir, "*.{c,s,S}" ))
create_makefile('digest/kangarootwelve', target_dir)
File.write('Makefile', 'V = 1', mode: 'a') if enable_config('verbose-mode')
