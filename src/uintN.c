#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <inttypes.h>

#include "uintN.h"

const static uintN_t ZERO =
  { 0 };
const static uintN_t ONE =
  { 1 };

uint1_t
uintN_isequal (const uintN_t *a, const uintN_t *b)
{
  assert(a != NULL);
  assert(b != NULL);

  return memcmp (a->parts, b->parts, NUMBER_OF_BYTES) == 0;
}

static uint1_t
uintN_isgreater_n (const uintN_t *a, const uintN_t *b, const uint1_t eq)
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
uintN_isgreatoreq (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreater_n (a, b, 1);
}

uint1_t
uintN_isgreat (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreater_n (a, b, 0);
}

uint1_t
uintN_isless (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreat (a, b) == 0;
}

uint1_t
uintN_isodd (const uintN_t *bn)
{
  assert(bn != NULL);

  return bn->parts[0] & 0x01;
}

uint1_t
uintN_iseven (const uintN_t *bn)
{
  return uintN_isodd (bn) == 0;
}

uint1_t
uintN_iszero (const uintN_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->parts, ZERO.parts, NUMBER_OF_BYTES) == 0;
}

uint1_t
uintN_isone (const uintN_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->parts, ONE.parts, NUMBER_OF_BYTES) == 0;
}

void
uintN_set (const uintN_t *bn, const uint_p *c)
{
  assert(bn != NULL);
  assert(c != NULL);

  memcpy(bn->parts, c, NUMBER_OF_BYTES);
}

void
uintN_add (const uintN_t *a, const uintN_t *b, uintN_t *c)
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
uintN_inc (const uintN_t *a, uintN_t *dest)
{
  uintN_add (a, &ONE, dest);
}

void
uintN_sub (const uintN_t *a, const uintN_t *b, uintN_t *c)
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
uintN_dec (const uintN_t *a, uintN_t *dest)
{
  uintN_sub (a, &ONE, dest);
}

static void
shift_and_add (const uintN_t *a, const uintN_t *b, uintN_t *dest)
{
  // assert not needed.

// SENSITIVE -> zeroize after use
  uintN_t _a;
  uintN_t _b;

  uintN_set (&_a, a->parts);
  uintN_set (&_b, b->parts);

  uintN_zeroize (dest);

  while (!uintN_iszero (&_a))
    {
      if (uintN_isodd (&_a))
	uintN_add (&_b, dest, dest);

      uintN_rshift (&_a, 1, &_a);
      uintN_lshift (&_b, 1, &_b);
    }

  uintN_zeroize (&_a);
  uintN_zeroize (&_b);
}

void
uintN_mul (const uintN_t *a, const uintN_t *b, uintN_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  shift_and_add (a, b, dest);
}

void
uintN_gcd (const uintN_t *a, const uintN_t *b, uintN_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  if (uintN_iszero (a) || uintN_isequal (a, b))
    {
      uintN_set (dest, b->parts);
      return;
    }
  if (uintN_iszero (b))
    {
      uintN_set (dest, a->parts);
      return;
    }

  // SENSITIVE -> zeroize after use
  uintN_t _a;
  uintN_t _b;

  uintN_set (&_a, a->parts);
  uintN_set (&_b, b->parts);

  uint16_t shift;
  for (shift = 0; (uintN_iseven (&_a) && uintN_iseven (&_b)); ++shift)
    {
      uintN_rshift (&_a, 1, &_a);
      uintN_rshift (&_b, 1, &_b);
    }

  while (!uintN_isodd (&_a))
    uintN_rshift (&_a, 1, &_a);

  do
    {
      while (!uintN_isodd (&_b))
	uintN_rshift (&_b, 1, &_b);

      if (uintN_isgreat (&_a, &_b))
	uintN_swap (&_a, &_b);
      uintN_sub (&_b, &_a, &_b);
    }
  while (!uintN_iszero (&_b));

  uintN_lshift (&_a, shift, dest);

  uintN_zeroize (&_a);
  uintN_zeroize (&_b);
}

void
uintN_mod (const uintN_t *a, const uintN_t *b, uintN_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  // SENSITIVE -> zeroize after use
  uintN_t _r;
  uintN_t _q;

  uintN_set (&_r, a->parts);
  uintN_zeroize (&_q);

  while (uintN_isgreatoreq (&_r, b))
    {
      uintN_sub (&_r, b, &_r);
      uintN_inc (&_q, &_q);
    }
  uintN_set (dest, _r.parts);

  uintN_zeroize (&_r);
  uintN_zeroize (&_q);
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
uintN_modp (const uintN_t *base, const uintN_t *exp, const uintN_t *mod,
	    uintN_t *dest)
{
  assert(base != NULL);
  assert(exp != NULL);
  assert(mod != NULL);
  assert(dest != NULL);

  uintN_zeroize (dest);

  if (uintN_isequal (mod, &ONE))
    return;

  // SENSITIVE -> zeroize after use
  uintN_t _base;
  uintN_t _exp;

  uintN_set (dest, &ONE.parts);
  uintN_set (&_exp, exp->parts);

  uintN_mod (base, mod, &_base);

  while (!uintN_iszero (&_exp))
    {
      if (uintN_isodd (&_exp))
	{
	  uintN_mul (dest, &_base, dest);
	  uintN_mod (dest, mod, dest);
	}
      uintN_rshift (&_exp, 1, &_exp);
      uintN_mul (&_base, &_base, &_base);
      uintN_mod (&_base, mod, &_base);
    }
}

void
uintN_lshift (const uintN_t *bn, uint16_t n, uintN_t *dest)
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
uintN_rshift (const uintN_t *bn, uint16_t n, uintN_t *dest)
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
uintN_zeroize (const uintN_t *bn)
{
  assert(bn != NULL);

  memset(bn->parts, 0, NUMBER_OF_BYTES);
}

void
uintN_swap (uintN_t *a, uintN_t *b)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(a != b);

  // SENSITIVE -> zeroize after use
  uintN_t _t;

  uintN_set (&_t, a->parts);
  uintN_set (a, b->parts);
  uintN_set (b, _t.parts);

  uintN_zeroize (&_t);
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
uintN_print (uintN_t *bn)
{
  assert(bn != NULL);

  print_array (bn->parts, NUMBER_OF_PARTS);
}

static uint_p
endianize (uint_p x)
{
  // swap adjacent 32-bit blocks
  x = (x >> 32) | (x << 32);
  // swap adjacent 16-bit blocks
  x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);
  // swap adjacent 8-bit blocks
  return ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
}

void
uintN_parse (const char *str, uintN_t *bn)
{
  assert(str != NULL);
  assert(bn != NULL);

  uint16_t i, length, step;
  uint_p value;

  step = sizeof(value) * 2;
  length = strlen (str) / step;

  for (i = 0; i < min(NUMBER_OF_BYTES, length); i++)
    if (sscanf (str, PRINT_FORMAT, &value) == 1)
      {
	bn->parts[i] = endianize (value);
	str += step;
      }
    else
      printf ("failed to parse '%s' as hexadecimal number\n", str);
}

