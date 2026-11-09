#ifndef RC4_H
#define RC4_H

#include <cstdint>
#include <cstddef>

// Performs RC4 encryption/decryption using a given key.
void rc4_crypt(uint8_t* buffer, const uint32_t size, const uint8_t* key, const uint32_t key_size);

#endif
