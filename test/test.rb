require 'minitest/autorun'
require File.expand_path('../../lib/digest/kangarootwelve', __FILE__)

def get_repeated_0x00_to_0xfa(length)
  hex = (0..0xfa).to_a.map{ |e| sprintf "%2x", e }.join
  str = [hex].pack('H*')
  cycles = (Float(length) / str.size).ceil
  [str].cycle(cycles).to_a.join[0...length]
end

def get_repeated_0xff(length)
  str = ["ffffffffffffffffffff"].pack('H*')
  cycles = (Float(length) / str.size).ceil
  [str].cycle(cycles).to_a.join[0...length]
end

def hex_encode(str)
  str.unpack('H*').pop
end

describe Digest::KangarooTwelve do
  it "produces implementation classes" do
    _(Digest::KangarooTwelve[32].superclass).must_equal Digest::KangarooTwelve::Impl
    _(Digest::KangarooTwelve[32].digest_length).must_equal 32
    _(Digest::KangarooTwelve[32]).must_equal Digest::KangarooTwelve_32
    _(Digest::KangarooTwelve.implement(digest_length: 64).superclass).must_equal Digest::KangarooTwelve::Impl
    _(Digest::KangarooTwelve.implement(digest_length: 64).digest_length).must_equal 64
    _(Digest::KangarooTwelve.implement(digest_length: 64)).must_equal Digest::KangarooTwelve_64
  end

  it "produces a default implemention with a digest length of 64" do
    _(Digest::KangarooTwelve.implement).must_equal Digest::KangarooTwelve_64
    _(Digest::KangarooTwelve_64.digest_length).must_equal 64
  end

  it "produces instances that are consistent with produced implementation classes" do
    _(Digest::KangarooTwelve[32].new.class).must_equal Digest::KangarooTwelve_32
    _(Digest::KangarooTwelve.implement(digest_length: 48)).must_equal Digest::KangarooTwelve_48
  end

  it "produces classes with equal digest length as its instances" do
    _(Digest::KangarooTwelve[32].new.digest_length).must_equal Digest::KangarooTwelve_32.digest_length
    _(Digest::KangarooTwelve.implement(digest_length: 48).new.digest_length).must_equal Digest::KangarooTwelve_48.digest_length
  end

  it "produces hashes" do
    _(Digest::KangarooTwelve.default.digest("").class).must_equal String
    _(Digest::KangarooTwelve.default.hexdigest("").class).must_equal String
    _(Digest::KangarooTwelve.default.new.digest("").class).must_equal String
    _(Digest::KangarooTwelve.default.new.hexdigest("").class).must_equal String
  end

  it "produces similar output with its digest and hexdigest methods" do
    digest_a = Digest::KangarooTwelve[32].digest("abcd")
    _(digest_a.class).must_equal String
    digest_b = Digest::KangarooTwelve[32].new.digest("abcd")
    _(digest_b.class).must_equal String
    _(digest_a).must_equal digest_b
    hex_digest_a = Digest::KangarooTwelve[32].hexdigest("abcd")
    _(hex_digest_a.class).must_equal String
    hex_digest_b = Digest::KangarooTwelve[32].new.hexdigest("abcd")
    _(hex_digest_b.class).must_equal String
    _(hex_digest_a).must_equal hex_digest_b
    _(hex_digest_a).must_equal hex_encode(digest_a)
  end

  it "works with customization strings" do
    Digest::KangarooTwelve.implement(customization: "abcd").new.digest("")
  end

  it "produces implementations with small and long option names" do
    a = Digest::KangarooTwelve.implement(n: "KangarooTwelveTestA", d: 48, c: "abcd")
    b = Digest::KangarooTwelve.implement(name: "KangarooTwelveTestB", digest_length: 48, customization: "abcd")
    c = Digest::KangarooTwelve.implement(name: "KangarooTwelveTestC", digest_length: 48, customization_hex: "61626364")
    d = Digest::KangarooTwelve.implement(name: "KangarooTwelveTestD", digest_length: 48, ch: "61626364")
    _(a.name).must_equal "Digest::KangarooTwelveTestA"
    _(b.name).must_equal "Digest::KangarooTwelveTestB"
    _(a.digest_length).must_equal 48
    _(b.digest_length).must_equal a.digest_length
    _(a.customization).must_equal "abcd"
    _(b.customization).must_equal a.customization
    _(c.customization).must_equal a.customization
    _(d.customization).must_equal a.customization
  end

  it "has a customization method that returns nil when customization string is undefined" do
    _(Digest::KangarooTwelve.implement.customization).must_be_nil
  end

  it "has a customization_hex method that returns hex of customization string, or nil" do
    _(Digest::KangarooTwelve.implement(c: "abcd").customization_hex).must_equal "61626364"
    _(Digest::KangarooTwelve.implement.customization_hex).must_be_nil
  end

  it "has a declared block length of 8192 bytes" do
    _(Digest::KangarooTwelve::BLOCK_LENGTH).must_equal 8192
    _(Digest::KangarooTwelve.default.block_length).must_equal 8192
    _(Digest::KangarooTwelve.default.new.block_length).must_equal 8192
  end

  it "produces valid hashes" do
    # KangarooTwelve(M=empty, C=empty, 32 bytes):
    _(Digest::KangarooTwelve[32].new.hexdigest("")).must_equal "1ac2d450fc3b4205d19da7bfca1b37513c0803577ac7167f06fe2ce1f0ef39e5"
    # KangarooTwelve(M=empty, C=empty, 64 bytes):
    _(Digest::KangarooTwelve[64].new.hexdigest("")).must_equal "1ac2d450fc3b4205d19da7bfca1b37513c0803577ac7167f06fe2ce1f0ef39e54269c056b8c82e48276038b6d292966cc07a3d4645272e31ff38508139eb0a71"
    # KangarooTwelve(M=empty, C=empty, 10032 bytes), last 32 bytes:
    _(Digest::KangarooTwelve[10032].new.hexdigest("")[-64..-1]).must_equal "e8dc563642f7228c84684c898405d3a834799158c079b12880277a1d28e2ff6d"

    # KangarooTwelve(M=pattern 0x00 to 0xFA for 17^i bytes, C=empty, 32 bytes):
    [
      [0, "2bda92450e8b147f8a7cb629e784a058efca7cf7d8218e02d345dfaa65244a1f"],
      [1, "6bf75fa2239198db4772e36478f8e19b0f371205f6a9a93a273f51df37122888"],
      [2, "0c315ebcdedbf61426de7dcf8fb725d1e74675d7f5327a5067f367b108ecb67c"],
      [3, "cb552e2ec77d9910701d578b457ddf772c12e322e4ee7fe417f92c758f0d59d0"],
      [4, "8701045e22205345ff4dda05555cbb5c3af1a771c2b89baef37db43d9998b9fe"],
      [5, "844d610933b1b9963cbdeb5ae3b6b05cc7cbd67ceedf883eb678a0a8e0371682"],
      [6, "3c390782a8a4e89fa6367f72feaaf13255c8d95878481d3cd8ce85f58e880af8"]
    ].each do |i, hash|
      m = get_repeated_0x00_to_0xfa(17 ** i)
      _(Digest::KangarooTwelve[32].new.hexdigest(m)).must_equal hash
    end
  end

  it "produces valid hashes with customization strings" do
    # KangarooTwelve(M=i times byte 0xFF, C=pattern 0x00 to 0xFA for 41^j bytes, 32 bytes):
    [
      [0, 0, "fab658db63e94a246188bf7af69a133045f46ee984c56e3c3328caaf1aa1a583"],
      [1, 1, "d848c5068ced736f4462159b9867fd4c20b808acc3d5bc48e0b06ba0a3762ec4"],
      [3, 2, "c389e5009ae57120854c2e8c64670ac01358cf4c1baf89447a724234dc7ced74"],
      [7, 3, "75d2f86a2e644566726b4fbcfc5657b9dbcf070c7b0dca06450ab291d7443bcf"]
    ].each do |i, j, hash|
      m = get_repeated_0xff(i)
      c = get_repeated_0x00_to_0xfa(41 ** j)
      _(Digest::KangarooTwelve.implement(digest_length: 32, customization: c).new.hexdigest(m)).must_equal hash
    end
  end

  it "must have VERSION constant" do
    _(Digest::KangarooTwelve.constants).must_include :VERSION
  end
end
