require 'mkmf'

TARGETS = ['armv6m', 'armv7a', 'armv7m', 'armv8a', 'asmx86-64', 'asmx86-64shld',
    'avr8', 'bulldozer', 'compact', 'generic32', 'generic32lc', 'generic64',
    'generic64lc', 'haswell', 'nehalem', 'reference', 'reference32bits',
    'sandybridge', 'skylakex']

target = with_config('target') || 'compact'
raise "Invalid target '#{target}'." unless TARGETS.include? target
common_dir = File.join(File.dirname(__FILE__), 'keccak', 'common')
target_dir = File.join(File.dirname(__FILE__), 'keccak', target)
find_header('align.h', common_dir)
find_header('KeccakP-1600-SnP.h', target_dir)
$defs.push('-Wall') if enable_config('all-warnings')
create_makefile('digest/kangarootwelve', target_dir)
File.write('Makefile', 'V = 1', mode: 'a') if enable_config('verbose-mode')
