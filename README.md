# digest-kangarootwelve-ruby

The digest-kangarootwelve gem is an implementation of KangarooTwelve for Ruby
that works on top of `Digest::Base`.

It allows hashing with different digest lengths and different customization
strings.

See http://kangarootwelve.org/ to know more about the hashing algorithm.

The core implementation was extracted/generated from the KCP
(https://github.com/gvanas/KeccakCodePackage).

KCP provides implementations on many target platforms.  This gem used to only
have the `compact` implementation, but it now has adopted all of them.

The available implementation targets are `armv6m`, `armv7a`, `armv7m`, `armv8a`,
`asmx86-64`, `asmx86-64shld`, `avr8`, `bulldozer`, `compact`, `generic32`,
`generic32lc`, `generic64`, `generic64lc`, `haswell`, `nehalem`, `reference`,
`reference32bits`, `sandybridge`, and `skylakex`, with `compact`  being the
default.

Instructions on how to select a target is written below.

## Installation

Add this line to the application's Gemfile:

    gem 'digest-kangarootwelve'

And then execute:

    $ bundle

It can also be installed manually with:

    $ gem install digest-kangarootwelve

To install with a different platform target, use `--with-target`.  Example:

    $ gem install digest-kangarootwelve -- --with-target=sandybridge

It's a good idea to test the gem when using a different target platform other
than default as some platforms are not yet tested, and behavior of optimized
code can vary on different machines or compilers.  This can be done by running
the following command.  Replace VERSION with the installed gem's version.

    $ ( cd "$(ruby -e 'puts Gem.dir')"/gems/digest-kangarootwelve-VERSION && bundle && rake test )

The library can also be installed in Gentoo system-wide using 'layman':

    # Fetch remote list of overlays, and add 'konsolebox' overlay
    layman -f && layman -a konsolebox

    # Unmask unstable keyword
    echo 'dev-ruby/digest-kangarootwelve' > /etc/portage/package.keywords/dev-ruby.digest-kangarootwelve

    # Merge package
    emerge dev-ruby/digest-kangarootwelve

## Example Usage

    Digest::KangarooTwelve[32].digest("abc")
    => "\xAB\x17O2\x8CU\xA5Q\v\v \x97\x91\xBF\x8B`\xE8\x01\xA7\xCF\xC2\xAAB\x04-\xCB\x8FT\x7F\xBE:}"

    Digest::KangarooTwelve[32].hexdigest("abc")
    => "ab174f328c55a5510b0b209791bf8b60e801a7cfc2aa42042dcb8f547fbe3a7d"

    Digest::KangarooTwelve[32].new.update("a").update("b").update("c").hexdigest
    => "ab174f328c55a5510b0b209791bf8b60e801a7cfc2aa42042dcb8f547fbe3a7d"

    Digest::KangarooTwelve.implement(name: "SecretHash", digest_length: 32, customization: "secret")
    => Digest::SecretHash

    Digest::KangarooTwelve.implement(n: "SecretHash", d: 32, c: "secret")
    => Digest::SecretHash

    Digest::SecretHash.hexdigest("abc")
    => "dc1fd53f85402e2b34fa92bd87593dd9c3fe6cc49d9db6c05dc0cf26c6a7e03f"

    Digest::KangarooTwelve.default
    => Digest::KangarooTwelve_64

## Details

The implementation classes produced by `[]`, `default` or
`implement` can be used just like any other implementation class in `Digest`
(like `Digest::SHA1` or `Digest::SHA512`), since the implementation classes are
based on `Digest::Base`.

For details on how to use these methods, please examine the comments in
`ext/digest/kangarootwelve/ext.c`, or run `ri` with
`ri 'Digest::KangarooTwelve'`, or `ri 'Digest::KangarooTwelve::<method_name>'`.

## API Documentation

RubyGems.org also provides autogenerated API documentation of the library in
https://www.rubydoc.info/gems/digest-kangarootwelve/.

## Github Page

https://github.com/konsolebox/digest-kangarootwelve-ruby

## RubyGems.org Page

https://rubygems.org/gems/digest-kangarootwelve

## Contributing

1. Fork it ( https://github.com/konsolebox/digest-kangarootwelve-ruby/fork ).
2. Create feature branch (`git checkout -b my-new-feature`).
3. Commit changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin my-new-feature`).
5. Create a new Pull Request.

[![Build Status](https://travis-ci.org/konsolebox/digest-kangarootwelve-ruby.svg?branch=master)](https://travis-ci.org/konsolebox/digest-kangarootwelve-ruby)
[![Build status](https://ci.appveyor.com/api/projects/status/bwedifhi4wa5wik7?svg=true)](https://ci.appveyor.com/project/konsolebox/digest-kangarootwelve-ruby)
