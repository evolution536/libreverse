#include "hmac.h"
#include "hash.h"
#include "string_util.h"

#define HMAC_SHA256_BLOCK_SIZE	64
#define I_PAD 0x36
#define O_PAD 0x5C

// Computes the HMAC of given message 'm' with private key 'private_key' using the SHA256 hash.
// Implemented from pseudo-code: https://en.wikipedia.org/wiki/HMAC
std::string hmac_sha256(const std::string& private_key, const std::string& m)
{
	// Check whether the key is larger than the block size.
	// If so, we apply the hash function first.
	// The result is K' = H(K)  if len(K) > block_size and K otherwise
	std::string k_prime = private_key;
	if (private_key.size() > HMAC_SHA256_BLOCK_SIZE)
	{
		k_prime = sha256(private_key);
	}

	// Pad the key with zeroes if it is shorter than the block size.
	const auto pre_pad_key_length = k_prime.size();
	k_prime.resize(HMAC_SHA256_BLOCK_SIZE);
	for (uint32_t i = pre_pad_key_length; i < k_prime.size(); ++i)
	{
		k_prime[i] = 0;
	}
	// XOR padded key with both ipad and opad bytes.
	std::string ipad_xorred = xor_string(k_prime, I_PAD);
	std::string opad_xorred = xor_string(k_prime, O_PAD);

	// Compute hash of (K' xor ipad) | m.
	std::string h_k_xor_ipad_m = sha256(ipad_xorred + m);

	// Compute hash of K' xor opad.
	std::string h_k_xor_opad = sha256(opad_xorred);

	// Compute final hash of the first two concatenated in reverse.
	return sha256(h_k_xor_opad + h_k_xor_ipad_m);
}
