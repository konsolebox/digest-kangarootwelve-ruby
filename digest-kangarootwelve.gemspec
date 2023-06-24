# coding: utf-8

lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'digest/kangarootwelve/version'
require 'find'

Gem::Specification.new do |spec|
  spec.name          = "digest-kangarootwelve"
  spec.version       = Digest::KangarooTwelve::VERSION
  spec.authors       = ["konsolebox"]
  spec.email         = ["konsolebox@gmail.com"]
  spec.summary       = "KangarooTwelve for Ruby"
  spec.description   = "A KangarooTwelve library that utilizes the XKCP package and can be " \
                       "optionally optimized during compile-time based on a target. It produces " \
                       "implementation classes that work on top of Digest::Base and conform " \
                       "with how Digest classes work."
  spec.homepage      = "https://github.com/konsolebox/digest-kangarootwelve-ruby"
  spec.license       = "MIT"

  spec.required_ruby_version = ">= 2.2"

  spec.files = %w[
    Gemfile
    LICENSE
    README.md
    Rakefile
    digest-kangarootwelve.gemspec
    lib/digest/kangarootwelve/version.rb
    rakelib/alt-install-task.rake
    test/test.rb
  ]
  spec.files += Find.find("ext").to_a
  spec.files += ["LICENSE.XKCP"] if File.exist? "LICENSE.XKCP"

  spec.executables   = []
  spec.test_files    = ["test/test.rb"]
  spec.require_paths = ["lib"]

  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler", "~> 1.2", ">= 1.2.3"
  spec.add_development_dependency "minitest", "~> 5.8"
  spec.add_development_dependency "nori"
  spec.add_development_dependency "nokogiri"

  spec.extensions = %w[ext/digest/kangarootwelve/extconf.rb]
end
