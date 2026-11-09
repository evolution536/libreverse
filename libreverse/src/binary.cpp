#include "binary.h"
#include <cstring>
#include <string>
#include <cmath>

// Reads a 7-bit encoded integer from 'ptr' and stores it in 'out'.
// 'input_size' is the maximum number of bytes that the function is allowed to read.
// Returns the length of the encoded integer.
template <typename T>
uint32_t read_7bit_encoded_integer(const uint8_t* ptr, T* const out, const uint32_t input_size)
{
	uint32_t i = 0;
	T result = 0;

	// Read at most the input size number of bytes.
	for (; i < input_size;)
	{
		// Read the next 7 bits.
		result |= ((T)(ptr[i] & 0x7F) << (7 * i));

		// The most significant bit is set, so another byte will follow.
		if (!(ptr[i++] & 0x80))
		{
			break;
		}
	}

	// Return the decoded result and number of bytes read.
	*out = result;
	return i;
}

// Define the uint32_t and uint64_t versions of the 7-bit encoded integer read function.
template uint32_t read_7bit_encoded_integer<uint32_t>(const uint8_t* ptr, uint32_t* const out, const uint32_t input_size);
template uint32_t read_7bit_encoded_integer<uint64_t>(const uint8_t* ptr, uint64_t* const out, const uint32_t input_size);

// Decompresses a compressed unsigned integer used in the .NET blob heaps (#Blob and #US).
// Returns the decompressed integer (reads at most 4 bytes from 'ptr').
uint32_t decompress_dotnet_unsigned_integer(const uint8_t* ptr, uint32_t* const out)
{
	// Check whether the value is encoded in a single byte.
	if ((ptr[0] & 0x80) == 0)
	{
		*out = ptr[0];
		return sizeof(uint8_t);
	}
	// Check whether the value is encoded in two bytes.
	else if ((ptr[0] & 0xC0) == 0x80)
	{
		*out = (uint32_t)((ptr[0] & 0x3F) << 8 | ptr[1]);
		return sizeof(uint16_t);
	}
	// Check whether the value is encoded in four bytes.
	else if ((ptr[0] & 0xE0) == 0xC0)
	{
		*out = (uint32_t)((ptr[0] & 0x1F) << 24 | ptr[1] << 16 | ptr[2] << 8 | ptr[3]);
		return sizeof(uint32_t);
	}
	else
	{
		// None of the masks matched, so just return zero.
		return 0;
	}
}

// Repeats a string (the key string) of any bytes for a given length.
void repeat_byte_string(uint8_t* buffer, const uint32_t buffer_size, const uint8_t* string, const uint32_t string_length)
{
	// Compute the number of full repeats and whatever remains.
	const uint32_t num_key_repeats = buffer_size / string_length;
	const uint32_t remainder = buffer_size - (num_key_repeats * string_length);

	// Perform full string repeats.
	for (uint32_t j = 0; j < num_key_repeats; ++j)
	{
		memcpy(&buffer[j * string_length], string, string_length);
	}

	// Fill the remainder of the output with a partial repeat.
	memcpy(&buffer[num_key_repeats * string_length], string, remainder);
}

// Fixes the positions of a substring of the key in the no-divide key buffer.
void repeat_fix_substring_positions(uint8_t* buffer, const uint32_t buffer_size, const uint8_t* string, const uint32_t string_length, const uint32_t offset)
{
        // Compute the number of full repeats and whatever remains.
        const uint32_t num_key_repeats = buffer_size / string_length;
        const uint32_t remainder = buffer_size - (num_key_repeats * string_length);

        // Perform full string repeats.
        for (uint32_t j = 0; j < num_key_repeats; ++j)
        {
                memcpy(&buffer[j * string_length + offset], string, string_length - offset);
        }

        // Fill the remainder of the output with a partial repeat.
        if (offset < remainder)
        {
                memcpy(&buffer[num_key_repeats * string_length + offset], string, remainder - offset);
        }
}

// Rotates a 32-bit unsigned integer left by 'c'
uint32_t rotate_left_32 (uint32_t n, uint8_t c)
{
	return n << (c & 31) | n >> (-c & 31);
}
