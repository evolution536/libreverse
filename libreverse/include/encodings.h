#ifndef ENCODINGS_H
#define ENCODINGS_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <stdexcept>

template <size_t NumSrcBits, size_t NumDstBits>
class encoding_table;

template <size_t NumSrcBits, size_t NumDstBits>
class encoding_table_backend
{
	static_assert(NumSrcBits <= 16, "NumSrcBits must be at most 16");
	static_assert(NumSrcBits >= 1, "NumSrcBits must be at least 1");
	static_assert(NumDstBits <= 16, "NumDstBits must be at most 16");
	static_assert(NumDstBits >= 1, "NumDstBits must be at least 1");

protected:
	typedef int32_t table_entry_t; // FIXME: determine the right (signed!) int type based on NumDstBits
	enum { num_table_entries = (1 << NumSrcBits) };
	table_entry_t table[num_table_entries];

public:
	enum
	{
		INVALID_ELEMENT = -1,
		IGNORE_ELEMENT = -2,
		IGNORE_TAIL_ELEMENT = -3,
	};

	enum { num_src_bits = NumSrcBits };
	enum { num_dst_bits = NumDstBits };

	inline table_entry_t operator[](size_t index) const
	{
		return table[index];
	}

	encoding_table_backend()
	{
		for (size_t i = 0; i < num_table_entries; ++i)
		{
			table[i] = INVALID_ELEMENT;
		}
	}

	encoding_table_backend(const encoding_table_backend&) = delete;
	void operator=(const encoding_table_backend&) = delete;
};

template <size_t NumSrcBits, size_t NumDstBits>
class encoding_table : public encoding_table_backend<NumSrcBits, NumDstBits> { };

template <size_t NumDstBits>
class encoding_table<8, NumDstBits> : public encoding_table_backend<8, NumDstBits>
{
private:
	typedef encoding_table_backend<8, NumDstBits> backend_t;

public:
	void from_string(const char* str)
	{
		int32_t i = 0;
		for (auto p = str; *p != '\0'; ++p, ++i)
		{
			backend_t::table[uint8_t(*p)] = i;
		}
	}

	void from_string(const std::string& str)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			backend_t::table[uint8_t(str[i])] = i;
		}
	}

	void ignore_tail_element(char c)
	{
		backend_t::table[uint8_t(c)] = backend_t::IGNORE_TAIL_ELEMENT;
	}

	void ignore_element(char c)
	{
		backend_t::table[uint8_t(c)] = backend_t::IGNORE_ELEMENT;
	}
};

template <size_t NumSrcBits>
class encoding_table<NumSrcBits, 8> : public encoding_table_backend<NumSrcBits, 8>
{
public:
	void to_string(const char* str)
	{
		size_t i = 0;
		for (auto p = str; *p != '\0'; ++p, ++i)
		{
			encoding_table_backend<NumSrcBits, 8>::table[i] = uint8_t(*p);
		}
	}
};

// FIXME: should be implemented using both <X,8> and <8,X> functions
template <>
class encoding_table<8, 8> : public encoding_table_backend<8, 8>
{
private:
	typedef encoding_table_backend<8, 8> backend_t;

public:
	void from_string(const char* str)
	{
		int32_t i = 0;
		for (auto p = str; *p != '\0'; ++p, ++i)
		{
			backend_t::table[uint8_t(*p)] = i;
		}
	}

	void from_string(const std::string& str)
	{
		for (size_t i = 0; i < str.size(); ++i)
		{
			backend_t::table[uint8_t(str[i])] = (int32_t)i;
		}
	}
};

template <class ConversionSpec>
std::string encoding_convert(const std::string& input, const ConversionSpec& conversion_spec);

template <size_t NumSrcBits, size_t NumDstBits>
std::string encoding_convert(const std::string& input, const encoding_table<NumSrcBits, NumDstBits>& translate_table)
{
	typedef encoding_table<NumSrcBits, NumDstBits> TranslateTable;
	const size_t char_num_bits = 8;
	size_t dst_size = (((input.size() * char_num_bits + (TranslateTable::num_src_bits - 1)) / TranslateTable::num_src_bits) * TranslateTable::num_dst_bits + (char_num_bits - 1)) / char_num_bits;

	std::string dst;
	dst.reserve(dst_size);

	uint32_t src_bits = 0;
	size_t src_bits_left = 0;
	uint32_t dst_bits = 0;
	size_t dst_bits_left = 0;

	bool in_tail = false;
	for (size_t i = 0; i < input.size(); )
	{
		while (i < input.size() && src_bits_left < 32 - char_num_bits)
		{
			src_bits <<= char_num_bits;
			src_bits_left += char_num_bits;
			src_bits |= uint8_t(input[i++]);
		}
		do
		{
			uint32_t index = src_bits >> (src_bits_left - TranslateTable::num_src_bits);
			src_bits &= (1 << (src_bits_left - TranslateTable::num_src_bits)) - 1;
			src_bits_left -= TranslateTable::num_src_bits;

			int32_t t = translate_table[index];
			if (in_tail)
			{ // FIXME: Add compile time check on conversion type whether we need to include this
				if (t != TranslateTable::IGNORE_TAIL_ELEMENT)
				{
					throw std::domain_error("Invalid element in convert: non-tail element in tail");
				}
			}
			else
			{
				if (t >= 0)
				{
					dst_bits <<= TranslateTable::num_dst_bits;
					dst_bits |= (uint32_t)t;
					dst_bits_left += TranslateTable::num_dst_bits;
					while (dst_bits_left >= char_num_bits)
					{
						uint32_t val = dst_bits >> (dst_bits_left - char_num_bits);
						dst_bits &= (1 << (dst_bits_left - char_num_bits)) - 1;
						dst_bits_left -= char_num_bits;
						dst.push_back(char(val));
					}
				}
				else if (t == TranslateTable::IGNORE_ELEMENT)
				{
					// Nothing to do
				}
				else if (t == TranslateTable::INVALID_ELEMENT)
				{
					throw std::domain_error("Invalid element in convert");
				}
				else if (t == TranslateTable::IGNORE_TAIL_ELEMENT)
				{
					in_tail = true;
				}
				else
				{
					throw std::domain_error("Unknown element translation in convert");
				}
			}
			// FIXME: Remainder of src_bits
		}
		while (src_bits_left >= TranslateTable::num_src_bits);
	}
	// FIXME remainder of dst_bits

	return dst;
}

const encoding_table<8, 6>& base64_decode();
const encoding_table<4, 8>& base16_encode();
const encoding_table<8, 4>& base16_decode();

// Base64-encoding function, accepts string and returns string,
std::string base64_encode(const std::string& bindata);

#endif // ENCODINGS_H
