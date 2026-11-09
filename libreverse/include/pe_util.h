#ifndef PE_UTIL_H
#define PE_UTIL_H

#include <set>
#include <vector>
#include "pe.h"

// Converts an RVA in the PE file to the actual offset within the binary.
uint32_t rva_to_file_offset(const uint8_t* binary, const uint32_t rva);

// Gets the base address of an image.
uint32_t get_image_base_address(const uint8_t* binary, const uint32_t size);

// Gets a specified section from the packed executable headers.
IMAGE_SECTION_HEADER* get_section_from_image(const uint8_t* binary, const char* section, const uint32_t section_len);

// Obtains a vector of section headers for every section in the PE binary.
void obtain_list_of_sections(const uint8_t* binary, std::vector<IMAGE_SECTION_HEADER>& sections);

// Obtains operands from instructions in a code section of a binary.
void obtain_operands_from_code(const uint8_t* start, const uint32_t size, std::set<uint32_t>& operands);

// Represents an entry of extended operand information.
struct operand_ex
{
	uint32_t offset;
	uint32_t operand;

	// Default constructor.
	operand_ex(const uint32_t off, const uint32_t opnd)
	{
		this->offset = off;
		this->operand = opnd;
	};
};

// Obtains operands from instructions in a code section of a binary and returns the instruction addresses too.
void obtain_operands_from_code_ex(const uint8_t* start, const uint32_t size, const uint32_t segment_base, std::vector<operand_ex>& operands);

// Represents an operand island.
struct operand_island
{
	// The start address of the island.
	uint32_t start_address;

	// Represents the size of the island in bytes.
	uint32_t island_size;

	// The set of consecutive operands that made this island.
	std::set<uint32_t> operands;

	// Default constructor.
	operand_island(const uint32_t addr, const uint32_t size, const std::set<uint32_t>& ops)
	{
		this->start_address = addr;
		this->island_size = size;
		this->operands = ops;
	};
};

// Tries to find the next island of instructions accessing a number of sequential (4-byte aligned) operands in a given island size.
// Returns the index of the next island inside the 'operands' vector or -1 if no more islands can be found.
template <uint32_t Alignment>
bool find_operand_instruction_islands(const std::vector<operand_ex>& operands, const uint32_t max_island_size, const uint32_t opnd_count, std::vector<operand_island>& results);

// Converts instruction operand values to file offsets.
uint32_t convert_operand_to_file_offset(const uint8_t* binary, const uint32_t base, const uint32_t operand);

// Tries to dereference a pointer in the binary as long as it points inside the binary.
// The 'out_operand' argument gets the last operand value before dereferencing.
uint32_t resolve_multi_level_pointer(const uint8_t* binary, const uint32_t base, const uint32_t operand, uint32_t* const out_operand);

// Validates and adds an offset from operand to the offsets list.
void validate_offsets(const uint8_t* binary, const uint32_t image_base, const std::set<uint32_t>& input, std::set<uint32_t>& out_offsets);

// Defines when a pattern is not found.
#define PATTERN_NOT_FOUND (uint8_t*)-1

// Does a linear scan in a binary for some byte pattern.
uint8_t* find_byte_pattern(uint8_t* const blob, const uint32_t blob_size, const uint8_t* const pattern, const uint32_t pattern_length, const char* mask);

// Obtains pointer and size of PE resource specified by given ID.
// Returns the size of the obtained data buffer or zero if it failed to obtain the resource data.
uint32_t obtain_resource_size_and_data(const uint8_t* binary, const uint32_t size, const uint32_t resource_id, uint8_t** resource_ptr);

#endif
