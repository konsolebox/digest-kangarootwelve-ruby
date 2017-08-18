# digest-kangarootwelve-ruby

An implementation of KangarooTwelve for Ruby that works on top of Digest::Base.

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

    require 'digest/kangarootwelve'
    => true

    klass = Digest::KangarooTwelve[32]
    => Digest::KangarooTwelve_32

    hash = klass.new
    => #<Digest::KangarooTwelve_32:32||1ac2d450fc3b4205d19da7bfca1b37513c0803577ac7167f06fe2ce1f0ef39e5>

    hash.hexdigest("abc")
    => "ab174f328c55a5510b0b209791bf8b60e801a7cfc2aa42042dcb8f547fbe3a7d"

    Digest::KangarooTwelve[32].new.hexdigest("abc")
    => "ab174f328c55a5510b0b209791bf8b60e801a7cfc2aa42042dcb8f547fbe3a7d"

    Digest::KangarooTwelve.default
    => Digest::KangarooTwelve_64

    Digest::KangarooTwelve.implement(digest_length: 32, customization: "secret")
    => Digest::KangarooTwelve_32_736563726574

    Digest::KangarooTwelve.implement(name: "SecretHash", digest_length: 32, customization: "secret")
    => Digest::SecretHash

    Digest::KangarooTwelve.implement(n: "SecretHash", d: 32, c: "secret")
    => Digest::SecretHash

    Digest::SecretHash.new.hexdigest("abc")
    => "dc1fd53f85402e2b34fa92bd87593dd9c3fe6cc49d9db6c05dc0cf26c6a7e03f"

    Digest::KangarooTwelve.implement(name: nil, digest_length: 48)
    => #<Class:0x00000001167208>

## Details

For details on how to use the methods, please examine the comments in
`ext/digest/kangarootwelve/ext.c`, or try to run
`ri 'Digest::KangarooTwelve'` or
`ri 'Digest::KangarooTwelve::<method_name>'`.  I'll try to provide a more
readable format of the documentation for the API soon.

You can use the implementation classes produced by `[]`, `default` or
`implement`, just like any other implementation class in `Digest`
(e.g. `Digest::SHA1`, `Digest::SHA512`), since they are also derived from
`Digest::Base`, and are programmed to work the way an implementation class
that's based on the `Digest` framework should.

## Contributing

1. Fork it ( https://github.com/konsolebox/digest-kangarootwelve-ruby/fork ).
2. Create your feature branch (`git checkout -b my-new-feature`).
3. Commit your changes (`git commit -am 'Add some feature'`).
4. Push to the branch (`git push origin my-new-feature`).
5. Create a new Pull Request.
