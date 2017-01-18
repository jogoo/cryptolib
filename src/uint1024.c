#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "uint1024.h"

const static uint1024_t ZERO =
  { 0 };
const static uint1024_t ONE =
  { 1 };

uint1_t
uint1024_isequal (const uint1024_t *a, const uint1024_t *b)
{
  assert(a != NULL);
  assert(b != NULL);

  return memcmp (a->bytes, b->bytes, MAX_SIZE_BYTES) == 0;
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
  return uint1024_isgreater (a, b) == 0;
}

uint1_t
uint1024_isodd (const uint1024_t *bn)
{
  assert(bn != NULL);

  return bn->bytes[0] & 0x01;
}

uint1_t
uint1024_iseven (const uint1024_t *bn)
{
  return uint1024_isodd (bn) == 0;
}

uint1_t
uint1024_iszero (const uint1024_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->bytes, ZERO.bytes, MAX_SIZE_BYTES) == 0;
}

uint1_t
uint1024_isone (const uint1024_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->bytes, ONE.bytes, MAX_SIZE_BYTES) == 0;
}

void
uint1024_set (const uint1024_t *bn, const uint8_t *val)
{
  assert(bn != NULL);
  assert(val != NULL);

  memcpy(bn->bytes, val, MAX_SIZE_BYTES);
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

  // SENSITIVE -> zeroize after use
  uint1024_t _a;
  uint1024_t _b;

  uint1024_set (&_a, a->bytes);
  uint1024_set (&_b, b->bytes);

  uint1024_zeroize (dest);

  while (!uint1024_iszero (&_a))
    {
      if (uint1024_isodd (&_a))
	uint1024_add (&_b, dest, dest);

      uint1024_rshift (&_a, 1, &_a);
      uint1024_lshift (&_b, 1, &_b);
    }

  uint1024_zeroize (&_a);
  uint1024_zeroize (&_b);
}

void
uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  shift_and_add (a, b, dest);
}

void
uint1024_gcd (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  if (uint1024_iszero (a) || uint1024_isequal (a, b))
    {
      uint1024_set (dest, b->bytes);
      return;
    }
  if (uint1024_iszero (b))
    {
      uint1024_set (dest, a->bytes);
      return;
    }

  // SENSITIVE -> zeroize after use
  uint1024_t _a;
  uint1024_t _b;

  uint1024_set (&_a, a->bytes);
  uint1024_set (&_b, b->bytes);

  uint16_t shift;
  for (shift = 0; (uint1024_iseven (&_a) && uint1024_iseven (&_b)); ++shift)
    {
      uint1024_rshift (&_a, 1, &_a);
      uint1024_rshift (&_b, 1, &_b);
    }

  while (!uint1024_isodd (&_a))
    uint1024_rshift (&_a, 1, &_a);

  do
    {
      while (!uint1024_isodd (&_b))
	uint1024_rshift (&_b, 1, &_b);

      if (uint1024_isgreater (&_a, &_b))
	uint1024_swap (&_a, &_b);
      uint1024_sub (&_b, &_a, &_b);
    }
  while (!uint1024_iszero (&_b));

  uint1024_lshift (&_a, shift, dest);

  uint1024_zeroize (&_a);
  uint1024_zeroize (&_b);
}

void
uint1024_mod (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  uint1024_print (a);
  uint1024_print (b);

  uint1024_gcd (a, b, dest);
  uint1024_print (dest);

  uint1024_mul (dest, b, dest);
  uint1024_print (dest);

  uint1024_sub (a, dest, dest);
  uint1024_print (dest);
}

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

  // SENSITIVE -> zeroize after use
  uint1024_t _base;
  uint1024_t _exp;

  uint1024_set (dest, &ONE.bytes);
  uint1024_set (&_exp, exp->bytes);

  uint1024_mod (base, mod, &_base);

  while (!uint1024_iszero (&_exp))
    {
      if (uint1024_isodd (&_exp))
	{
	  uint1024_mul (dest, &_base, dest);
	  uint1024_mod (dest, mod, dest);
	}
      uint1024_rshift (&_exp, 1, &_exp);
      uint1024_mul (&_base, &_base, &_base);
      uint1024_mod (&_base, mod, &_base);
    }
}

void
uint1024_lshift (const uint1024_t *bn, uint16_t n, uint1024_t *dest)
{
  assert(bn != NULL);
  assert(dest != NULL);
  assert(n < MAX_SIZE_BITS);

  if (n == 0)
    return;

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
uint1024_rshift (const uint1024_t *bn, uint16_t n, uint1024_t *dest)
{
  assert(bn != NULL);
  assert(dest != NULL);
  assert(n < MAX_SIZE_BITS);

  if (n == 0)
    return;

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
uint1024_swap (uint1024_t *a, uint1024_t *b)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(a != b);

  // SENSITIVE -> zeroize after use
  uint1024_t _t;

  uint1024_set (&_t, a->bytes);
  uint1024_set (a, b->bytes);
  uint1024_set (b, _t.bytes);

  uint1024_zeroize (&_t);
}

void
uint1024_print (uint1024_t *bn)
{
  assert(bn != NULL);

  print_array (bn->bytes, MAX_SIZE_BYTES);
}

