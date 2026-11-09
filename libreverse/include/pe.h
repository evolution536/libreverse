#ifndef PE_H
#define PE_H

#include <cstdint>
#include <cstddef>

#define IMAGE_SIZE_OF_HEADERS 0x400
#define IMAGE_DOS_SIGNATURE 0x5A4D
#define IMAGE_NT_SIGNATURE 0x4550
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

/* Directory Entries, indices into the DataDirectory array */

#define	IMAGE_DIRECTORY_ENTRY_EXPORT		0
#define	IMAGE_DIRECTORY_ENTRY_IMPORT		1
#define	IMAGE_DIRECTORY_ENTRY_RESOURCE		2
#define	IMAGE_DIRECTORY_ENTRY_EXCEPTION		3
#define	IMAGE_DIRECTORY_ENTRY_SECURITY		4
#define	IMAGE_DIRECTORY_ENTRY_BASERELOC		5
#define	IMAGE_DIRECTORY_ENTRY_DEBUG		6
#define	IMAGE_DIRECTORY_ENTRY_COPYRIGHT		7
#define	IMAGE_DIRECTORY_ENTRY_GLOBALPTR		8   /* (MIPS GP) */
#define	IMAGE_DIRECTORY_ENTRY_TLS		9
#define	IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	10
#define	IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	11
#define	IMAGE_DIRECTORY_ENTRY_IAT		12  /* Import Address Table */
#define	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	13
#define	IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR	14

// DOS header, first header of a PE file.
struct IMAGE_DOS_HEADER
{
	uint16_t  e_magic;      /* 00: MZ Header signature */
	uint16_t  e_cblp;       /* 02: Bytes on last page of file */
	uint16_t  e_cp;         /* 04: Pages in file */
	uint16_t  e_crlc;       /* 06: Relocations */
	uint16_t  e_cparhdr;    /* 08: Size of header in paragraphs */
	uint16_t  e_minalloc;   /* 0a: Minimum extra paragraphs needed */
	uint16_t  e_maxalloc;   /* 0c: Maximum extra paragraphs needed */
	uint16_t  e_ss;         /* 0e: Initial (relative) SS value */
	uint16_t  e_sp;         /* 10: Initial SP value */
	uint16_t  e_csum;       /* 12: Checksum */
	uint16_t  e_ip;         /* 14: Initial IP value */
	uint16_t  e_cs;         /* 16: Initial (relative) CS value */
	uint16_t  e_lfarlc;     /* 18: File address of relocation table */
	uint16_t  e_ovno;       /* 1a: Overlay number */
	uint16_t  e_res[4];     /* 1c: Reserved words */
	uint16_t  e_oemid;      /* 24: OEM identifier (for e_oeminfo) */
	uint16_t  e_oeminfo;    /* 26: OEM information; e_oemid specific */
	uint16_t  e_res2[10];   /* 28: Reserved words */
	uint32_t  e_lfanew;     /* 3c: Offset to extended header */
};

// PE file header in the NT headers.
struct IMAGE_FILE_HEADER
{
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	uint16_t Characteristics;
};

// Data directory struct.
struct IMAGE_DATA_DIRECTORY
{
	uint32_t VirtualAddress;
	uint32_t Size;
};

// Optional header in the NT headers.
struct IMAGE_OPTIONAL_HEADER
{
	uint16_t Magic;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	uint32_t BaseOfData;
	uint32_t ImageBase;
	uint32_t SectionAlignment;
	uint32_t FileAlignment;
	uint16_t MajorOperatingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSubsystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

// Image NT headers.
struct IMAGE_NT_HEADERS
{
	uint32_t Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
};

// Image section header.
struct IMAGE_SECTION_HEADER
{
	char Name[8];
	union
	{
		uint32_t PhysicalAddress;
		uint32_t VirtualSize;
	};
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLineNumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLineNumbers;
	uint32_t Characteristics;
};

// Retreives the first image section header from the Nt Header
#define IMAGE_FIRST_SECTION(NtHeader)((IMAGE_SECTION_HEADER*)((uintptr_t)NtHeader + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + NtHeader->FileHeader.SizeOfOptionalHeader))

// .NET metadata section header.
struct IMAGE_COR20_HEADER
{
	uint32_t cb;
	uint16_t  MajorRuntimeVersion;
	uint16_t  MinorRuntimeVersion;

	IMAGE_DATA_DIRECTORY MetaData;
	uint32_t Flags;
	union
	{
		uint32_t EntryPointToken;
		uint32_t EntryPointRVA;
	} DUMMYUNIONNAME;

	IMAGE_DATA_DIRECTORY Resources;
	IMAGE_DATA_DIRECTORY StrongNameSignature;
	IMAGE_DATA_DIRECTORY CodeManagerTable;
	IMAGE_DATA_DIRECTORY VTableFixups;
	IMAGE_DATA_DIRECTORY ExportAddressTableJumps;
	IMAGE_DATA_DIRECTORY ManagedNativeHeader;
};

// Resources section dexriptor.
typedef struct _IMAGE_RESOURCE_DIRECTORY
{
	uint32_t Characteristics;
	uint32_t TimeDateStamp;
	uint16_t MajorVersion;
	uint16_t MinorVersion;
	uint16_t NumberOfNamedEntries;
	uint16_t NumberOfIdEntries;
} IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

// Resource ID definitions.
#define RC_CURSOR	1
#define RC_BITMAP	2
#define RC_ICON		3
#define RC_MENU		4
#define RC_DIALOG	5
#define RC_STRING	6
#define RC_FONT_DIR	7
#define RC_FONT		8
#define RC_ACCELERATOR	9
#define RC_DATA		10
#define RC_MSG_TABLE	11
#define RC_VERSION	16
#define RC_DLG_INCLUDE	17
#define RC_PLUG_PLAY	19
#define RC_VXD		20
#define RC_ANIM_CURSOR	21
#define RC_ANIM_ICON	22
#define RC_HTML		23
#define RC_MANIFEST	24

// Represents a resource entry referenced by name.
typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY
{
	union
	{
		struct
		{
			uint32_t NameOffset:31;
			uint32_t NameIsString:1;
		};
		uint32_t Name;
		uint16_t Id;
	};

	union
	{
		uint32_t OffsetToData;
		struct
		{
			uint32_t OffsetToDirectory:31;
			uint32_t DataIsDirectory:1;
		};
	};
} IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;

// Represents a resource data entry.
typedef struct _IMAGE_RESOURCE_DATA_ENTRY
{
	uint32_t OffsetToData;
	uint32_t Size;
	uint32_t CodePage;
	uint32_t Reserved;
} IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;

#endif
