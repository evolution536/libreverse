#include "lznt1.h"
#include <stdio.h>
#include <cstring>
#include <string>

// Processes a compressed LZNT1 block.
static int32_t lznt1_block(const uint8_t* input, const uint32_t limit, uint32_t offset, uint8_t* const block)
{
	int32_t block_out_length = 0;
	uint32_t tag_bit = 0;
	uint32_t tag = 0;
	uint32_t split = 12;
	int32_t next_threshold = 16;
	uint8_t* copy_dest = block;

	while (offset != limit)
	{
		// Extract the tag from the block.
		if (!tag_bit)
		{
			tag = input[offset++];
			if (offset == limit)
			{
				// We reached the end of the block.
				break;
			}
		}

		// Calculate the source of where to copy data from and its length.
		if (tag & 1)
		{
			// Check whether the source and length are valid.
			if (offset + sizeof(uint16_t) > limit)
			{
				return -1;
			}

			// Start copying.
			const uint16_t* tuple = (uint16_t*)(input + offset);
			offset += sizeof(uint16_t);
			uint32_t copy_len = LZNT1_VALUE_LEN(*tuple, split);
			block_out_length += copy_len;
			if (copy_dest)
			{
				const uint8_t* copy_src = copy_dest - LZNT1_VALUE_OFFSET(*tuple, split);
				for (uint32_t i = 0; i < copy_len; ++i)
				{
					copy_dest[i] = copy_src[i];
				}
				copy_dest += copy_len;
			}
		}
		else
		{
			// We encountered an uncompressed value.
			const uint8_t* copy_src = input + offset;
			if (copy_dest)
			{
				*copy_dest++ = *copy_src;
			}
			++offset;
			++block_out_length;
		}

		// Update the split variable if necessary.
		while (block_out_length > next_threshold)
		{
			--split;
			next_threshold <<= 1;
		}

		// Finally, move to the next value in the block.
		tag >>= 1;
		tag_bit = (tag_bit + 1) % 8;
	}

	// Return the length of the decompressed block.
	return block_out_length;
}

// Decompresses data that is compressed using the Microsoft LZNT1 algorithm.
// Use this function as substitute for the RtlDecompressBuffer function with LZNT1.
int32_t decompress_lznt1_buffer(const uint8_t* input, const uint32_t length, uint8_t* const output)
{
	uint32_t offset = 0;
	const uint8_t* end;
	const uint16_t* header;
	int32_t out_length = 0;

	while (offset != length)
	{
		// Check whether the current offset is and 'end' marker.
		if (offset + sizeof(uint8_t) == length)
		{
			end = input + offset;
			if (!*end)
			{
				break;
			}
		}

		// Extract the block header for the next block.
		if (offset + sizeof(uint16_t) > length)
		{
			// We encountered a block header overrun.
			return -1;
		}
		header = (uint16_t*)(input + offset);
		offset += sizeof(uint16_t);

		// Test whether the block is compressed, and get its length.
		const uint32_t block_length = LZNT1_BLOCK_LEN(*header);
		if (LZNT1_BLOCK_COMPRESSED(*header))
		{
			const uint32_t limit = offset + block_length;
			uint8_t* block = output ? output + out_length : NULL;

			// Decompress the current block.
			const int32_t block_out_length = lznt1_block(input, limit, offset, block);
			if (block_out_length < 0)
			{
				return block_out_length;
			}

			// Increment the iterators.
			offset += block_length;
			out_length += block_out_length;
		}
		else
		{
			// The current block is not compressed.
			if (offset + block_length > length)
			{
				// Block length overrun in uncompressed block.
				return -1;
			}

			// Just copy the uncompressed input into the output.
			if (output)
			{
				memcpy(output + out_length, input + offset, block_length);
			}

			// Increment the iterators.
			offset += block_length;
			out_length += block_length;
		}
	}

	// Return the output length in bytes.
	return out_length;
}
