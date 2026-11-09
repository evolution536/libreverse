#ifndef LZNT1_H
#define LZNT1_H

#include <cstdint>

// Extracts the block length from the uint16_t header word.
#define LZNT1_BLOCK_LEN(header) (((header) & 0x0FFF) + 1)

// Determines whether the current block is compressed, using the header word.
#define LZNT1_BLOCK_COMPRESSED(header) ((header) & 0x8000)

// Extract the compressed value length from LZNT1 block.
#define LZNT1_VALUE_LEN(tuple, split) (((tuple) & ((1 << (split)) - 1)) + 3)

// Extracts the value offset of the next value to copy out.
#define LZNT1_VALUE_OFFSET(tuple, split) (((tuple) >> split) + 1)

// Decompresses data that is compressed using the Microsoft LZNT1 algorithm.
// Use this function as substitute for the RtlDecompressBuffer function with LZNT1.
int32_t decompress_lznt1_buffer(const uint8_t* input, const uint32_t length, uint8_t* const output);

#endif
