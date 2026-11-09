#include <cstring>
#include <pe_util.h>

// Converts an RVA in the PE file to the actual offset within the binary.
uint32_t rva_to_file_offset(const uint8_t* binary, const uint32_t rva)
{
	// Get the header structures.
	const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);

	// Get the section in which the operand value is located.
	// We assume that the PE and NT signatures have already been checked!
	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(nt_headers);
	for (uint32_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i, ++pSection)
	{
		// Get the relevant section size.
		uint32_t section_size = pSection->VirtualSize;
		if (!section_size)
		{
			section_size = pSection->SizeOfRawData;
		}

		// Check if the virtual address resides in this section.
		if (rva >= pSection->VirtualAddress && rva < (pSection->VirtualAddress + section_size))
		{
			// We have found the correct section, so we return the converted offset.
			return rva - (pSection->VirtualAddress - pSection->PointerToRawData);
		}
	}

	// Failed to convert RVA to offset.
	return 0;
}

// Gets the base address of an image.
uint32_t get_image_base_address(const uint8_t* binary, const uint32_t size)
{
	// Check if the binary size is large enough for the PE fields to be checked.
	if (size >= IMAGE_SIZE_OF_HEADERS)
	{
	        // Check the MZ signature first.
	        const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	        if (dos_h->e_magic != IMAGE_DOS_SIGNATURE)
	        {
	                return 0;
	        }

	        // Get the NT headers and check the PE signature.
	        const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);
	        if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
	        {
	                return 0;
	        }

	        // Retrieve the image base address.
	        return nt_headers->OptionalHeader.ImageBase;
	}

	return 0;
}

// Gets a specified section from the packed executable headers.
IMAGE_SECTION_HEADER* get_section_from_image(const uint8_t* binary, const char* section, const uint32_t section_len)
{
	// Get the header structures.
	const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);

	// Get the section in which the operand value is located.
	// We assume that the PE and NT signatures have already been checked!
	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(nt_headers);
	for (uint32_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i, ++pSection)
	{
		// Check if the virtual address resides in this section.
		if (strncmp(section, pSection->Name, section_len) == 0)
		{
		        return pSection;
		}
	}

	// The section was not found.
	return NULL;
}

// Obtains a vector of section headers for every section in the PE binary.
void obtain_list_of_sections(const uint8_t* binary, std::vector<IMAGE_SECTION_HEADER>& sections)
{
	// Get the header structures.
	const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);

	// Get the section in which the operand value is located.
	// We assume that the PE and NT signatures have already been checked!
	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(nt_headers);
	for (uint32_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i, ++pSection)
	{
		sections.push_back(*pSection);
	}
}

// Obtains operands from instructions in a code section of a binary.
void obtain_operands_from_code(const uint8_t* start, const uint32_t size, std::set<uint32_t>& operands)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		const uint8_t opcode = start[i];

		// We want to look for a mov instruction having an immediate offset value.
		if ((opcode == 0xA1 || opcode == 0xA3 || opcode == 0x68 || opcode == 0x05) && i + 5 <= size)
		{
			const uint32_t imm = *(uint32_t*)&start[i + 1];
			operands.emplace(imm);
			i += 4;
		}
		else if (opcode == 0x89 || opcode == 0x8B || opcode == 0xFF)
		{
			// Check the MOD/RM byte.
			const uint8_t mod = (start[i + 1] & 0xC0) >> 6;
			const uint8_t rm = start[i + 1] & 0x7;
			if (mod == 0x0 && rm == 0x5 && i + 6 <= size)
			{
				const uint32_t imm = *(uint32_t*)&start[i + 2];
				operands.emplace(imm);
				i += 5;
			}
		}
		else if (opcode == 0x81 || opcode == 0xC7)
		{
			const uint8_t mod = (start[i + 1] & 0xC0) >> 6;
			const uint8_t rm = start[i + 1] & 0x7;
			if (mod == 0x1)
			{
				if (rm == 0x4 && i + 8 <= size)
				{
					const uint32_t imm = *(uint32_t*)&start[i + 4];
					operands.emplace(imm);
					i += 7;
				}
				else if (rm == 0x5 && i + 7 <= size)
				{
					const uint32_t imm = *(uint32_t*)&start[i + 3];
					operands.emplace(imm);
					i += 6;
				}
			}
			else if (mod == 0x2)
			{
				if (rm == 0x5 && i + 10 <= size)
				{
					// Take the last operand and store it (the imm32).
					const uint32_t imm = *(uint32_t*)&start[i + 6];
					operands.emplace(imm);
					i += 9;
				}
			}
			else if (mod == 0x0)
			{
				if (rm == 0x5 && i + 10 <= size)
				{
					// We have two operands in this case: mov m32, imm32.
					const uint32_t opnd1 = *(uint32_t*)&start[i + 2];
					const uint32_t opnd2 = *(uint32_t*)&start[i + 6];
					operands.emplace(opnd1);
					operands.emplace(opnd2);
					i += 9;
				}
			}
		}
	}
}

// Obtains operands from instructions in a code section of a binary and returns the instruction addresses too.
void obtain_operands_from_code_ex(const uint8_t* start, const uint32_t size, const uint32_t segment_base, std::vector<operand_ex>& operands)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		const uint8_t opcode = start[i];

		// We want to look for a mov instruction having an immediate offset value.
		if ((opcode == 0xA1 || opcode == 0xA3 || opcode == 0x68 || opcode == 0x05) && i + 5 <= size)
		{
			const uint32_t imm = *(uint32_t*)&start[i + 1];
			operands.push_back(operand_ex(segment_base + i, imm));
			i += 4;
		}
		else if (opcode == 0x89 || opcode == 0x8B || opcode == 0xFF)
		{
			// Check the MOD/RM byte.
			const uint8_t mod = (start[i + 1] & 0xC0) >> 6;
			const uint8_t rm = start[i + 1] & 0x7;
			if (mod == 0x0 && rm == 0x5 && i + 6 <= size)
			{
				const uint32_t imm = *(uint32_t*)&start[i + 2];
				operands.push_back(operand_ex(segment_base + i, imm));
				i += 5;
			}
		}
		else if (opcode == 0x81 || opcode == 0xC7)
		{
			const uint8_t mod = (start[i + 1] & 0xC0) >> 6;
			const uint8_t rm = start[i + 1] & 0x7;
			if (mod == 0x1)
			{
				if (rm == 0x4 && i + 8 <= size)
				{
					const uint32_t imm = *(uint32_t*)&start[i + 4];
					operands.push_back(operand_ex(segment_base + i, imm));
					i += 7;
				}
				else if (rm == 0x5 && i + 7 <= size)
				{
					const uint32_t imm = *(uint32_t*)&start[i + 3];
					operands.push_back(operand_ex(segment_base + i, imm));
					i += 6;
				}
			}
			else if (mod == 0x2)
			{
				if (rm == 0x5 && i + 10 <= size)
				{
					// Take the last operand and store it (the imm32).
					const uint32_t imm = *(uint32_t*)&start[i + 6];
					operands.push_back(operand_ex(segment_base + i, imm));
					i += 9;
				}
			}
			else if (mod == 0x0)
			{
				if (rm == 0x5 && i + 10 <= size)
				{
					// We have two operands in this case: mov m32, imm32.
					const uint32_t opnd1 = *(uint32_t*)&start[i + 2];
					const uint32_t opnd2 = *(uint32_t*)&start[i + 6];
					operands.push_back(operand_ex(segment_base + i, opnd1));
					operands.push_back(operand_ex(segment_base + i, opnd2));
					i += 9;
				}
			}
		}
	}
}

// Tries to find the next island of instructions accessing a number of sequential (4-byte aligned) operands in a given island size.
// Returns the index of the next island inside the 'operands' vector or -1 if no more islands can be found.
// If calling this function iteratively to scan whole operand list, do not forget to add 1 to the next start index to avoid infinite loop!
template <uint32_t Alignment>
bool find_operand_instruction_islands(const std::vector<operand_ex>& operands, const uint32_t max_island_size, const uint32_t opnd_count, std::vector<operand_island>& results)
{
	bool anything = false;

	// Create a set of all available offsets.
	std::set<uint32_t> available_offsets;
	for (const auto x : operands)
	{
		available_offsets.emplace(x.operand);
	}

	// Iterate the newly created set to form islands of consecutive offsets of at least 'opnd_count' in size.
	std::vector<std::set<uint32_t>> found_islands;
	{
		uint32_t size = 1;
		auto it = available_offsets.cbegin();
		auto prev_it = it;
		++it;
		std::set<uint32_t> next_island;
		next_island.emplace(*prev_it);
		for (; it != available_offsets.cend(); ++it, ++prev_it)
		{
			// Check whether this offset is consecutive to the previous one.
			if (*prev_it + Alignment == *it)
			{
				// Emplace the current offset in the next island set.
				next_island.emplace(*it);

				// We have found a consecutive offset, increment the counter and check whether the island is complete.
				if (++size == opnd_count)
				{
					found_islands.push_back(next_island);
					size = 1;
					next_island.clear();
					next_island.emplace(*it);
				}
			}
			else
			{
				size = 1;
				next_island.clear();
				next_island.emplace(*it);
			}
		}
	}

	// Check whether any possible islands have been found. If not, we do not need to enter the last phase at all.
	if (found_islands.empty()) return anything;

	// Obtain the addresses for the given islands.
	for (auto const& i : found_islands)
	{
		// Obtain lower and upper bounds of this island.
		const auto lower_bound = *i.cbegin();
		const auto upper_bound = *i.crbegin();

		// Iterate the operands list to find out what addresses belong to these bounds.
		std::set<uint32_t> island_addresses;
		for (uint32_t i = 0; i < operands.size(); ++i)
		{
			// Is this operand inside the bounds of the island values?
			if (operands[i].operand >= lower_bound && operands[i].operand <= upper_bound)
			{
				island_addresses.emplace(operands[i].offset);
			}
		}

		// Now that we have the addresses, we should look for at least one valid island (within the maximum size).
		uint32_t size = 1;
		auto it = island_addresses.cbegin();
		auto prev_it = it;
		++it;
		uint32_t island_size = 0;
		uint32_t island_start_addr = *prev_it;
		for (; it != island_addresses.cend(); ++it, ++prev_it)
		{
			island_size += *it - *prev_it;
			if (island_size > max_island_size)
			{
				size = 1;
				island_size = 0;
				island_start_addr = *it;
			}
			else
			{
				if (++size == opnd_count)
				{
					// We have found a valid island, add it to the results.
					results.push_back(operand_island(island_start_addr, island_size, i));
					anything = true;
					break;
				}
			}
		}
	}

	// Return whether any results were found.
	return anything;
}

// Template instantiations for common alignment types.
template bool find_operand_instruction_islands<sizeof(uint32_t)>(const std::vector<operand_ex>& operands, const uint32_t max_island_size, const uint32_t opnd_count, std::vector<operand_island>& results);

// Converts instruction operand values to file offsets.
uint32_t convert_operand_to_file_offset(const uint8_t* binary, const uint32_t base, const uint32_t operand)
{
	// Get the header structures.
	const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);

	// Get the section in which the operand value is located.
	// We assume that the PE and NT signatures have already been checked!
	IMAGE_SECTION_HEADER* pSection = IMAGE_FIRST_SECTION(nt_headers);
	uint32_t sec_ind = (uint32_t)-1;
	for (uint32_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i)
	{
		// Check if the virtual address resides in this section.
		if (operand >= base + pSection[i].VirtualAddress && operand < base + pSection[i].VirtualAddress + pSection[i].VirtualSize)
		{
		        sec_ind = i;
			break;
		}
	}

	// If the section is found, we can grab the raw offset of the section and calculate the final raw offset of the operand.
	if (sec_ind != (uint32_t)-1)
	{
		// va = base + sec_va + raw_off - sec_raw_off
		// raw_off = va - sec_va - base + sec_raw_off
		return operand - pSection[sec_ind].VirtualAddress - base + pSection[sec_ind].PointerToRawData;
	}

	// The section was not found, so we cannot calculate the raw offset.
	return 0;
}

// Tries to dereference a pointer in the binary as long as it points inside the binary.
// The 'out_operand' argument gets the last operand value before dereferencing.
uint32_t resolve_multi_level_pointer(const uint8_t* binary, const uint32_t base, const uint32_t operand, uint32_t* const out_operand)
{
	uint32_t x = operand;
	uint32_t in = operand;
	uint32_t y = 0;
	uint32_t z = 0;
	while (x)
	{
		z = x;
		x = convert_operand_to_file_offset(binary, base, in);
		if (x)
		{
			y = in;
			in = *(uint32_t*)(binary + x);
		}
	}

	// The last operand may have some offsets added. Return it as well.
	*out_operand = y;
	return z;
}

// Validates and adds an offset from operand to the offsets list.
void validate_offsets(const uint8_t* binary, const uint32_t image_base, const std::set<uint32_t>& input, std::set<uint32_t>& out_offsets)
{
	// Iterate over the input offsets.
	for (const uint32_t operand : input)
	{
		// Dereference the offset, and check whether it still points inside the binary.
		uint32_t last_operand;
		const uint32_t final_offset = resolve_multi_level_pointer(binary, image_base, operand, &last_operand);
		if (final_offset && last_operand)
		{
			// Add the final offset to the output list.
			if (final_offset % sizeof(uint32_t) == 0)
			{
				out_offsets.emplace(final_offset);
			}

			// Add offsets to the last dereferenced pointer to find possible new offets.
			// Also dereference the last operand, and try to find possible new offsets with offsets added.
			const uint32_t last_value = *(uint32_t*)(binary + final_offset);
			for (const uint32_t known_size : input)
			{
				uint32_t resolved = convert_operand_to_file_offset(binary, image_base, last_operand + known_size);
				if (resolved && resolved % sizeof(uint32_t) == 0)
				{
					out_offsets.emplace(resolved);
				}

				resolved = convert_operand_to_file_offset(binary, image_base, last_value + known_size);
				if (resolved && resolved % sizeof(uint32_t) == 0)
				{
					out_offsets.emplace(resolved);
				}
			}
		}
	}
}

// Does a linear scan in a binary for some byte pattern.
uint8_t* find_byte_pattern(uint8_t* const blob, const uint32_t blob_size, const uint8_t* const pattern, const uint32_t pattern_length, const char* mask)
{
        // Perform an unaligned search for the pattern.
        for (unsigned int i = 0; i < blob_size; ++i)
        {
                // Check byte-by-byte whether the pattern is there.
                unsigned int j = 0;
                for (; j < pattern_length; ++j)
                {
                        // Is this byte present?
                        if (mask[j] == 'x' && blob[i + j] != pattern[j])
                        {
                                // The pattern is not present in this position.
                                break;
                        }
                }

                // Check if the pattern was found over its full length.
                if (j == pattern_length)
                {
                        return blob + i;
                }
        }

        // Not found!
        return PATTERN_NOT_FOUND;
}

// Obtains pointer and size of PE resource specified by given ID.
// Returns the size of the obtained data buffer or zero if it failed to obtain the resource data.
uint32_t obtain_resource_size_and_data(const uint8_t* binary, const uint32_t size, const uint32_t resource_id, uint8_t** resource_ptr)
{
	// Check the MZ signature first.
	const IMAGE_DOS_HEADER* dos_h = (IMAGE_DOS_HEADER*)binary;
	if (dos_h->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return 0;
	}

	// Get the NT headers and check the PE signature.
	const IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)(binary + dos_h->e_lfanew);
	if (nt_headers->Signature != IMAGE_NT_SIGNATURE)
	{
		return 0;
	}

	// Check whether PE file has enough data directories.
	if (nt_headers->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_RESOURCE)
	{
		return 0;
	}

	// Get the resources header, as it contains .NET information.
	const IMAGE_DATA_DIRECTORY* rsrc_dir = &nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
	const uint32_t rsrc_va = rsrc_dir->VirtualAddress;
	const uint32_t rsrc_sz = rsrc_dir->Size;

	// Is the resources directory valid in this binary?
	if (rsrc_va && rsrc_sz >= sizeof(IMAGE_RESOURCE_DIRECTORY) && rsrc_sz < size)
	{
		// Get the resource directory information.
		const IMAGE_RESOURCE_DIRECTORY* rsrc_section = (IMAGE_RESOURCE_DIRECTORY*)(binary + rva_to_file_offset(binary, rsrc_va));

		// First iterate over the 1st level directory entries, they contain the type of the resource.
		const IMAGE_RESOURCE_DIRECTORY_ENTRY* rs_ptr = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((uint8_t*)rsrc_section + sizeof(IMAGE_RESOURCE_DIRECTORY));
		for (uint32_t i = 0; i < rsrc_section->NumberOfNamedEntries + rsrc_section->NumberOfIdEntries; ++i, ++rs_ptr)
		{
			// Check if the resource entry is for the given ID.
			if (rs_ptr->DataIsDirectory && rs_ptr->Id == resource_id)
			{
				// We found a resouce with the RC_DATA group id. Let's extract this resource.
				const IMAGE_RESOURCE_DIRECTORY* directory_ptr = (IMAGE_RESOURCE_DIRECTORY*)((uint8_t*)rsrc_section + rs_ptr->OffsetToDirectory);
				const IMAGE_RESOURCE_DIRECTORY_ENTRY* subdir = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((uint8_t*)directory_ptr + sizeof(IMAGE_RESOURCE_DIRECTORY));
				for (uint32_t j = 0; j < directory_ptr->NumberOfNamedEntries + directory_ptr->NumberOfIdEntries; ++j, ++directory_ptr)
				{
					// Is this still a directory?
					if (subdir->DataIsDirectory)
					{
						const IMAGE_RESOURCE_DIRECTORY* data_desc = (IMAGE_RESOURCE_DIRECTORY*)((uint8_t*)rsrc_section + subdir->OffsetToDirectory);
						const IMAGE_RESOURCE_DIRECTORY_ENTRY* data_entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((uint8_t*)data_desc + sizeof(IMAGE_RESOURCE_DIRECTORY));

						// Perform the final directory search.
						for (uint32_t k = 0; k < data_desc->NumberOfNamedEntries + data_desc->NumberOfIdEntries; ++k, ++data_entry)
						{
							// Check whether this is a data directory. It shouldn't be this way.
							if (!data_entry->DataIsDirectory)
							{
								// Compute file offset for resource data.
								const IMAGE_RESOURCE_DATA_ENTRY* final_data_desc = (IMAGE_RESOURCE_DATA_ENTRY*)((uint8_t*)rsrc_section + data_entry->OffsetToDirectory);
								const uint32_t rsrc_file_offset = rva_to_file_offset(binary, final_data_desc->OffsetToData);

								// Check if the size is sufficient to read at least the size parameter.
								if (final_data_desc->Size >= sizeof(uint32_t) && final_data_desc->Size < size && rsrc_file_offset < size)
								{
									const uint8_t* netwalker_res_data = binary + rsrc_file_offset;
									uint8_t* resource_data = new uint8_t[final_data_desc->Size];
									memcpy(resource_data, netwalker_res_data, final_data_desc->Size);
									*resource_ptr = resource_data;
									return final_data_desc->Size;
								}
							}
						}
					}
				}
			}
		}
	}

	// Resource not found or other error occurred!
	return 0;
}
