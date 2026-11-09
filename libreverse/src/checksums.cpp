#include "hash.h"

// Calculates the PJW hash value (https://en.wikipedia.org/wiki/PJW_hash_function).
uint32_t pjw(const uint8_t* input, const uint32_t length)
{
	uint32_t checksum = 0;
	uint32_t high = 0;
	for (uint32_t i = 0; i < length; ++i)
	{
		checksum = (checksum << 4) + input[i];
		high = checksum & 0xF0000000;
		if (high)
		{
			checksum ^= high >> 24;
		}
		checksum &= ~high;
	}
	return checksum;
}

// DJB2 hash function, gives a 32-bit unsigned hash value using a string input.
uint32_t djb2(const uint8_t* data, const uint32_t length)
{
	uint32_t hash = 5381;
	for (uint32_t i = 0; i < length; ++i)
	{
		hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
	}
	return hash;
}
