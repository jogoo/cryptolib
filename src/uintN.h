/*
 * uintN.h
 *
 * Header file for unsigned big integer of size 8192b/1024B.
 * Currently named uintN_t but interchangeable depending on requested size.
 *
 * Used for handling big integer operations by the cryptography library.
 *
 *  Created on: Jan 16, 2017
 *      Author: pyk
 */
#ifndef UINT1024_H_
#define UINT1024_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
  {
#endif

#define NUMBER_OF_BITS 2048

#define PART_SIZE_BYTES sizeof(uint64_t)
#define PART_SIZE_BITS (PART_SIZE_BYTES * 8)
#define NUMBER_OF_BYTES (NUMBER_OF_BITS / 8)
#define NUMBER_OF_PARTS (NUMBER_OF_BITS / (PART_SIZE_BITS))

#define PRINT_FORMAT "%016llx"

#define true 1u
#define false 0u

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

typedef struct
{
  uint64_t parts[NUMBER_OF_PARTS];
} uintN_t;

typedef _Bool bool;

/**
 * uintN check if a > b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
bool
uintN_isgreat (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a >= b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
bool
uintN_isgreatoreq (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a < b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
bool
uintN_isless (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a == b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
bool
uintN_isequal (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if even.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
bool
uintN_iseven (const uintN_t *bn);

/**
 * uintN check if odd.
 *
 * The running time of implemented algorithm is O(1).
 */
bool
uintN_isodd (const uintN_t *bn);

/**
 * uintN check if zero.
 */
bool
uintN_iszero (const uintN_t *bn);

/**
 * uintN check if one.
 */
bool
uintN_isone (const uintN_t *bn);

/**
 * uintN set value.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_set (const uintN_t *bn, const uint64_t *val);

/**
 * uintN addition c = a + b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_add (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN increment a++
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_inc (uintN_t *a);

/**
 * uintN subtraction c = a - b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_sub (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN decrement a--
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_dec (uintN_t *a);

/**
 * uintN multiplication c = a * b.
 * the implementation use the shift-and-add algorithm.
 *
 * TODO implement divide-and-conquer (https://en.wikipedia.org/wiki/Divide_and_conquer_algorithm)
 *
 * The running time of implemented algorithm is O(n^2), where n is number of bits in a.
 */
void
uintN_mul (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN greatest common divisor  c = gcd(a, b).
 * Stein's algorithm
 */
void
uintN_gcd (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN modular c ≡ b (mod m).
 * the implementation use the Stein's algorithm.
 * this method drastically reduces the number of operations
 * to perform modular exponentiation, while keeping the same memory.
 *
 * The running time of implemented algorithm is O(n^2).
 */
void
uintN_mod (const uintN_t *base, const uintN_t *mod, uintN_t *c);

/**
 * uintN power c = base ^ exp.
 * the implementation use exponentiation by squaring
 */
void
uintN_pow (const uintN_t *base, const uintN_t *exp, uintN_t *c);

/**
 * uintN modular exponentiation c ≡ b ^ exp (mod m).
 * the implementation use the right-to-left binary method.
 * this method drastically reduces the number of operations
 * to perform modular exponentiation, while keeping the same memory.
 * based on Applied Cryptography, p. 244. by Bruce Schneier.
 *
 * The running time of implemented algorithm is O(log exp).
 */
void
uintN_modp (const uintN_t *base, const uintN_t *exp, const uintN_t *mod,
	    uintN_t *c);

void
uintp_rotr (uint64_t *a, uint8_t n, uint64_t *c);

void
uintp_rotl (uint64_t *a, uint8_t n, uint64_t *c);

/**
 * uintN logical left shift.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_lshift (const uintN_t *bn, uint16_t n, uintN_t *c);

/**
 * uintN logical right shift.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_rshift (const uintN_t *bn, uint16_t n, uintN_t *c);

/**
 * uintN zeroize.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_zeroize (const uintN_t *bn);

/**
 * uintN swap
 */
void
uintN_swap (uintN_t *a, uintN_t *b);

/**
 * uintN print.
 */
void
uintN_print (uintN_t *bn);

void
uintN_readstr (const char *str, uintN_t *bn);

void
uintN_tostring (const uintN_t *bn);

#ifdef __cplusplus
}
#endif

#endif /* UINT1024_H_ */
