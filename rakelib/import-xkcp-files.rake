require 'singleton'
require 'fileutils'
autoload :Nokogiri, 'nokogiri'
autoload :Nori, 'nori'

# Provides the 'import_xkcp_files' task
#
class ImportXKCPFilesTask < Rake::TaskLib
  include Singleton

  EXT_DIR = File.expand_path("../../ext/digest/kangarootwelve", __FILE__)
  IMPL_FILES = %w[ext.c utils.h]
  PERSISTENT_TARGETS = %w[KangarooTwelve]
  REL_PATH_FROM_TARGETS_TO_EXT_DIR = "../.."
  REL_PATH_FROM_TARGETS_TO_XKCP_COPY_DIR = "../../XKCP"
  TARGETS_DIR = File.join(EXT_DIR, "targets")
  TARGETS_LIST_FILE = File.join(EXT_DIR, "targets/list")
  TARGET_EXCLUSION_PATTERN = /^(Xoo|Ket)/
  XKCP_COPY_DIR = File.join(EXT_DIR, "XKCP")
  XKCP_DIR = File.expand_path("../../XKCP", __FILE__)

  def initialize
    define
  end

  def define
    desc "Import needed XKCP files and prepare target directories"
    task :import_xkcp_files => ["XKCP/Makefile.build", :clean] do
      execute_task
    end
  end

private

  def get_needed_fragments(target_names, mapped_fragments, results_hash)
    target_names
      .reject{ |name| TARGET_EXCLUSION_PATTERN =~ name || results_hash.has_key?(name) }
      .each{ |name|
        fragment = mapped_fragments[name] or raise "Fragment of '#{name}' not found."
        results_hash[name] = fragment
        next_target_names = (fragment['@inherits'] || "").split(' ')
        get_needed_fragments(next_target_names, mapped_fragments, results_hash)
      }

    results_hash
  end

  def get_fragment_files(fragments)
    fragments.inject({}) do |hash, fragment|
      ['c', 'h', 'inc', 's', 'S']
        .map{ |e| fragment[e] }
        .reject(&:nil?)
        .map{ |e| e.split(' ') unless e.is_a? Array; e }
        .flatten
        .each{ |file|
          raise "Not a file path: #{file.inspect}" unless file.is_a? String
          hash[file] = file
        }

      hash
    end.values
  end

  def execute_task
    build_data = Dir.chdir(XKCP_DIR) do
      xml = Nokogiri.XML(IO.read("Makefile.build"), &:xinclude).to_xml
      Nori.new.parse(xml)['build']
    end

    targets = build_data['group'].find{ |e| (e['@all'] rescue nil) == "XKCP" }['product']['factor']
      .find{ |e| e['@set'] =~ /reference/ }['@set'].split(' ')

    raise "Targets have similar names." unless targets.size == targets.map(&:downcase).uniq.size

    fragments = build_data['group']
      .select{ |e| e['@xml:base'] =~ /^lib\// }
      .flat_map{ |e| e['fragment'] }
      .concat(build_data['fragment'])
      .reject(&:nil?)

    mapped_fragments = fragments.inject({}) do |hash, fragment|
      name = fragment['@name'] or raise "Fragment with no name encountered."
      raise "Duplicate fragment with name '#{name}' found." if hash.has_key? name
      hash[name] = fragment
      hash
    end

    all_needed_fragments = get_needed_fragments(targets + PERSISTENT_TARGETS, mapped_fragments, {})
    all_needed_files = get_fragment_files(all_needed_fragments.values)

    FileUtils.remove_dir TARGETS_DIR if File.exist? TARGETS_DIR
    FileUtils.remove_dir XKCP_COPY_DIR if File.exist? XKCP_COPY_DIR
    FileUtils.mkdir XKCP_COPY_DIR
    FileUtils.mkdir TARGETS_DIR

    all_needed_files.each do |file|
      src = File.join(XKCP_DIR, file)
      dest = File.join(XKCP_COPY_DIR, file)
      puts "Copying '#{src}' to '#{dest}'."
      FileUtils.mkdir_p File.dirname(dest)
      FileUtils.install src, dest
    end

    all_config = {}

    targets.each do |target_name|
      needed_fragments = get_needed_fragments([target_name] + PERSISTENT_TARGETS, mapped_fragments, {})
      needed_files = get_fragment_files(needed_fragments.values)
      expanded_impl_files = IMPL_FILES.map{ |file| File.join(EXT_DIR, file) }
      delegate_file_reg = {}
      target_dir = File.join(TARGETS_DIR, target_name.downcase)
      FileUtils.mkdir target_dir

      [
        needed_files.map{ |file| REL_PATH_FROM_TARGETS_TO_XKCP_COPY_DIR + "/" + file },
        IMPL_FILES.map{ |file| REL_PATH_FROM_TARGETS_TO_EXT_DIR + "/" + file }
      ].flatten.each do |relative_path|
        basename = relative_path[/[^\/]+$/]
        delegate_file = File.join(target_dir, basename)
        raise "Delegate file conflict: #{delegate_file}" if delegate_file_reg.has_key? delegate_file
        delegate_file_reg[delegate_file] = true
        puts "Creating #{delegate_file}."
        File.binwrite(delegate_file, "#include \"#{relative_path}\"\n")
      end

      all_config[target_name] = needed_fragments.values.map do |fragment|
        config = fragment['config'] || []
        config.is_a?(Array) ? config : config.split(' ')
      end.flatten.uniq

      defines = needed_fragments.values.map do |fragment|
        define = fragment['define'] || []
        define.is_a?(Array) ? define : define.split(' ')
      end.flatten.uniq

      unless defines.empty?
        defines_file = File.join(TARGETS_DIR, "defs")
        puts "Creating #{defines_file}."

        File.open(defines_file, 'w') do |io|
          io.puts defines.map{ |e| "#define #{e}" }
        end
      end
    end

    common_config = all_config.values.flatten.inject(Hash.new(0)){ |h, v| h[v] += 1; h }
      .select{ |k, v| v == all_config.length }.keys

    unless common_config.empty?
      config_h_file = File.join(TARGETS_DIR, "config.h")
      puts "Creating #{config_h_file}."

      File.open(config_h_file, 'w') do |io|
        io.puts common_config.map{ |e| "#define #{e}" }
      end
    end

    targets.each do |target_name|
      config = all_config[target_name].reject{ |e| common_config.include? e }

      unless common_config.empty? && config.empty?
        config_h_file = File.join(TARGETS_DIR, target_name.downcase, "config.h")
        puts "Creating #{config_h_file}."

        File.open(config_h_file, 'w') do |io|
          io.puts "#include \"../config.h\"" unless common_config.empty?
          io.puts config.map{ |e| "#define #{e}" } unless config.empty?
        end
      end
    end

    File.open(TARGETS_LIST_FILE, 'w'){ |io| io.puts targets.map(&:downcase) }
  end
end

ImportXKCPFilesTask.instance
