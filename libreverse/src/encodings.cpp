#include "encodings.h"
#include <cassert>

// Base64-encoding function, accepts string and returns string,
static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string base64_encode(const std::string& bindata)
{
	const size_t binlen = bindata.size();

	// Use = signs so the end is properly padded.
	std::string retval((((binlen + 2) / 3) * 4), '=');
	size_t outpos = 0;
	int bits_collected = 0;
	unsigned int accumulator = 0;
	const std::string::const_iterator binend = bindata.end();

	for (std::string::const_iterator i = bindata.begin(); i != binend; ++i)
	{
		accumulator = (accumulator << 8) | (*i & 0xffu);
		bits_collected += 8;
		while (bits_collected >= 6)
		{
			bits_collected -= 6;
			retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
		}
	}
	if (bits_collected > 0)
	{
		assert(bits_collected < 6);
		accumulator <<= 6 - bits_collected;
		retval[outpos++] = b64_table[accumulator & 0x3fu];
	}
	assert(outpos >= (retval.size() - 2));
	assert(outpos <= retval.size());
	return retval;
}

// Encoding table for Base64 decoding.
const encoding_table<8, 6>& base64_decode()
{
	static bool initialized(false);
	static encoding_table<8, 6> table;
	if (!initialized)
	{
		table.from_string("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
		table.ignore_tail_element('=');
		initialized = true;
	}
	return table;
}

// Encoding table for Base16 encoding.
const encoding_table<4, 8>& base16_encode()
{
	static bool initialized(false);
	static encoding_table<4, 8> table;
	if (!initialized)
	{
		table.to_string("0123456789abcdef");
		initialized = true;
	}
	return table;
}

// Encoding table for Base16 decoding.
const encoding_table<8, 4>& base16_decode()
{
	static bool initialized(false);
	static encoding_table<8, 4> table;
	if (!initialized)
	{
		table.from_string("0123456789abcdef");
		table.from_string("0123456789ABCDEF");
		initialized = true;
	}
	return table;
}
