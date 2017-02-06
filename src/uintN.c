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

bool
uintN_isequal (const uintN_t *a, const uintN_t *b)
{
  assert(a != NULL);
  assert(b != NULL);

  return memcmp (a->parts, b->parts, NUMBER_OF_BYTES) == 0;
}

static bool
uintN_isgreater_n (const uintN_t *a, const uintN_t *b, const bool eq)
{
  assert(a != NULL);
  assert(b != NULL);

  uint16_t i;
  uint64_t ai, bi;
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

bool
uintN_isgreatoreq (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreater_n (a, b, 1);
}

bool
uintN_isgreat (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreater_n (a, b, 0);
}

bool
uintN_isless (const uintN_t *a, const uintN_t *b)
{
  return uintN_isgreat (a, b) == 0;
}

bool
uintN_isodd (const uintN_t *bn)
{
  assert(bn != NULL);

  return bn->parts[0] & 0x01;
}

bool
uintN_iseven (const uintN_t *bn)
{
  return uintN_isodd (bn) == 0;
}

bool
uintN_iszero (const uintN_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->parts, ZERO.parts, NUMBER_OF_BYTES) == 0;
}

bool
uintN_isone (const uintN_t *bn)
{
  assert(bn != NULL);

  return memcmp (bn->parts, ONE.parts, NUMBER_OF_BYTES) == 0;
}

void
uintN_set (const uintN_t *bn, const uint64_t *c)
{
  assert(bn != NULL);
  assert(c != NULL);

  memcpy((void* ) bn->parts, c, NUMBER_OF_BYTES);
}

void
uintN_add (const uintN_t *a, const uintN_t *b, uintN_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uint16_t i;
  uint64_t sum, carry;

  for (i = 0, carry = 0; i < NUMBER_OF_PARTS; i++)
    {
      carry |= __builtin_uaddll_overflow (a->parts[i], carry, &sum);
      carry |= __builtin_uaddll_overflow (sum, b->parts[i], &c->parts[i]);
    }
}

void
uintN_inc (uintN_t *bn)
{
  uintN_add (bn, &ONE, bn);
}

void
uintN_sub (const uintN_t *a, const uintN_t *b, uintN_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uint16_t i;
  uint64_t sum, carry;

  for (i = 0, carry = 0; i < NUMBER_OF_PARTS; i++)
    {
      carry |= __builtin_usubll_overflow (a->parts[i], carry, &sum);
      carry |= __builtin_usubll_overflow (sum, b->parts[i], &c->parts[i]);
    }
}

void
uintN_dec (uintN_t *bn)
{
  uintN_sub (bn, &ONE, bn);
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

  // zeroize
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
uintN_gcd (const uintN_t *a, const uintN_t *b, uintN_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  if (uintN_iszero (a))
    uintN_set (c, b->parts);
  if (uintN_iszero (b))
    uintN_set (c, a->parts);
  if (uintN_isequal (a, c))
    return;
  else if (uintN_isequal (b, c))
    return;

  // SENSITIVE -> zeroize after use
  uintN_t _a;
  uintN_t _b;

  uintN_set (&_a, a->parts);
  uintN_set (&_b, b->parts);

  uint16_t i;
  for (i = 0; (uintN_iseven (&_a) && uintN_iseven (&_b)); ++i)
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

  uintN_lshift (&_a, i, c);

  // zeroize
  uintN_zeroize (&_a);
  uintN_zeroize (&_b);
}

// https://en.wikipedia.org/wiki/Division_algorithm
//if D = 0 then error(DivisionByZeroException) end
//Q := 0                 -- initialize quotient and remainder to zero
//R := 0
//for i := n − 1 ... 0 do     -- where n is number of bits in N
//  R := R << 1          -- left-shift R by 1 bit
//  R(0) := N(i)         -- set the least-significant bit of R equal to bit i of the numerator
//  if R ≥ D then
//    R := R − D
//    Q(i) := 1
//  end
//end
void
uintN_div (const uintN_t *a, const uintN_t *b, uintN_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uintN_zeroize (c);

  uint16_t i;
//  for (i = 0; i < NUMBER_OF_PARTS; i++)

}

void
uintN_mod (const uintN_t *a, const uintN_t *b, uintN_t *c)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(c != NULL);

  uint16_t i;
//  for (i = 0; i < NUMBER_OF_PARTS; i++)

}

void
uintp_rotl (uint64_t *a, uint8_t n, uint64_t *c)
{
  const uint8_t mask = PART_SIZE_BITS - 1;
  n &= mask;
  *c = (*a << n) | (*a >> ((-n) & mask));
}

void
uintp_rotr (uint64_t *a, uint8_t n, uint64_t *c)
{
  const uint8_t mask = PART_SIZE_BITS - 1;
  n &= mask;
  *c = (*a >> n) | (*a << ((-n) & mask));
}

void
uintN_pow (const uintN_t *x, const uintN_t *n, uintN_t *c)
{
  assert(x != NULL);
  assert(n != NULL);
  assert(c != NULL);

  uintN_set (c, ONE.parts);

  if (uintN_iszero (n))
    return;

  // SENSITIVE -> zeroize after use
  uintN_t _x;
  uintN_t _y;
  uintN_t _n;

  uintN_set (&_x, x->parts);
  uintN_set (&_y, ONE.parts);
  uintN_set (&_n, n->parts);

  do
    {
      if (uintN_isodd (n))
	{
	  uintN_mul (&_x, &_y, &_y);
	  uintN_mul (&_x, &_x, &_x);
	  uintN_dec (&_n);
	}
      else
	uintN_mul (&_x, &_x, &_x);
      uintN_rshift (&_n, 1, &_n);
    }
  while (!uintN_isone (&_n));

  uintN_mul (&_x, &_y, c);

  // zeroize
  uintN_zeroize (&_x);
  uintN_zeroize (&_y);
  uintN_zeroize (&_n);
}

// https://en.wikipedia.org/wiki/Fermat's_little_theorem
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

  uintN_set (dest, ONE.parts);
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

  // zeroize
  uintN_zeroize (&_base);
  uintN_zeroize (&_exp);
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
  uint64_t carry, mask;

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
uintN_rshift (const uintN_t *bn, uint16_t n, uintN_t *c)
{
  assert(bn != NULL);
  assert(c != NULL);
  assert(n < NUMBER_OF_BITS);

  if (n == 0)
    return;

  int16_t j;
  uint16_t i;
  uint8_t mod, residue, shift;
  uint64_t carry, mask;

  mod = n % PART_SIZE_BITS;
  residue = n - mod;
  shift = residue / PART_SIZE_BITS;
  mask = (1 << mod) - 1;

  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      j = i + shift;
      carry = (j < NUMBER_OF_PARTS - 1) ? bn->parts[j + 1] & mask : 0;
      c->parts[i] = (j < NUMBER_OF_PARTS) ? (bn->parts[j] >> mod) | carry : 0;
    }
}

void
uintN_zeroize (const uintN_t *bn)
{
  assert(bn != NULL);

  memset((void * ) bn->parts, 0, NUMBER_OF_BYTES);
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

  // zeroize
  uintN_zeroize (&_t);
}

void
print_array (const uint64_t *array, uint16_t size)
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

static uint64_t
endianize (uint64_t x)
{
  // swap adjacent 32-bit blocks
  x = (x >> 32) | (x << 32);

  // swap adjacent 16-bit blocks
  x = ((x & 0xFFFF0000FFFF0000) >> 16) | ((x & 0x0000FFFF0000FFFF) << 16);

  // swap adjacent 8-bit blocks
  return ((x & 0xFF00FF00FF00FF00) >> 8) | ((x & 0x00FF00FF00FF00FF) << 8);
}

void
uintN_readstr (const char *str, uintN_t *bn)
{
  assert(str != NULL);
  assert(bn != NULL);

  uint16_t i, length, step;
  uint64_t value;

  uintN_zeroize (bn);

  step = sizeof(value) * 2;

  // TODO left pad zeroes + modulo
  length = strlen (str) / step;
  if (length == 0)
    length = 1;

  for (i = 0; i < min(NUMBER_OF_BYTES, length); i++)
    if (sscanf (str, PRINT_FORMAT, &value) == 1)
      {
	bn->parts[i] = endianize (value);
	str += step;
      }
    else
      printf ("failed to parse '%s' as hexadecimal number\n", str);
}

