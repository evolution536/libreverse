#ifndef INCLUDED_LCG_H
#define INCLUDED_LCG_H

#include <cstdint>

// Linear Congruential Generator PRNG template class.
// A: LCG multiplier 'A'
// C: LCG constant 'C'
// RightShift: An optional number [0, 31] bits to shift the result to the right before applying the mask.
// Mask: An optional mask to select bits, should be 0xFFFFFFFF by default (use all bits).
template <uint32_t A, uint32_t C, uint32_t RightShift, uint32_t Mask>
class lcg
{
public:
	// Default constructor for LCG object, providing constants A and C.
	// LCG is defined by: X_n+1 = (A * X_n + C) mod m
	lcg(const uint32_t initial_seed)
	{
		this->x_prev = initial_seed;
		this->multiplicative_inverse = this->calculate_multiplicative_inverse();
	};

	~lcg() { };

	// Returns the current internal state.
	const uint32_t get_state() const { return this->x_prev; };

	// Calculates next random number in the sequence, and returns it modulo a given modulus.
	// Since some LCG implementations use only a selected subset of output bits, we also
	// use the 'Mask' template value to optionally select bits.
	const uint32_t next(const uint32_t modulus)
	{
		return this->next() % modulus;
	};

	// Calculates next random number in the sequence, and returns it modulo 2^32 (no modulus specified).
	const uint32_t next()
	{
		this->x_prev = this->x_prev * A + C;
		return ((this->x_prev >> RightShift) & Mask);
	};

	// Calculates the next 'n' random numbers in the sequence, and returns it modulo a given modulus.
	const uint32_t next_n(const uint32_t n, const uint32_t modulus)
	{
		return this->next_n(n) % modulus;
	};

	// Calculates the next 'n' random numbers in the sequence, and returns it modulo 2^32 (no modulus specified).
	// This function is based on the closed expression that represents the LCG sequence:
	// x_n = A^n * x_0 + (A^n - 1) / (A - 1) * C.
	// https://www.nayuki.io/page/fast-skipping-in-a-linear-congruential-generator
	const uint32_t next_n(const uint32_t n)
	{
		this->x_prev = this->unsigned_power_mod_32(A, n) * this->x_prev + this->multi_step_second_term(A, n, C);
		return ((this->x_prev >> RightShift) & Mask);
	};

	// Calculates the previous random value in the sequence, and returns it modulo a given modulus.
	const uint32_t prev(const uint32_t modulus)
	{
		return this->prev() % modulus;
	};

	// Calculates the previous random value in the sequence, and returns it modulo 2^32 (no modulus specified).
	const uint32_t prev()
	{
		this->x_prev -= C;
		this->x_prev *= this->multiplicative_inverse;
		return ((this->x_prev >> RightShift) & Mask);
	};

	// Calculates the previous 'n' random values in the sequence, and returns it modulo a given modulus.
	const uint32_t prev_n(const uint32_t n, const uint32_t modulus)
	{
		return this->prev_n(n) % modulus;
	};

	// Calculates the previous 'n' random values in the sequence, and returns it modulo 2^32 (no modulus specified).
	// This function is based on the same closed expression formula as the the next_n function. However, the parameters
	// A and C are replaced by A^(-1) and -(A^(-1) * C) respectively.
	const uint32_t prev_n(const uint32_t n)
	{
		this->x_prev = this->unsigned_power_mod_32(this->multiplicative_inverse, n) * this->x_prev
			+ this->multi_step_second_term(this->multiplicative_inverse, n, -(this->multiplicative_inverse * C));
		return ((this->x_prev >> RightShift) & Mask);
	};

	// Calculates the distance between the current LCG state, and a specified previous state.
	// This algorithm is based on the PCG (Permuted Congruential Generator) distance functionality
	// and has complexity O(log(n)). It works as long as the modulus is a power of 2 (for example 2^32).
	// https://math.stackexchange.com/questions/2008585/computing-the-distance-between-two-linear-congruential-generator-states
	// http://www.pcg-random.org
	// https://en.wikipedia.org/wiki/Permuted_congruential_generator
	const uint32_t distance(const uint32_t prev_state) const
	{
		uint32_t z = this->x_prev;
		uint32_t p = 1;
		uint32_t steps = 0;
		uint32_t a = A;
		uint32_t c = C;
		while (z != prev_state)
		{
			if ((z ^ prev_state) & p)
			{
				z = a * z + c;
				steps += p;
			}

			c *= a + 1;
			a *= a;
			p <<= 1;
		}

		return steps;
	};
private:
	uint32_t x_prev;
	uint32_t multiplicative_inverse;

	// Formula derived from the Newton-Raphson division algorithm.
	// We must find a function f(x) that has a zero at x = 1/b. The function
	// that works is f(x) = 1/x - b. Filling in this formula we get:
	// x_n+1 = x_n - f(x_n)/f'(x_n) = x_n * (2 - b * x_n).
	constexpr uint32_t f32(uint32_t x, uint32_t y) const
	{
		return  y * ( 2 - y * x );
	};

	// Recursion formula for the f32 function. This way the functionality can be constexpr'd.
	constexpr uint32_t rec_f32(uint32_t x, uint32_t y, int n) const
	{
		return (n > 0) ? rec_f32(x, f32(x, y), n-1) : f32(x, y);
	};

	// This function is used to compute the multiplicative inverse of A in modulo 2^32.
	// It is compile-time, as A is also compile-time. The calculation is based on the
	// Newton-Raphson division method, which can also be used to find the multiplicative
	// inverse in mod 2^n. By taking x_0 = A as first guess, we already have the first
	// 3 bits of the inverse (note that in mod 8, all odd numbers are their own inverse).
	// The number of bits of the inverse we get doubles per iteration of the formula. That
	// means that for mod 2^32, we need 4 iterations (2^6, 2^12, 2^24, 2^48). After the 4th
	// iteration, we have the multiplicative inverse for any mod m <= 48.
	// References:
	// https://arxiv.org/pdf/1209.6626v2.pdf
	// https://yurichev.com/blog/modulo/
	// https://lemire.me/blog/2017/09/18/computing-the-inverse-of-odd-integers/
	// https://en.wikipedia.org/wiki/Multiplicative_inverse#Algorithms
	// https://en.wikipedia.org/wiki/Division_algorithm#Newton–Raphson_division
	// The initial estimate can be any odd number [1], but for 32-bit inverses, the
	// initial estimate can be tailored to be 'correct' to 4 bits. See the book
	// 'Hacker's Delight', below figure 10-6 for more information.
	// https://doc.lagout.org/security/Hackers%20Delight.pdf
	// [1] https://arxiv.org/pdf/1303.0328.pdf
	constexpr uint32_t calculate_multiplicative_inverse() const
	{
		return rec_f32(A, A, 4);
	};

	// Computes the second term of the fast-forward/backward formula in the sum representation.
	// This way, we avoid rounding and truncation errors introduced by the integer arithmetic.
	// For reference: https://laws.lanl.gov/vhosts/mcnp.lanl.gov/pdf_files/anl-rn-arb-stride.pdf
	const uint32_t multi_step_second_term(const uint32_t base, const uint32_t n, const uint32_t mul) const
	{
		uint32_t term = base + 1;
		for (uint32_t i = 2; i < n; ++i)
		{
			term += this->unsigned_power_mod_32(base, i);
		}
		return term * mul;
	};

	// Computes the 'base' raised to the power 'power' in mod 2^32.
	// This power function is based on 'exponentiation by squaring'.
	// For powers greater than about 4 this is computationally more efficient
	// than naively multiplying the base with itself repeatedly.
	// https://en.wikipedia.org/wiki/Exponentiation_by_squaring
	const uint32_t unsigned_power_mod_32(uint32_t base, uint32_t power) const
	{
		uint32_t a_n = 1;
		while (power > 0)
		{
			// Multiply with base if power is odd.
			if (power & 1)
			{
				a_n *= base;
			}

			// The next power must be even.
			power >>= 1;
			base *= base;
		}
		return a_n;
	};
};

// The LCG implementation used by Visual C++.
typedef lcg<0x343FD, 0x269EC3, 16, 0x7FFF> VisualCppLcg;

#endif // INCLUDED_LCG_H
