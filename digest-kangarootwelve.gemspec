# coding: utf-8

lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'digest/kangarootwelve/version'

Gem::Specification.new do |spec|
  spec.name          = "digest-kangarootwelve"
  spec.version       = Digest::KangarooTwelve::VERSION
  spec.authors       = ["konsolebox"]
  spec.email         = ["konsolebox@gmail.com"]
  spec.summary       = "KangarooTwelve for Ruby"
  spec.description   = "A KangarooTwelve library that works on top of Digest::Base."
  spec.homepage      = "https://github.com/konsolebox/digest-kangarootwelve-ruby"
  spec.license       = "MIT"

  spec.required_ruby_version = '>= 2.2'

  spec.files = %w{Gemfile
      LICENSE
      README.md
      Rakefile
      digest-kangarootwelve.gemspec
      ext/digest/kangarootwelve/KangarooTwelve.c
      ext/digest/kangarootwelve/KangarooTwelve.h
      ext/digest/kangarootwelve/KeccakP-1600-SnP.h
      ext/digest/kangarootwelve/KeccakP-1600-compact64.c
      ext/digest/kangarootwelve/KeccakP-1600-times2-SnP.h
      ext/digest/kangarootwelve/KeccakP-1600-times2-on1.c
      ext/digest/kangarootwelve/KeccakP-1600-times4-SnP.h
      ext/digest/kangarootwelve/KeccakP-1600-times4-on1.c
      ext/digest/kangarootwelve/KeccakP-1600-times8-SnP.h
      ext/digest/kangarootwelve/KeccakP-1600-times8-on1.c
      ext/digest/kangarootwelve/KeccakSponge-common.h
      ext/digest/kangarootwelve/KeccakSponge.inc
      ext/digest/kangarootwelve/KeccakSpongeWidth1600.c
      ext/digest/kangarootwelve/KeccakSpongeWidth1600.h
      ext/digest/kangarootwelve/Phases.h
      ext/digest/kangarootwelve/PlSnP-Fallback.inc
      ext/digest/kangarootwelve/SnP-Relaned.h
      ext/digest/kangarootwelve/align.h
      ext/digest/kangarootwelve/brg_endian.h
      ext/digest/kangarootwelve/ext.c
      ext/digest/kangarootwelve/extconf.rb
      ext/digest/kangarootwelve/utils.h
      lib/digest/kangarootwelve/version.rb
      test/test.rb}

  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]

  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler", "~> 1.0"
  spec.add_development_dependency "minitest", "~> 5.8"

  spec.extensions = %w[ext/digest/kangarootwelve/extconf.rb]
end
