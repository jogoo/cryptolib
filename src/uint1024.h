/*
 * uint1024.h
 *
 * Header file for unsigned big integer of size 8192b/1024B.
 * Currently named uint1024_t but interchangable depending on requested size.
 *
 * Used for handling big integer operations by the cryptography library.
 *
 *  Created on: Jan 16, 2017
 *      Author: pyk
 */

#ifndef UINT1024_H_
#define UINT1024_H_

#include <stdint.h>
#include "util.h"

#ifdef __cplusplus
extern "C"
  {
#endif

#define MAX_SIZE_BITS 8192
#define MAX_SIZE_BYTES MAX_SIZE_BITS / 8

typedef _Bool uint1_t;

typedef struct
{
  uint8_t bytes[MAX_SIZE_BYTES];
} uint1024_t;

/**
 * uint1024 check if a > b.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uint1024_isgreater (const uint1024_t *a, const uint1024_t *b);

/**
 * uint1024 check if a < b.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uint1024_isless (const uint1024_t *a, const uint1024_t *b);

/**
 * uint1024 check if a == b.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uint1024_isequal (const uint1024_t *a, const uint1024_t *b);

/**
 * uint1024 check if even.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uint1024_iseven (const uint1024_t *bn);

/**
 * uint1024 check if odd.
 *
 * The running time of implemented algorithm is O(1).
 */
uint1_t
uint1024_isodd (const uint1024_t *bn);

/**
 * uint1024 set value.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_set (const uint1024_t *bn, const uint8_t *value);

/**
 * uint1024 addition c = a + b.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *c);

/**
 * uint1024 subtraction c = a - b.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_sub (const uint1024_t *a, const uint1024_t *b, uint1024_t *c);

/**
 * uint1024 multiplication c = a * b.
 *
 * The running time of implemented algorithm is O(n^2), where n is number of bits in a.
 */
void
uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *c);

/**
 * uint1024 modular exponentiation c ≡ b^exp (mod m).
 *
 * The running time of implemented algorithm is O(log exp).
 */
void
uint1024_modp (const uint1024_t *base, const uint1024_t *exp,
	       const uint1024_t *mod, uint1024_t *c);

/**
 * uint1024 logical left shift.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_lshift (const uint1024_t *bn, uint1024_t *dest, uint16_t n);

/**
 * uint1024 logical right shift.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_rshift (const uint1024_t *bn, uint1024_t *dest, uint16_t steps);

/**
 * uint1024 zeroize.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_zeroize (const uint1024_t *bn);

/**
 * uint1024 print.
 *
 * The running time of implemented algorithm is O(1).
 */
void
uint1024_print (uint1024_t *bn);

#ifdef __cplusplus
}
#endif

#endif /* UINT1024_H_ */
