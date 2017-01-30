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

#define NUMBER_OF_BITS 8192
typedef uint64_t uint_p;

#define PART_SIZE_BYTES sizeof(uint_p)
#define PART_SIZE_BITS (PART_SIZE_BYTES * 8)
#define NUMBER_OF_BYTES (NUMBER_OF_BITS / 8)
#define NUMBER_OF_PARTS (NUMBER_OF_BITS / (PART_SIZE_BITS))

#define PRINT_FORMAT "%016llx"

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

typedef struct
{
  uint_p parts[NUMBER_OF_PARTS];
} uintN_t;

typedef _Bool uint1_t;

/**
 * uintN check if a > b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
uint1_t
uintN_isgreat (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a >= b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
uint1_t
uintN_isgreatoreq (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a < b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
uint1_t
uintN_isless (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if a == b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
uint1_t
uintN_isequal (const uintN_t *a, const uintN_t *b);

/**
 * uintN check if even.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
uint1_t
uintN_iseven (const uintN_t *bn);

/**
 * uintN check if odd.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uintN_isodd (const uintN_t *bn);

/**
 * uintN check if zero.
 */
uint1_t
uintN_iszero (const uintN_t *bn);

/**
 * uintN check if one.
 */
uint1_t
uintN_isone (const uintN_t *bn);

/**
 * uintN set value.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_set (const uintN_t *bn, const uint_p *val);

/**
 * uintN addition c = a + b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_add (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN increment c = a + 1.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_inc (const uintN_t *a, uintN_t *c);

/**
 * uintN subtraction c = a - b.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_sub (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN decrement c = a - 1.
 *
 * The running time of implemented algorithm is O(n) where n is number of bytes is uintN.
 */
void
uintN_dec (const uintN_t *a, uintN_t *c);

/**
 * uintN multiplication c = a * b.
 *
 * The running time of implemented algorithm is O(n^2), where n is number of bits in a.
 */
void
uintN_mul (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN multiplication c = a * b.
 */
void
uintN_gcd (const uintN_t *a, const uintN_t *b, uintN_t *c);

/**
 * uintN modular c ≡ b (mod m).
 *
 *
 */
void
uintN_mod (const uintN_t *base, const uintN_t *mod, uintN_t *c);

/**
 * uintN modular exponentiation c ≡ b^exp (mod m).
 * the implementation use the right-to-left binary method.
 * this method drastically reduces the number of operations
 * to perform modular exponentiation, while keeping the same memory.
 *
 * based on Applied Cryptography, p. 244. by Bruce Schneier
 *
 * The running time of implemented algorithm is O(log exp).
 */
void
uintN_modp (const uintN_t *base, const uintN_t *exp, const uintN_t *mod,
	    uintN_t *c);

void
uintp_rotr (uint_p *a, uint8_t n, uint_p *c);

void
uintp_rotl (uint_p *a, uint8_t n, uint_p *c);

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
uintN_parse (const char *str, uintN_t *bn);

void
uintN_tostring (const uintN_t *bn);

#ifdef __cplusplus
}
#endif

#endif /* UINT1024_H_ */
