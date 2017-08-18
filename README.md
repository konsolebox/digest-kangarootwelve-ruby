# digest-kangarootwelve-ruby

The digest-kangarootwelve gem is an implementation of KangarooTwelve for Ruby
that works on top of Digest::Base.

It allows hashing with different digest lengths and different customization
strings.

See http://kangarootwelve.org/ to know more about the hashing algorithm.

The core implementation was extracted/generated from the KCP
(https://github.com/gvanas/KeccakCodePackage).  The extension currently utilizes
the compact version, but I'll try to use a more optimized version in the future,
and also consider adding a capability to use an external library that can be
natively optimized (e.g. libkeccak.a), without compromisations.

## Installation

Add this line to your application's Gemfile:

    gem 'digest-kangarootwelve'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install digest-kangarootwelve

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

You can also visit the automatically generated API documentation pages provided
by RubyDoc.info in http://www.rubydoc.info/gems/digest-kangarootwelve/.

## Contributing

1. Fork it ( https://github.com/konsolebox/digest-kangarootwelve-ruby/fork ).
2. Create your feature branch (`git checkout -b my-new-feature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin my-new-feature`).
5. Create a new Pull Request.
