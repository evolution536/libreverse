#include "hash.h"
#include <string>
#include <cstdint>
#include <stdexcept>

// Rotates the value 'val' to the left by 'n' bits.
static inline uint32_t rol(uint32_t val, uint32_t n)
{
	return (val << n) | (val >> (32 - n));
}

// Calculates the SHA1 hash of the buffer 'str'.
std::string sha1(const std::string& str)
{
	const char* ptr = str.c_str();
	const int32_t size = (int32_t)str.size();
	if (size > 0x7FFFFFFF)
	{
		throw std::length_error("SHA1 not implemented on strings larger than 0x7FFFFFFF");
	}
	int32_t left = size;
	const uint64_t ml = uint64_t(size) * 8;
	uint32_t h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
	int32_t num_blocks = 1 + (size + 8) / 64;
	for (int32_t block = 0; block < num_blocks; ++block)
	{
		uint32_t w[80];
		for (size_t i = 0; i < 64; ++i)
		{
			uint32_t val;
			if (left > 0)
			{
				val = uint32_t(uint8_t(*ptr++));
			}
			else if (left == 0)
			{ // Append a single 1 bit to the input string
				val = 0x80;
			}
			else if (block < (num_blocks - 1) || i < 56)
			{ // Append zero bytes
				val = 0x0;
			}
			else
			{ // Append the 64 bits number of bits in the input (big-endian)
				val = (ml >> uint64_t(8 * (63 - i))) & 0xFF;
			}
			switch (i & 0x3)
			{
				case 0:
					w[i >> 2] = val << 24;
					break;
				case 1:
					w[i >> 2] |= val << 16;
					break;
				case 2:
					w[i >> 2] |= val << 8;
					break;
				case 3:
					w[i >> 2] |= val;
					break;
			}
			--left;
		}

		for (size_t i = 16; i < 80; ++i)
		{
			w[i] = rol( (w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16]), 1);
		}

		uint32_t a = h[0];
		uint32_t b = h[1];
		uint32_t c = h[2];
		uint32_t d = h[3];
		uint32_t e = h[4];

		for (size_t i = 0; i < 80; ++i)
		{
			uint32_t f;
			uint32_t k;

			if (i < 20)
			{
				f = (b & c) | ((~b) & d);
				k = 0x5A827999;
			}
			else if (i < 40)
			{
				f = b ^ c ^ d;
				k = 0x6ED9EBA1;
			}
			else if (i < 60)
			{
				f = (b & c) | (b & d) | (c & d);
				k = 0x8F1BBCDC;
			}
			else
			{
				f = b ^ c ^ d;
				k = 0xCA62C1D6;
			}

			uint32_t temp = rol(a, 5) + f + e + k + w[i];
			e = d;
			d = c;
			c = rol(b, 30);
			b = a;
			a = temp;
		}

		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
		h[4] += e;
	}

	std::string result;
	result.resize(20);
	for (size_t i = 0; i < 20; ++i)
	{
		result[i] = (int8_t)(h[i >> 2] >> (8 * (3 - (i & 0x3))));
	}

	return result;
}
