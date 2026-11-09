#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

// Prints the characters in the input string all in escaped notation.
std::string string_escape_all(const std::string& input);

// Prints all readable characters in a given input string.
std::string string_escape_unreadable(const std::string& input);

// Converts the raw byte public RSA modulus to a hex string that can be imported by OpenSSL.
std::string base16_encode(const uint8_t* raw_bytes, const unsigned int size);

// Converts a Unicode string to ANSI (multi-byte).
// This function naively truncates the wide character, so it only supports low ASCII characters.
std::string unicode_to_ansi_windows_naive(const std::vector<uint16_t>& unicode);

// Counts the number of occurrences 'substr' occurs in 'data'.
uint32_t count_string_occurrences(const std::string& data, const std::string& substr);

// Splits a string into multiple fields.
void split_string_by_separator(const std::string& data, const std::string& sep, std::vector<std::string>& fields);

// Checks whether input string is a valid hex-encoded value.
bool is_string_valid_hex(const std::string& str);

// Reads all lines from a given input file.
bool read_lines_from_file(const std::string& filename, std::vector<std::string>& lines);

// XORs all bytes in a string with the 'xor' byte.
std::string xor_string(const std::string& input, const uint8_t xor_val);

#endif
