#ifndef BINARY_H
#define BINARY_H

#include <cstdint>
#include <cstddef>

// Reads a 7-bit encoded integer from 'ptr' and stores it in 'out'.
// Returns the length of the encoded integer.
template <typename T>
uint32_t read_7bit_encoded_integer(const uint8_t* ptr, T* const out, const uint32_t input_size);

// Decompresses a compressed unsigned integer used in the .NET blob heaps (#Blob and #US).
// Returns the decompressed integer (reads at most 4 bytes from 'ptr').
uint32_t decompress_dotnet_unsigned_integer(const uint8_t* ptr, uint32_t* const out);

// Repeats a string (the key string) of any bytes for a given length.
void repeat_byte_string(uint8_t* buffer, const uint32_t buffer_size, const uint8_t* string, const uint32_t string_length);

// Fixes the positions of a substring of the key in the no-divide key buffer.
void repeat_fix_substring_positions(uint8_t* buffer, const uint32_t buffer_size, const uint8_t* string, const uint32_t string_length, const uint32_t offset);

// Rotates a 32-bit unsigned integer left by 'c'
uint32_t rotate_left_32 (uint32_t n, uint8_t c);

#endif
