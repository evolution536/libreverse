#include <string_util.h>
#include <sstream>
#include <vector>
#include <regex>
#include <fstream>

// Prints the characters in the input string all in escaped notation.
std::string string_escape_all(const std::string& input)
{
        std::stringstream ss;
        ss << std::hex;
        for (const auto c : input)
        {
                ss << "\\x";
                if ((c & 0xFF) < 16)
                {
                        ss << "0";
                }
                ss << (c & 0xFF);
        }
        return ss.str();
}

// Prints all readable characters in a given input string.
std::string string_escape_unreadable(const std::string& input)
{
	std::stringstream ss;
	ss << std::hex;
	for (const auto c : input)
	{
		if (c >= 0x20 && c < 0x7F)
		{
			ss << c;
		}
		else
		{
			ss << "\\x";
			if ((c & 0xFF) < 16)
			{
				ss << "0";
			}
			ss << (c & 0xFF);
		}
	}
	return ss.str();
}

// Converts the raw byte public RSA modulus to a hex string that can be imported by OpenSSL.
static const char* base16_alphabet = "0123456789ABCDEF";
std::string base16_encode(const uint8_t* raw_bytes, const unsigned int size)
{
	std::vector<char> out(2 * size + 1);
	uint32_t j = 0;
	for (uint32_t i = 0; i < size; ++i)
	{
		out[j++] = base16_alphabet[raw_bytes[i] >> 4];
		out[j++] = base16_alphabet[raw_bytes[i] & 0xF];
	}
	out[j] = 0;
	return std::string(out.begin(), out.end());
}

// Converts a Unicode string to ANSI (multi-byte).
// This function naively truncates the wide character, so it only supports low ASCII characters.
std::string unicode_to_ansi_windows_naive(const std::vector<uint16_t>& unicode)
{
	std::vector<char> ansi;
	ansi.reserve(unicode.size());
	for (const auto w : unicode)
	{
		ansi.push_back((char)w);
	}
	return std::string(ansi.begin(), ansi.end());
}

// Splits a string into multiple fields.
void split_string_by_separator(const std::string& data, const std::string& sep, std::vector<std::string>& fields)
{
        size_t prev = 0;
        size_t current = data.find(sep);
        while (current != std::string::npos)
        {
                fields.push_back(data.substr(prev, current - prev));
                prev = current + sep.size();
                current = data.find(sep, prev);
        }
        fields.push_back(data.substr(prev, current - prev));
}

// Counts the number of occurrences 'substr' occurs in 'data'.
uint32_t count_string_occurrences(const std::string& data, const std::string& substr)
{
        size_t pos = 0;
        uint32_t occurrences = 0;
        while ((pos = data.find(substr, pos)) != std::string::npos)
        {
                ++occurrences;
                pos += substr.size();
        }
        return occurrences;
}

// Checks whether input string is a valid hex-encoded value.
static const std::regex hex_regex("([A-Fa-f0-9]{2})+");
bool is_string_valid_hex(const std::string& str)
{
	return std::regex_match(str, hex_regex);
}

// Reads all lines from a given input file.
bool read_lines_from_file(const std::string& filename, std::vector<std::string>& lines)
{
	std::ifstream ifstr(filename); // Open in non-binary mode; that's OK here!
	if (!ifstr.is_open())
	{
		return false;
	}

	std::string line;
	while (std::getline(ifstr, line))
	{
		if (!std::regex_match(line, std::regex{"[ \\t]*"}))
		{
			lines.emplace_back(line);
		}
	}

	return true;
}

// XORs all bytes in a string with the 'xor' byte.
std::string xor_string(const std::string& input, const uint8_t xor_val)
{
	std::string result = input;
	for (uint32_t i = 0; i < result.size(); ++i)
	{
		result[i] ^= xor_val;
	}
	return result;
}
