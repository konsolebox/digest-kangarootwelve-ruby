/*
 * These works are licensed under the Creative Commons Attribution-ShareAlike
 * 3.0 Unported License.  To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-sa/3.0/ or send a letter to Creative
 * Commons, PO Box 1866, Mountain View, CA 94042, USA.
 */

#ifndef UTILS_H
#define UTILS_H

/*
 * A simplified hex encoder based on Yannuth's answer in StackOverflow
 * (https://stackoverflow.com/a/17147874/445221).
 *
 * Length of `dest[]` is implied to be twice of `len`.
 */
static void hex_encode_str_implied(const unsigned char *src, size_t len, unsigned char *dest)
{
	static const unsigned char table[] = "0123456789abcdef";

	for (; len > 0; --len) {
		unsigned char c = *src++;
		*dest++ = table[c >> 4];
		*dest++ = table[c & 0x0f];
	}
}

/*
 * Decodes hex string.
 *
 * Length of `dest[]` is implied to be calculated with calc_hex_decoded_str_length.
 *
 * Returns nonzero is successful.
 */
static int hex_decode_str_implied(const unsigned char *src, size_t len, unsigned char *dest)
{
	unsigned char low, high;

	if (len % 2) {
		low = *src++;

		if (low >= '0' && low <= '9') {
			low -= '0';
		} else if (low >= 'A' && low <= 'F') {
			low -= 'A' - 10;
		} else if (low >= 'a' && low <= 'f') {
			low -= 'a' - 10;
		} else {
			return 0;
		}

		*dest++ = low;
		--len;
	}

	for (; len > 0; len -= 2) {
		high = *src++;

		if (high >= '0' && high <= '9') {
			high -= '0';
		} else if (high >= 'A' && high <= 'F') {
			high -= 'A' - 10;
		} else if (high >= 'a' && high <= 'f') {
			high -= 'a' - 10;
		} else {
			return 0;
		}

		low = *src++;

		if (low >= '0' && low <= '9') {
			low -= '0';
		} else if (low >= 'A' && low <= 'F') {
			low -= 'A' - 10;
		} else if (low >= 'a' && low <= 'f') {
			low -= 'a' - 10;
		} else {
			return 0;
		}

		*dest++ = high << 4 | low;
	}

	return -1;
}

/*
 * Calculates length of string that would store decoded hex.
 */
static size_t calc_hex_decoded_str_length(size_t hex_encoded_length)
{
	if (hex_encoded_length == 0)
		return 0;

	if (hex_encoded_length % 2)
		++hex_encoded_length;

	return hex_encoded_length / 2;
}

#endif
