#ifndef HASH_H
#define HASH_H

#include <string>

// Calculates the MD5 hash of the input data buffer 'str'.
std::string md5(const std::string& str);

// Calculates the SHA1 hash of the input data buffer 'str'.
std::string sha1(const std::string& str);

// Calculates the SHA256 hash of the input data buffer 'str'.
std::string sha256(const std::string& str);

// Calculates the PJW hash value (https://en.wikipedia.org/wiki/PJW_hash_function).
uint32_t pjw(const uint8_t* input, const uint32_t length);

// DJB2 hash function, gives a 32-bit unsigned hash value using a string input.
uint32_t djb2(const uint8_t* data, const uint32_t length);

#endif // HAS_H
