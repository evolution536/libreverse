#include <iostream>
#include "libreverse/include/math_opt.h"

// Computes invariant and postshift constants to optimize divisions out.
// Taken from Hacker's Delight book.
template <uint32_t Div>
invariant_compute_result get_invariant_reciprocal()
{
        int p = 31;
        invariant_compute_result result;
        result.addend = 0;             // Initialize "add" indicator.
        uint32_t nc = (-1 - (-Div)%Div); // Unsigned arithmetic here.
        uint32_t q1 = 0x80000000/nc;     // Init. q1 = 2**p/nc.
        uint32_t r1 = 0x80000000 - q1*nc;// Init. r1 = rem(2**p, nc).
        uint32_t q2 = 0x7FFFFFFF/Div;      // Init. q2 = (2**p - 1)/d.
        uint32_t r2 = 0x7FFFFFFF - q2*Div; // Init. r2 = rem(2**p - 1, d).
        uint32_t delta = 0;
        do
        {
                p = p + 1;
                if (r1 >= nc - r1)
                {
                        q1 = 2*q1 + 1;
                        r1 = 2*r1 - nc;
                }
                else
                {
                        q1 = 2*q1;
                        r1 = 2*r1;
                }
                if (r2 + 1>= Div - r2)
                {
                        if (q2 >= 0x7FFFFFFF) result.addend = 1;
                        q2 = 2*q2 + 1;
                        r2 = 2*r2 + 1 - Div;
                }
                else
                {
                        if (q2 >= 0x80000000) result.addend = 1;
                        q2 = 2*q2;
                        r2 = 2*r2 + 1;
                }
                delta = Div - 1 - r2;
        }
        while (p < 64 && (q1 < delta || (q1 == delta && r1 == 0)));

        // Create final result.
        result.multiplier = q2 + 1;
        result.shift = p - 32;
        return result;
}

int main(int argc, char** argv)
{
	const uint32_t x = 163291;
	const uint32_t d = 257;
	const invariant_compute_result result = get_invariant_reciprocal<d>();
	std::cout << "Multiplier: 0x" << std::hex << std::uppercase << result.multiplier << std::dec << ", shift: " << result.shift << ", addend: " << result.addend << std::endl;
	std::cout << x << " / " << d << " = " << (((x + result.addend) * result.multiplier) >> (32 + result.shift)) << std::endl;
}
