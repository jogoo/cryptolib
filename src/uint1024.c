#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>

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

  return memcmp (a->parts, b->parts, NUMBER_OF_BYTES) == 0;
}

static uint1_t
uint1024_isgreater_n (const uint1024_t *a, const uint1024_t *b,
		      const uint1_t eq)
{
  assert(a != NULL);
  assert(b != NULL);

  uint16_t i;
  uint_p ai, bi;
  for (i = NUMBER_OF_PARTS; i > 0;)
    {
      ai = a->parts[--i];
      bi = b->parts[i];

      if (ai == bi)
	continue;
      else if (ai > bi)
	return 1;
      else
	return 0;
    }
  return eq;
}

uint1_t
uint1024_isgreatoreq (const uint1024_t *a, const uint1024_t *b)
{
  return uint1024_isgreater_n (a, b, 1);
}

uint1_t
uint1024_isgreat (const uint1024_t *a, const uint1024_t *b)
{
  return uint1024_isgreater_n (a, b, 0);
}

uint1_t
uint1024_isless (const uint1024_t *a, const uint1024_t *b)
{
  return uint1024_isgreat (a, b) == 0;
}

uint1_t
uint1024_isodd (const uint1024_t *bn)
{
  assert(bn != NULL);

  return bn->parts[0] & 0x01;
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

  return memcmp (bn->parts, ZERO.parts, NUMBER_OF_BYTES) == 0;
}

uint1_t
uint1024_isone (const uint1024_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->parts, ONE.parts, NUMBER_OF_BYTES) == 0;
}

void
uint1024_set (const uint1024_t *bn, const uint_p *c)
{
  assert(bn != NULL);
  assert(c != NULL);

  memcpy(bn->parts, c, NUMBER_OF_BYTES);
}

void
uint1024_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uint16_t i;
  uint_p sum, carry;

  for (i = 0, carry = 0; i < NUMBER_OF_PARTS; i++)
    {
      carry |= __builtin_uaddll_overflow (a->parts[i], carry, &sum);
      carry |= __builtin_uaddll_overflow (sum, b->parts[i], &c->parts[i]);
    }
}

void
uint1024_inc (const uint1024_t *a, uint1024_t *dest)
{
  uint1024_add (a, &ONE, dest);
}

void
uint1024_sub (const uint1024_t *a, const uint1024_t *b, uint1024_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uint16_t i;
  uint_p sum, carry;

  for (i = 0, carry = 0; i < NUMBER_OF_PARTS; i++)
    {
      carry |= __builtin_usubll_overflow (a->parts[i], carry, &sum);
      carry |= __builtin_usubll_overflow (sum, b->parts[i], &c->parts[i]);
    }
}

void
uint1024_dec (const uint1024_t *a, uint1024_t *dest)
{
  uint1024_sub (a, &ONE, dest);
}

static void
shift_and_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  // assert not needed.

// SENSITIVE -> zeroize after use
  uint1024_t _a;
  uint1024_t _b;

  uint1024_set (&_a, a->parts);
  uint1024_set (&_b, b->parts);

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
      uint1024_set (dest, b->parts);
      return;
    }
  if (uint1024_iszero (b))
    {
      uint1024_set (dest, a->parts);
      return;
    }

  // SENSITIVE -> zeroize after use
  uint1024_t _a;
  uint1024_t _b;

  uint1024_set (&_a, a->parts);
  uint1024_set (&_b, b->parts);

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

      if (uint1024_isgreat (&_a, &_b))
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

  // SENSITIVE -> zeroize after use
  uint1024_t _r;
  uint1024_t _q;

  uint1024_set (&_r, a->parts);
  uint1024_zeroize (&_q);

  while (uint1024_isgreatoreq (&_r, b))
    {
      uint1024_sub (&_r, b, &_r);
      uint1024_inc (&_q, &_q);
    }
  uint1024_set (dest, _r.parts);

  uint1024_zeroize (&_r);
  uint1024_zeroize (&_q);
}

void
uintp_rotl (uint_p *a, uint8_t n, uint_p *c)
{
  const uint8_t mask = PART_SIZE_BITS - 1;
  n &= mask;
  *c = (*a << n) | (*a >> ((-n) & mask));
}

void
uintp_rotr (uint_p *a, uint8_t n, uint_p *c)
{
  const uint8_t mask = PART_SIZE_BITS - 1;
  n &= mask;
  *c = (*a >> n) | (*a << ((-n) & mask));
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

  uint1024_set (dest, &ONE.parts);
  uint1024_set (&_exp, exp->parts);

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
  assert(n < NUMBER_OF_BITS);

  if (n == 0)
    return;

  int16_t j;
  uint16_t i;
  uint8_t d, t, k;
  uint_p carry, mask;

  d = n % PART_SIZE_BITS;
  k = n - d;
  t = k / PART_SIZE_BITS;
  mask = (1 << d) - 1;
  uintp_rotr (&mask, d, &mask);

  for (i = NUMBER_OF_PARTS; i > 0;)
    {
      j = --i - t;
      carry = (j > 0) ? (bn->parts[j - 1] & mask) >> k : 0;
      dest->parts[i] = (j >= 0) ? (bn->parts[j] << d) | carry : 0;
    }
}

void
uint1024_rshift (const uint1024_t *bn, uint16_t n, uint1024_t *dest)
{
  assert(bn != NULL);
  assert(dest != NULL);
  assert(n < NUMBER_OF_BITS);

  if (n == 0)
    return;

  int16_t j;
  uint16_t i;
  uint8_t d, k, t;
  uint_p carry, mask;

  d = n % PART_SIZE_BITS;
  k = n - d;
  t = k / PART_SIZE_BITS;
  mask = (1 << d) - 1;

  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      j = i + t;
      carry = (j < NUMBER_OF_PARTS - 1) ? bn->parts[j + 1] & mask : 0;
      dest->parts[i] = (j < NUMBER_OF_PARTS) ? (bn->parts[j] >> d) | carry : 0;
    }
}

void
uint1024_zeroize (const uint1024_t *bn)
{
  assert(bn != NULL);

  memset(bn->parts, 0, NUMBER_OF_BYTES);
}

void
uint1024_swap (uint1024_t *a, uint1024_t *b)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(a != b);

  // SENSITIVE -> zeroize after use
  uint1024_t _t;

  uint1024_set (&_t, a->parts);
  uint1024_set (a, b->parts);
  uint1024_set (b, _t.parts);

  uint1024_zeroize (&_t);
}

void
print_array (uint_p *array, uint16_t size)
{
  uint16_t i;
  for (i = size; i > 0;)
    printf (PRINT_FORMAT, array[--i]);
  printf ("\n");
}

void
uint1024_print (uint1024_t *bn)
{
  assert(bn != NULL);

  print_array (bn->parts, NUMBER_OF_PARTS);
}

