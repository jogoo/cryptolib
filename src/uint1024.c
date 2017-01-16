#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "uint1024.h"

const static uint1024_t ZERO =
  { 0 };
const static uint1024_t ONE =
  { 0x01 };

uint1_t
uint1024_isequal (const uint1024_t *a, const uint1024_t *b)
{
  assert(a != NULL);
  assert(b != NULL);

  return memcmp (a->bytes, b->bytes, MAX_SIZE_BYTES) == 0;
}

uint1_t
uint1024_iszero (const uint1024_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->bytes, ZERO.bytes, MAX_SIZE_BYTES) == 0;
}

uint1_t
uint1024_isgreater (const uint1024_t *a, const uint1024_t *b)
{
  assert(a != NULL);
  assert(b != NULL);

  uint8_t ai, bi;
  uint16_t i;
  for (i = MAX_SIZE_BYTES; i > 0;)
    {
      ai = a->bytes[--i];
      bi = b->bytes[i];

      if (ai == bi)
	continue;
      else if (ai > bi)
	return 1;
      else
	return 0;
    }

  return 0;
}

uint1_t
uint1024_isless (const uint1024_t *a, const uint1024_t *b)
{
  return !uint1024_isgreater (a, b);
}

static uint1_t
uint1024_isodd_n (const uint8_t *array)
{
  assert(array != NULL);

  return array[0] & 0x01;
}

uint1_t
uint1024_isodd (const uint1024_t *bn)
{
  return uint1024_isodd_n (bn->bytes);
}

uint1_t
uint1024_iseven (const uint1024_t *bn)
{
  return uint1024_isodd (bn) == 0;
}

void
uint1024_set (const uint1024_t *bn, const uint8_t *array)
{
  assert(bn != NULL);
  assert(array != NULL);

  memcpy(bn->bytes, array, MAX_SIZE_BYTES);
}

static void
uint1024_add_n (const uint8_t *a, const uint8_t *b, uint8_t *dest,
		uint16_t size)
{
  // assert not needed.

  uint16_t i, s, cy;
  for (i = 0, cy = 0; i < MAX_SIZE_BYTES; i++)
    {
      s = a[i] + b[i] + cy;
      cy = s >> 8;
      dest[i] = (uint8_t) s;
    }
}

void
uint1024_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  uint1024_add_n (a->bytes, b->bytes, dest->bytes, MAX_SIZE_BYTES);
}

static void
uint1024_sub_n (const uint8_t *a, const uint8_t *b, uint8_t *dest,
		uint16_t size)
{
  // assert not needed.

  uint16_t i;
  uint8_t by, cy;
  for (i = 0, cy = 0; i < size; i++)
    {
      by = b[i] + cy;
      cy = by < cy;
      cy += a[i] < by;
      dest[i] = a[i] - by;
    }
}

void
uint1024_sub (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  uint1024_sub_n (a->bytes, b->bytes, dest->bytes, MAX_SIZE_BYTES);
}

static void
shift_and_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  // assert not needed.

  uint1024_t temp1;
  uint1024_t temp2;

  uint1024_set (&temp1, a->bytes);
  uint1024_set (&temp2, b->bytes);
  uint1024_zeroize (dest);

  while (!uint1024_iszero (&temp1))
    {
      if (uint1024_isodd (&temp1))
	uint1024_add (&temp2, dest, dest);

      uint1024_rshift (&temp1, &temp1, 1);
      uint1024_lshift (&temp2, &temp2, 1);
    }
}

void
uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  shift_and_add (a, b, dest);
}

//  function modular_pow(base, exponent, modulus)
//      if modulus = 1 then return 0
//      Assert :: (modulus - 1) * (modulus - 1) does not overflow base
//      result := 1
//      base := base mod modulus
//      while exponent > 0
//          if (exponent mod 2 == 1):
//             result := (result * base) mod modulus
//          exponent := exponent >> 1
//          base := (base * base) mod modulus
//      return result
void
uint1024_modp (const uint1024_t *base, const uint1024_t *exp,
	       const uint1024_t *mod, uint1024_t *dest)
{
  assert(base != NULL);
  assert(exp != NULL);
  assert(mod != NULL);
  assert(dest != NULL);

  uint1024_zeroize (dest);

  if (uint1024_isequal (mod, &ONE))
    return;
}

void
uint1024_lshift (const uint1024_t *bn, uint1024_t *dest, uint16_t n)
{
  assert(bn != NULL);
  assert(dest != NULL);
  assert(n > 0);
  assert(n < MAX_SIZE_BITS);

  int16_t j;
  uint8_t d, t;
  uint16_t i, cy;

  d = n % 8;
  t = (n - d) / 8;
  for (i = MAX_SIZE_BYTES; i > 0;)
    {
      j = --i - t;
      cy = (j > 0) ? (bn->bytes[j - 1] << d) >> 8 : 0;
      dest->bytes[i] = (j >= 0) ? (bn->bytes[j] << d) | cy : 0;
    }
}

void
uint1024_rshift (const uint1024_t *bn, uint1024_t *dest, uint16_t n)
{
  assert(bn != NULL);
  assert(dest != NULL);
  assert(n > 0);
  assert(n < MAX_SIZE_BITS);

  int16_t j;
  uint8_t d, t, cy;
  uint16_t i;

  d = n % 8;
  t = (n - d) / 8;
  for (i = 0; i < MAX_SIZE_BYTES; i++)
    {
      j = i + t;
      cy = (j < MAX_SIZE_BYTES - 1) ? bn->bytes[j + 1] << (8 - d) : 0;
      dest->bytes[i] = (j < MAX_SIZE_BYTES) ? (bn->bytes[j] >> d) | cy : 0;
    }
}

void
uint1024_zeroize (const uint1024_t *bn)
{
  assert(bn != NULL);

  memset(bn->bytes, 0, MAX_SIZE_BYTES);
}

void
uint1024_print (uint1024_t *bn)
{
  assert(bn != NULL);

  print_array (bn->bytes, MAX_SIZE_BYTES);
}

