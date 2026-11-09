#include <string>

// Computes the HMAC of given message 'm' with private key 'private_key' using the SHA256 hash.
std::string hmac_sha256(const std::string& private_key, const std::string& m);
