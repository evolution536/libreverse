#ifndef MATH_H
#define MATH_H

#include <cstdint>
#include <cstddef>

// Represents an invariant object which's parameters can be used to optimize an integer divide.
struct invariant_compute_result
{
	uint32_t multiplier;
	uint32_t addend;
	int32_t shift;
};

// Computes invariant and postshift constants to optimize divisions out.
// Taken from Hacker's Delight book.
invariant_compute_result get_invariant_reciprocal(const uint32_t Div);

// Usage of the optimized formula:
// const uint64_t divided = ((x + result.addend) * result.multiplier) >> (32 + result.shift);

#endif
