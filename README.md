# digest-kangarootwelve

This gem is an implementation of KangarooTwelve for Ruby that works on top of
the `Digest::Base` class.

It can be configured to produce outputs with different digest lengths, and can
be initilized with different customization strings.

To know more about the KangarooTwelve algorithm, visit
https://keccak.team/kangarootwelve.html.

The core implementation of KangarooTwelve is part of the
[XKCP](https://github.com/XKCP/XKCP) project.

XKCP provides multiple targets for different architecture-dependent builds.

This gem can be configured to compile differently based on the target specified.

The current target implementations are `armv6`, `armv6m`, `armv7a`, `armv7m`,
`armv8a`, `avr8`, `avx`, `avx2`, `avx2noasm`, `avx512`, `avx512noasm`,
`compact`, `generic32`, `generic32lc`, `generic64`, `generic64lc`,  `ssse3`, and
`xop`, with `compact` being the default target.

Details on what architectures these targets support are provided in the
[README.markdown](https://github.com/XKCP/XKCP/blob/master/README.markdown) file
of XKCP.

## Installation

To use the gem in an application, add the following line to the application's
Gemfile:

    gem 'digest-kangarootwelve'

And then execute:

    bundle

The gem can also be installed manually using the `gem` command:

    gem install digest-kangarootwelve

To build and install the gem against a different platform target, use a
`--with-target` option that follows a `--`.  Example:

    gem install digest-kangarootwelve -- --with-target=avx

Targets may also need `CFLAGS` specified.  Please see the
[Build failures](#build-failures) section.

It's also a good idea to test the gem before actively using it, especially when
using a not so commonly used target.  Here's one way to test it:

    cd GEM_DIR/gems/digest-kangarootwelve-VERSION
    bundle
    rake test

To know the right value of GEM_DIR, try running
`gem info digest-kangarootwelve`.

## Installing in Gentoo

The library can also be globally installed in Gentoo using `layman` or
`eselect-repository`:

    # Fetch remote list of overlays, and add 'konsolebox' overlay.
    layman -f && layman -a konsolebox

    # Or enable repo through eselect-repository.
    eselect repository enable konsolebox
    emaint sync --repo konsolebox

    # Unmask unstable keyword.
    echo 'dev-ruby/digest-kangarootwelve' > /etc/portage/package.accept_keywords/dev-ruby.digest-kangarootwelve

    # Optionally specify a different build target that works better in the
    # machine.  This may need additional CFLAGS options like -march=native set
    # in the environment.  The default build target `compact` should also be
    # disabled.
    echo 'dev-ruby/digest-kangarootwelve target_avx512 -target_compact' > /etc/portage/package.use/dev-ruby.digest-kangarootwelve

    # To enable testing, FEATURES=test should be set in the environment, and
    # test use flag should also be enabled.
    printf '%s\n' 'FEATURES="${FEATURES-} test"' 'USE="${USE-} test"' > /etc/portage/env/test
    echo 'dev-ruby/digest-kangarootwelve test' > /etc/portage/package.env/dev-ruby.digest-kangarootwelve

    # Merge package.
    emerge dev-ruby/digest-kangarootwelve

## Testing from source

The gem can also be tested from source using the following commands:

    # Clone this repo.
    git clone https://github.com/konsolebox/digest-kangarootwelve-ruby.git -b master

    # Change to directory.
    cd digest-kangarootwelve-ruby

    # Optionally checkout a tagged version.
    git checkout v0.4.8

    # Run bundle
    bundle

    # Finally run `rake` to compile the extension and test the gem.
    rake

    # A different target can also be specified.  Example:
    rake clean clobber && rake -- --with-target=avx

## Build failures

Targets like AVX2 may fail to build unless an explicit `CFLAGS` with proper
architecture-related options is specified.

Specifying a `CFLAGS` can be done by using the `--with-cflags` option.  For
example:

    rake -- --with-target=avx2 --with-cflags="-march=native"

    gem install digest-kangarootwelve -- --with-target=avx2 --with-cflags="-march=native"

    bundle config build.digest-kangarootwelve --with-target=avx2 --with-cflags="-march=native"

Compact (the default target) and AVX targets have been tested to not need a
`CFLAGS` to compile and work.

Please note that this gem has not been tested to work with cross-compilations,
so please test the resulting runtime thoroughly.

## Example Usage

    require 'digest/kangarootwelve'

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

[![Build Status](https://github.com/konsolebox/digest-kangarootwelve-ruby/actions/workflows/ruby.yml/badge.svg)](https://github.com/konsolebox/digest-kangarootwelve-ruby/actions/workflows/ruby.yml)
[![Build Status](https://ci.appveyor.com/api/projects/status/bwedifhi4wa5wik7?svg=true)](https://ci.appveyor.com/project/konsolebox/digest-kangarootwelve-ruby)
