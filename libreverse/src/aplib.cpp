#include "aplib.h"

// aPlib internal data structure.
struct APDSTATE
{
	const uint8_t* source;
	uint8_t* destination;
	uint32_t tag;
	uint32_t bitcount;

	// Default constructor.
	APDSTATE(const uint8_t* compressed, uint8_t* decompressed)
	{
		this->source = compressed;
		this->destination = decompressed;
		this->bitcount = 0;
		this->tag = 0;
	};
};

static uint32_t aplib_getbit(APDSTATE* const ud)
{
	// Check if tag is empty.
	if (!ud->bitcount--)
	{
		// Tag is empty, load next tag.
		ud->tag = *ud->source++;
		ud->bitcount = 7;
	}

	const uint32_t bit = (ud->tag >> 7) & 1;
	ud->tag <<= 1;
	return bit;
}

static uint32_t aplib_getgamma(APDSTATE* const ud)
{
	uint32_t result = 1;
	do
	{
		result = (result << 1) + aplib_getbit(ud);
	}
	while (aplib_getbit(ud));
	return result;
}

// Decompresses an aPlib-compressed block of data.
uint32_t aplib_decompress(const uint8_t* compressed, uint8_t* decompressed)
{
	uint32_t R0 = (uint32_t)-1;
	uint32_t LWM = 0;
	uint32_t len = 0;
	uint32_t offsets = 0;
	bool done = false;

	// Initialize aPlib state.
	APDSTATE ud(compressed, decompressed);
	*ud.destination++ = *ud.source++;

	// Main loop for decompression.
	while (!done)
	{
		if (aplib_getbit(&ud))
		{
			if (aplib_getbit(&ud))
			{
				if (aplib_getbit(&ud))
				{
					offsets = 0;
					for (uint32_t i = 4; i; i--)
					{
						offsets = (offsets << 1) + aplib_getbit(&ud);
					}

					if (offsets)
					{
						*ud.destination = *(ud.destination - offsets);
						ud.destination++;
					}
					else
					{
						*ud.destination++ = 0;
					}

					LWM = 0;
				}
				else
				{
					offsets = *ud.source++;
					len = 2 + (offsets & 1);
					offsets >>= 1;
					if (offsets)
					{
						for (; len; len--)
						{
							*ud.destination = *(ud.destination - offsets);
							ud.destination++;
						}
					}
					else
					{
						done = true;
					}

					R0 = offsets;
					LWM = 1;
				}
			}
			else
			{
				offsets = aplib_getgamma(&ud);
				if ((LWM == 0) && offsets == 2)
				{
					offsets = R0;
					len = aplib_getgamma(&ud);
					for (; len; len--)
					{
						*ud.destination = *(ud.destination - offsets);
						ud.destination++;
					}
				}
				else
				{
					if (LWM == 0)
					{
						offsets -= 3;
					}
					else
					{
						offsets -= 2;
					}
					offsets <<= 8;
					offsets += *ud.source++;
					len = aplib_getgamma(&ud);
					if (offsets >= 32000)
					{
						len++;
					}
					if (offsets >= 1280)
					{
						len++;
					}
					if (offsets < 128)
					{
						len += 2;
					}
					for (; len; len--)
					{
						*ud.destination = *(ud.destination - offsets);
						ud.destination++;
					}
					R0 = offsets;
				}

				LWM = 1;
			}
		}
		else
		{
			*ud.destination++ = *ud.source++;
			LWM = 0;
		}
	}

	return (uint32_t)(ud.destination - decompressed);
}
