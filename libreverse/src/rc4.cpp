#include "rc4.h"

#define N 256

// Swaps two elements at pointers a and b.
static void swap(uint8_t* a, uint8_t* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

// Performs RC4 key scheduling.
static void key_scheduling(const uint8_t* key, const uint32_t key_size, uint8_t* S)
{
	int j = 0;
	for (int i = 0; i < N; i++)
		S[i] = i;

	for (int i = 0; i < N; i++)
	{
		j = (j + S[i] + key[i % key_size]) % N;
		swap(&S[i], &S[j]);
	}
}

// Performs PRGA for RC4.
static void prga(uint8_t* S, uint8_t* plaintext, const uint32_t length)
{
	int i = 0;
	int j = 0;

	for (size_t n = 0; n < length; n++)
	{
		i = (i + 1) % N;
		j = (j + S[i]) % N;
		swap(&S[i], &S[j]);
		int rnd = S[(S[i] + S[j]) % N];
		plaintext[n] ^= rnd;
	}
}

// Performs RC4 encryption/decryption using a given key.
void rc4_crypt(uint8_t* buffer, const uint32_t size, const uint8_t* key, const uint32_t key_size)
{
	unsigned char S[N];
	key_scheduling(key, key_size, S);
	prga(S, buffer, size);
}

