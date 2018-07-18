require 'mkmf'

TARGETS = ['ARMv6M', 'ARMv7A', 'ARMv7M', 'ARMv8A', 'asmX86-64', 'asmX86-64shld',
    'AVR8', 'Bulldozer', 'compact', 'generic32', 'generic32lc', 'generic64',
    'generic64lc', 'Haswell', 'Nehalem', 'reference', 'reference32bits',
    'SandyBridge', 'SkylakeX']

target = with_config('target') || 'compact'
raise "Invalid target '#{target}'." unless TARGETS.include? target
common_dir = File.absolute_path(File.join(__FILE__, '..', 'keccak', 'common'))
target_dir = File.absolute_path(File.join(__FILE__, '..', 'keccak', target))
find_header('align.h', common_dir)
find_header('KeccakP-1600-SnP.h', target_dir)
create_makefile('digest/kangarootwelve', target_dir)
