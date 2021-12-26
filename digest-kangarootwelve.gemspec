# coding: utf-8

lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'digest/kangarootwelve/version'
require 'find'

Gem::Specification.new do |spec|
  spec.name          = 'digest-kangarootwelve'
  spec.version       = Digest::KangarooTwelve::VERSION
  spec.authors       = ['konsolebox']
  spec.email         = ['konsolebox@gmail.com']
  spec.summary       = 'KangarooTwelve for Ruby'
  spec.description   = 'A KangarooTwelve library that works on top of Digest::Base.'
  spec.homepage      = 'https://github.com/konsolebox/digest-kangarootwelve-ruby'
  spec.license       = 'MIT'

  spec.required_ruby_version = '>= 2.2'

  spec.files = %w{
    Gemfile
    LICENSE
    README.md
    Rakefile
    digest-kangarootwelve.gemspec
    lib/digest/kangarootwelve/version.rb
    test/test.rb
  } + Find.find('ext').to_a

  spec.executables   = []
  spec.test_files    = ['test/test.rb']
  spec.require_paths = ['lib']

  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rake-compiler', '~> 1.0'
  spec.add_development_dependency 'minitest', '~> 5.8'
  spec.add_development_dependency 'nori'
  spec.add_development_dependency 'nokogiri'

  spec.extensions = %w[ext/digest/kangarootwelve/extconf.rb]
end
