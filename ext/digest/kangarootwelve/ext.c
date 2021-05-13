/*
 * Copyright (c) 2021 konsolebox
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <ruby.h>
#include <ruby/digest.h>

#include "KangarooTwelve.h"
#include "utils.h"

#define KT_DEFAULT_DIGEST_LENGTH 64 /* 512 bits */
#define KT_BLOCK_LENGTH 8192 /* chunkSize */
#define KT_MIN_DIGEST_LENGTH 1

#define KT_DIGEST_API_VERSION_IS_SUPPORTED(version) (version == 3)

#if !(KT_DIGEST_API_VERSION_IS_SUPPORTED(RUBY_DIGEST_API_VERSION))
#	error Digest API version is not supported.
#endif

#define KT_DEBUG(...) fprintf(stderr, __VA_ARGS__)

static ID _id_auto;
static ID _id_block_length;
static ID _id_b;
static ID _id_customization;
static ID _id_customization_hex;
static ID _id_c;
static ID _id_ch;
static ID _id_default;
static ID _id_digest_length;
static ID _id_d;
static ID _id_hexdigest;
static ID _id_metadata;
static ID _id_name;
static ID _id_new;
static ID _id_n;
static ID _id_unpack;

static VALUE _Digest;
static VALUE _Digest_KangarooTwelve;
static VALUE _Digest_KangarooTwelve_Impl;
static VALUE _Digest_KangarooTwelve_Metadata;

typedef struct {
	KangarooTwelve_Instance instance;
	VALUE customization;
} kangarootwelve_context_t;

#define KT_CONTEXT kangarootwelve_context_t
#define KT_CONTEXT_PTR(void_ctx_ptr) ((KT_CONTEXT *) void_ctx_ptr)

#define _RSTRING_PTR_U(x) (unsigned char *)RSTRING_PTR(x)

static void check_digest_length(int digest_length)
{
	if (!(digest_length >= KT_MIN_DIGEST_LENGTH))
		rb_raise(rb_eArgError, "Digest length lesser than minimum (%d): %d", KT_MIN_DIGEST_LENGTH,
				digest_length);
}

static VALUE hex_encode_str(VALUE str)
{
	int len;
	VALUE hex;

	len = RSTRING_LEN(str);
	hex = rb_str_new(0, len * 2);
	hex_encode_str_implied(_RSTRING_PTR_U(str), len, _RSTRING_PTR_U(hex));
	return hex;
}

static VALUE hex_decode_str(VALUE str)
{
	int len;
	VALUE decoded;

	len = RSTRING_LEN(str);
	decoded = rb_str_new(0, calc_hex_decoded_str_length(len));

	if (! hex_decode_str_implied(_RSTRING_PTR_U(str), len, _RSTRING_PTR_U(decoded))) {
		VALUE inspect = rb_inspect(str);
		rb_raise(rb_eArgError, "Failed to decode hex string %s.", RSTRING_PTR(inspect));
	}

	return decoded;
}

static int kangarootwelve_init(void *ctx)
{
	VALUE klass_or_instance, digest_length, digest_length_int, customization;

	if (ctx == NULL)
		rb_raise(rb_eRuntimeError, "Context pointer is NULL.");

	klass_or_instance = rb_current_receiver();

	if (TYPE(klass_or_instance) == T_CLASS && klass_or_instance == _Digest_KangarooTwelve_Impl)
		rb_raise(rb_eStandardError, "Digest::KangarooTwelve::Impl is a base class and cannot be "
				"instantiated.");

	digest_length = rb_funcall(klass_or_instance, _id_digest_length, 0);

	if (TYPE(digest_length) != T_FIXNUM)
		rb_raise(rb_eTypeError, "Invalid object type for digest length.");

	digest_length_int = FIX2INT(digest_length);

	check_digest_length(digest_length_int);

	customization = rb_funcall(klass_or_instance, _id_customization, 0);

	if (TYPE(customization) != T_NIL && TYPE(customization) != T_STRING)
		rb_raise(rb_eTypeError, "Invalid object type for a customization string.");

	KT_CONTEXT_PTR(ctx)->customization = customization;

	if (KangarooTwelve_Initialize(&KT_CONTEXT_PTR(ctx)->instance, digest_length_int) != 0)
		rb_raise(rb_eRuntimeError, "Failed to initialize hash data instance.");

	return 1;
}

static void kangarootwelve_update(void *ctx, unsigned char *data, size_t length)
{
	if (ctx == NULL)
		rb_raise(rb_eRuntimeError, "Context pointer is NULL.");

	if (data == NULL)
		rb_raise(rb_eRuntimeError, "Data pointer is NULL.");

	if (KangarooTwelve_Update(&KT_CONTEXT_PTR(ctx)->instance, data, length) != 0)
		rb_raise(rb_eRuntimeError, "Hash update failed.");
}

static int kangarootwelve_finish(void *ctx, unsigned char *data)
{
	VALUE customization;

	if (ctx == NULL)
		rb_raise(rb_eRuntimeError, "Context pointer is NULL.");

	customization = KT_CONTEXT_PTR(ctx)->customization;

	switch (TYPE(customization)) {
	case T_NIL:
		return KangarooTwelve_Final(&KT_CONTEXT_PTR(ctx)->instance, data, 0, 0) == 0;
	case T_STRING:
		return KangarooTwelve_Final(&KT_CONTEXT_PTR(ctx)->instance, data,
				_RSTRING_PTR_U(customization), RSTRING_LEN(customization)) == 0;
	default:
		rb_raise(rb_eRuntimeError, "Object type of customization string became invalid.");
	}
}

static VALUE implement(VALUE name, VALUE digest_length, VALUE customization)
{
	VALUE impl_class, impl_class_name, prev_digest_length, metadata_obj;
	ID impl_class_name_id, id;
	int digest_length_int, prev_digest_length_int;
	rb_digest_metadata_t *metadata;

	if (!KT_DIGEST_API_VERSION_IS_SUPPORTED(RUBY_DIGEST_API_VERSION))
		rb_raise(rb_eRuntimeError, "Digest API version is not supported.");

	switch (TYPE(digest_length)) {
	case T_NIL:
		digest_length_int = KT_DEFAULT_DIGEST_LENGTH;
		break;
	case T_FIXNUM:
		digest_length_int = FIX2INT(digest_length);
		check_digest_length(digest_length);
		break;
	default:
		rb_raise(rb_eTypeError, "Invalid value type for digest length.");
	}

	switch (TYPE(customization)) {
	case T_NIL:
	case T_STRING:
		break;
	default:
		rb_raise(rb_eTypeError, "Invalid value type for customization string.");
	}

	impl_class_name = Qnil;

	switch (TYPE(name)) {
	case T_NIL:
		break;
	case T_SYMBOL:
		id = SYM2ID(name);

		if (id == _id_auto) {
			if (customization != Qnil) {
				impl_class_name = hex_encode_str(customization);
			} else {
				impl_class_name = rb_sprintf("KangarooTwelve_%d", digest_length_int);
			}

			impl_class_name_id = rb_intern_str(impl_class_name);
		} else {
			VALUE symbol_inspect = rb_inspect(name);
			rb_raise(rb_eArgError, "Invalid symbol argument for class name: %s",
					StringValueCStr(symbol_inspect));
		}

		break;
	case T_STRING:
		id = rb_intern_str(name);

		if (!rb_is_const_id(id))
			rb_raise(rb_eArgError, "Not a valid class name: %s", StringValueCStr(name));

		impl_class_name = name;
		impl_class_name_id = id;
		break;
	default:
		rb_raise(rb_eTypeError, "Invalid argument type for class name.");
	}

	if (impl_class_name == Qnil) {
		impl_class = rb_funcall(rb_cClass, _id_new, 1, _Digest_KangarooTwelve_Impl);
	} else {
		if (rb_const_defined(_Digest, impl_class_name_id)) {
			impl_class = rb_const_get(_Digest, impl_class_name_id);

			if (TYPE(impl_class) != T_CLASS) {
				rb_raise(rb_eTypeError,
						"Digest::%s was already defined but is not a class.",
						StringValueCStr(impl_class_name));
			}

			if (rb_class_superclass(impl_class) != _Digest_KangarooTwelve_Impl) {
				rb_raise(rb_eTypeError, "Digest::%s was already defined but not derived from "
						"Digest::KangarooTwelve::Impl.", StringValueCStr(impl_class_name));
			}

			prev_digest_length = rb_ivar_get(impl_class, _id_digest_length);

			if (TYPE(prev_digest_length) != T_FIXNUM) {
				rb_raise(rb_eRuntimeError, "Previous definition of Digest::%s has invalid digest "
						"length value type.", StringValueCStr(impl_class_name));
			}

			prev_digest_length_int = FIX2INT(prev_digest_length);

			if (prev_digest_length_int != digest_length_int) {
				rb_raise(rb_eTypeError, "Digest::%s was already defined but has different digest "
						"length (%d instead of %d).", StringValueCStr(impl_class_name),
						prev_digest_length_int, digest_length_int);
			}

			return impl_class;
		}

		impl_class = rb_define_class_id_under(_Digest, impl_class_name_id,
				_Digest_KangarooTwelve_Impl);
	}

	metadata_obj = Data_Make_Struct(_Digest_KangarooTwelve_Metadata, rb_digest_metadata_t, 0, -1,
			metadata);

	metadata->api_version = RUBY_DIGEST_API_VERSION;
	metadata->digest_len = digest_length_int;
	metadata->block_len = KT_BLOCK_LENGTH;
	metadata->ctx_size = sizeof(KT_CONTEXT);
	metadata->init_func = kangarootwelve_init;
	metadata->update_func = kangarootwelve_update;
	metadata->finish_func = kangarootwelve_finish;

	rb_ivar_set(impl_class, _id_metadata, metadata_obj);
	rb_ivar_set(impl_class, _id_digest_length, INT2FIX(digest_length_int));
	rb_ivar_set(impl_class, _id_block_length, INT2FIX(KT_BLOCK_LENGTH));
	rb_ivar_set(impl_class, _id_customization, customization);

	return impl_class;
}

/*
 * Document-module: Digest::KangarooTwelve
 *
 * The Digest::KangarooTwelve module is the main component of the KangarooTwelve
 * extension for Ruby.
 *
 * It contains singleton methods that can be used to create implementation
 * classes.  These implementation classes can then be used to create instances
 * for producing hashes.
 *
 * Example:
 *
 * <tt>Digest::KangarooTwelve[32].new.update("one").update("two").digest</tt>
 *
 * The implementation classes can also be used to directly produce hashes.
 *
 * Example:
 *
 * <tt>Digest::KangarooTwelve[32].hexdigest("1234")</tt>
 *
 * The produced implementation classes and their instances can be used just like
 * any other classes and instances in the Digest namespace that's based on
 * Digest::Base.
 */

/*
 * call-seq: Digest::KangarooTwelve.default -> klass
 *
 * Returns the default implementation class which has a digest length of 64
 * bytes, and doesn't have a customization string.
 */
static VALUE _Digest_KangarooTwelve_singleton_default(VALUE self)
{
	VALUE default_ = Qnil;

	if (rb_ivar_defined(self, _id_default) == Qtrue)
		default_ = rb_ivar_get(self, _id_default);

	if (NIL_P(default_)) {
		default_ = implement(ID2SYM(_id_auto), INT2FIX(KT_DEFAULT_DIGEST_LENGTH), Qnil);
		rb_ivar_set(self, _id_default, default_);
	}

	Check_Type(default_, T_CLASS);
	return default_;
}

/*
 * call-seq: Digest::KangarooTwelve.implement(**opts) -> klass
 *
 * The Digest::KangarooTwelve::implement method is more configurable than the
 * Digest::KangarooTwelve::[] method, and can implement classes with
 * customization strings, a custom block length, and a different or anonymous
 * name.
 *
 * The method can be called with the following options:
 *
 * :n, :name ::
 *   Specifies the name of the class, which would be placed in the Digest
 *   module.
 *
 *   The default value for this option is +:auto+, and it implies automatic
 *   generation of the name.  The generated name is in the form of
 *   Digest::KangarooTwelve_<digest_length> if a customization string is not
 *   specified.
 *
 *   If a customization string is specified, the format would be
 *   Digest::KangarooTwelve_<digest_length>_<cust_str_hex>.
 *
 *   Specifying a string would make the method produce Digest::<string>.
 *
 *   Specifying +nil+ would produce an anonymous class. I.e., a class not
 *   assigned to any constant, and has no name returned with Class#name,
 *   but +nil+.
 *
 * :d, :digest_length ::
 *   Specifies the digest length.
 *
 *   See Digest::KangarooTwelve::DEFAULT_DIGEST_LENGTH for the default value.
 *
 * :c, :customization ::
 *   Specifies the customization string.  Adding a customization string changes
 *   the resulting digest of every input.
 *
 * :ch, :customization_hex ::
 *   Specifies the customization string in hex mode.
 *
 * Calling the method with no argument is the same as calling the
 * Digest::KangarooTwelve::default method.
 */
static VALUE _Digest_KangarooTwelve_singleton_implement(int argc, VALUE *argv, VALUE self)
{
	VALUE opts, name, digest_length, customization;

	rb_scan_args(argc, argv, "0:", &opts);

	if (NIL_P(opts)) {
		name = ID2SYM(_id_auto);
		digest_length = customization = Qnil;
	} else {
		name = rb_hash_lookup2(opts, ID2SYM(_id_n), Qundef);

		if (name == Qundef)
			name = rb_hash_lookup2(opts, ID2SYM(_id_name), ID2SYM(_id_auto));

		digest_length = rb_hash_lookup2(opts, ID2SYM(_id_d), Qundef);

		if (digest_length == Qundef)
			digest_length = rb_hash_lookup2(opts, ID2SYM(_id_digest_length), Qnil);

		customization = rb_hash_lookup2(opts, ID2SYM(_id_c), Qundef);

		if (customization == Qundef)
			customization = rb_hash_lookup2(opts, ID2SYM(_id_customization), Qundef);

		if (customization == Qundef) {
			VALUE customization_hex = rb_hash_lookup2(opts, ID2SYM(_id_customization_hex), Qundef);

			if (customization_hex == Qundef)
				customization_hex = rb_hash_lookup2(opts, ID2SYM(_id_ch), Qundef);

			if (customization_hex == Qundef) {
				customization = Qnil;
			} else {
				if (TYPE(customization_hex) != T_STRING)
					rb_raise(rb_eTypeError, "Customization argument not a string.");

				customization = hex_decode_str(customization_hex);
			}
		}
	}

	return implement(name, digest_length, customization);
}

/*
 * call-seq: Digest::KangarooTwelve[digest_length] -> klass
 *
 * Creates an implementation class with specified digest length.  This method
 * is the simpler form of Digest::KangarooTwelve.implement.
 *
 * The class is generated in the name form of
 * Digest::KangarooTwelve_<digest_length>, and can be directly referenced after
 * this method is called.
 */
static VALUE _Digest_KangarooTwelve_singleton_implement_simple(VALUE self, VALUE digest_length)
{
	return implement(ID2SYM(_id_auto), digest_length, Qnil);
}

/*
 * Document-class: Digest::KangarooTwelve::Impl
 *
 * This is the base class for KangarooTwelve's implementation classes,
 * and it adds additional class methods and instance methods to the methods
 * provided by Digest::Base, which can be used with the implementaion classes
 * and the hashing objects respectively.
 */

/*
 * call-seq: new -> obj
 *
 * Creates a new object instance of the implementation class.
 */
static VALUE _Digest_KangarooTwelve_Impl_singleton_new(VALUE self)
{
	if (self == _Digest_KangarooTwelve_Impl)
		rb_raise(rb_eRuntimeError, "Digest::KangarooTwelve::Impl is an abstract class.");

	if (rb_ivar_defined(self, _id_metadata) == Qfalse ||
			rb_obj_class(rb_ivar_get(self, _id_metadata)) != _Digest_KangarooTwelve_Metadata)
		rb_raise(rb_eRuntimeError, "Metadata not set or invalid.  Please do not manually inherit "
				"KangarooTwelve.");

	return rb_call_super(0, 0);
}

/*
 * call-seq: digest_length -> int
 *
 * Returns configured digest length of the implementation class.
 */
static VALUE _Digest_KangarooTwelve_Impl_singleton_digest_length(VALUE self)
{
	if (self == _Digest_KangarooTwelve_Impl)
		rb_raise(rb_eRuntimeError, "Digest::KangarooTwelve::Impl is an abstract class.");

	return rb_ivar_get(self, _id_digest_length);
}

/*
 * call-seq: block_length -> int
 *
 * Returns 8192.
 */
static VALUE _Digest_KangarooTwelve_Impl_singleton_block_length(VALUE self)
{
	return INT2FIX(KT_BLOCK_LENGTH);
}

/*
 * call-seq: customization -> string or nil
 *
 * Returns configured customization string of the implementation class.
 */
static VALUE _Digest_KangarooTwelve_Impl_singleton_customization(VALUE self)
{
	if (self == _Digest_KangarooTwelve_Impl)
		rb_raise(rb_eRuntimeError, "Digest::KangarooTwelve::Impl is an abstract class.");

	return rb_ivar_defined(self, _id_customization) == Qtrue ? rb_ivar_get(self, _id_customization)
			: Qnil;
}

/*
 * call-seq: customization_hex -> string or nil
 *
 * Returns configured customization string of the implementation class in hex.
 */
static VALUE _Digest_KangarooTwelve_Impl_singleton_customization_hex(VALUE self)
{
	if (self == _Digest_KangarooTwelve_Impl)
		rb_raise(rb_eRuntimeError, "Digest::KangarooTwelve::Impl is an abstract class.");

	if (rb_ivar_defined(self, _id_customization)) {
		VALUE customization = rb_ivar_get(self, _id_customization);

		if (!NIL_P(customization)) {
			if (TYPE(customization) != T_STRING)
				rb_raise(rb_eTypeError, "Unexpected class for a customization string.");

			return hex_encode_str(customization);
		}
	}

	return Qnil;
}

/*
 * call-seq: customization -> string or nil
 *
 * Returns configured customization string of the implementation object.
 */
static VALUE _Digest_KangarooTwelve_Impl_customization(VALUE self)
{
	VALUE customization;

	if (rb_ivar_defined(self, _id_customization)) {
		customization = rb_ivar_get(self, _id_customization);
	} else {
		VALUE klass = rb_obj_class(self);
		customization = rb_ivar_get(klass, _id_customization);
		rb_ivar_set(self, _id_customization, customization);
	}

	return customization;
}

/*
 * call-seq: customization_hex -> string or nil
 *
 * Returns configured customization string of the implementation object in hex.
 */
static VALUE _Digest_KangarooTwelve_Impl_customization_hex(VALUE self)
{
	VALUE customization = rb_funcall(self, _id_customization, 0);
	return hex_encode_str(customization);
}

/*
 * call-seq: inspect -> string
 *
 * Returns a string in the form of #<impl_class_name|digest_length|hex_cust_string|hex_digest>
 */
static VALUE _Digest_KangarooTwelve_Impl_inspect(VALUE self)
{
	VALUE klass, klass_name, digest_length, customization_hex, hexdigest, args[4];

	klass = rb_obj_class(self);
	klass_name = rb_class_name(klass);

	if (klass_name == Qnil)
		klass_name = rb_inspect(klass);

	digest_length = rb_funcall(self, _id_digest_length, 0);
	customization_hex = rb_funcall(self, _id_customization_hex, 0);
	hexdigest = rb_funcall(self, _id_hexdigest, 0);

	args[0] = klass_name;
	args[1] = digest_length;
	args[2] = customization_hex;
	args[3] = hexdigest;
	return rb_str_format(sizeof(args), args, rb_str_new_literal("#<%s:%d|%s|%s>"));
}

/*
 * Init
 */

void Init_kangarootwelve()
{
	#define DEFINE_ID(x) _id_##x = rb_intern_const(#x);

	DEFINE_ID(auto)
	DEFINE_ID(block_length)
	DEFINE_ID(b)
	DEFINE_ID(ch)
	DEFINE_ID(customization)
	DEFINE_ID(customization_hex)
	DEFINE_ID(c)
	DEFINE_ID(default)
	DEFINE_ID(digest_length)
	DEFINE_ID(d)
	DEFINE_ID(hexdigest)
	DEFINE_ID(metadata)
	DEFINE_ID(name)
	DEFINE_ID(new)
	DEFINE_ID(n)
	DEFINE_ID(unpack)

	rb_require("digest");
	_Digest = rb_path2class("Digest");

	#if 0
	/* Tell RDoc about Digest since it doesn't recognize rb_path2class. */
	_Digest = rb_define_module("Digest");
	#endif

	/*
	 * Document-module: Digest::KangarooTwelve
	 */

	_Digest_KangarooTwelve = rb_define_module_under(_Digest, "KangarooTwelve");

	rb_define_singleton_method(_Digest_KangarooTwelve, "default",
			_Digest_KangarooTwelve_singleton_default, 0);
	rb_define_singleton_method(_Digest_KangarooTwelve, "implement",
			_Digest_KangarooTwelve_singleton_implement, -1);
	rb_define_singleton_method(_Digest_KangarooTwelve, "[]",
			_Digest_KangarooTwelve_singleton_implement_simple, 1);

	/*
	 * Document-const: Digest::KangarooTwelve::BLOCK_LENGTH
	 *
	 * 8192 bytes
	 */

	/* 8192 bytes */

	rb_define_const(_Digest_KangarooTwelve, "BLOCK_LENGTH", INT2FIX(KT_BLOCK_LENGTH));

	/*
	 * Document-const: Digest::KangarooTwelve::DEFAULT_DIGEST_LENGTH
	 *
	 * 64 bytes (512 bits)
	 */

	/* 64 bytes (512 bits) */

	rb_define_const(_Digest_KangarooTwelve, "DEFAULT_DIGEST_LENGTH",
			INT2FIX(KT_DEFAULT_DIGEST_LENGTH));

	/*
	 * Document-class: Digest::KangarooTwelve::Impl
	 */

	_Digest_KangarooTwelve_Impl = rb_define_class_under(_Digest_KangarooTwelve, "Impl",
			rb_path2class("Digest::Base"));

	rb_define_singleton_method(_Digest_KangarooTwelve_Impl, "new",
			_Digest_KangarooTwelve_Impl_singleton_new, 0);
	rb_define_singleton_method(_Digest_KangarooTwelve_Impl, "block_length",
			_Digest_KangarooTwelve_Impl_singleton_block_length, 0);
	rb_define_singleton_method(_Digest_KangarooTwelve_Impl, "digest_length",
			_Digest_KangarooTwelve_Impl_singleton_digest_length, 0);
	rb_define_singleton_method(_Digest_KangarooTwelve_Impl, "customization",
			_Digest_KangarooTwelve_Impl_singleton_customization, 0);
	rb_define_singleton_method(_Digest_KangarooTwelve_Impl, "customization_hex",
			_Digest_KangarooTwelve_Impl_singleton_customization_hex, 0);

	rb_define_method(_Digest_KangarooTwelve_Impl, "customization",
			_Digest_KangarooTwelve_Impl_customization, 0);
	rb_define_method(_Digest_KangarooTwelve_Impl, "customization_hex",
			_Digest_KangarooTwelve_Impl_customization_hex, 0);
	rb_define_method(_Digest_KangarooTwelve_Impl, "inspect",
			_Digest_KangarooTwelve_Impl_inspect, 0);

	/*
	 * Document-class: Digest::KangarooTwelve::Metadata
	 *
	 * This class represents the internal metadata produced for the
	 * implementation classes.
	 */

	_Digest_KangarooTwelve_Metadata = rb_define_class_under(_Digest_KangarooTwelve, "Metadata",
			rb_cObject);
}
