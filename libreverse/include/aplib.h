#ifndef APLIB_H
#define APLIB_H

#include <cstdint>
#include <cstddef>

// Decompresses an aPlib-compressed block of data.
uint32_t aplib_decompress(const uint8_t* compressed, uint8_t* decompressed);

#endif
