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

uint1_t
uint1024_isgreater (const uint1024_t *a, const uint1024_t *b);
uint1_t
uint1024_isless (const uint1024_t *a, const uint1024_t *b);
uint1_t
uint1024_isequal (const uint1024_t *a, const uint1024_t *b);
uint1_t
uint1024_iseven (const uint1024_t *bn);
uint1_t
uint1024_isodd (const uint1024_t *bn);

void
uint1024_set (const uint1024_t *bn, const uint8_t *array);
void
uint1024_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *r);
void
uint1024_sub (const uint1024_t *a, const uint1024_t *b, uint1024_t *r);
void
uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *r);
void
uint1024_lshift (const uint1024_t *bn, uint1024_t *dest, uint16_t n);
void
uint1024_rshift (const uint1024_t *bn, uint1024_t *dest, uint16_t steps);
void
uint1024_zeroize (const uint1024_t *bn);
void
uint1024_print (uint1024_t *bn);

#ifdef __cplusplus
}
#endif

#endif /* UINT1024_H_ */
