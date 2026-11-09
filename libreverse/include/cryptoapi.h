#ifndef CRYPTOAPI_H
#define CRYPTOAPI_H

#include <cstdint>

// Reference: https://docs.microsoft.com/en-us/windows/win32/seccrypto/base-provider-key-blobs

// Public key structure in Windows Crypto API (CryptImportKey, CryptExportKey).
struct PUBLICKEYSTRUC
{
	uint8_t Type;
	uint8_t Version;
	uint16_t Reserved;
	uint16_t KeyAlgorithm;
};

// RSA public key structure with modulus and exponent.
struct RSAPUBKEY
{
	uint32_t Magic;
	uint32_t BitLength;
	uint32_t PublicExponent;
};

// Public key blob imported or exported using Crypto API in Windows.
struct PUBLICKEYBLOB
{
	PUBLICKEYSTRUC PublicKeyStructure;
	RSAPUBKEY RSAPublicKey;
	uint8_t Modulus[0];
};

#endif
