#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>

#include "uint1024.h"

const static uint1024_t ZERO =
  { 0 };

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
uint1024_iseven_n (const uint8_t *array)
{
  assert(array != NULL);
  uint1_t iseven = array[0] & 0x01;
  printf("is even=%d\n", iseven);
  return iseven;
}

uint1_t
uint1024_iseven (const uint1024_t *bn)
{
  return uint1024_iseven_n (bn->bytes);
}

uint1_t
uint1024_isodd (const uint1024_t *bn)
{
  uint1_t isodd = uint1024_iseven (bn);
  printf("is odd=%d\n", isodd);
  return !isodd;
}

void
uint1024_set (const uint1024_t *bn, const uint8_t *array)
{
  assert(bn != NULL);
  assert(array != NULL);

  memcpy (bn->bytes, array, MAX_SIZE_BYTES);
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

//  procedure karatsuba(num1, num2)
//  if (num1 < 10) or (num2 < 10)
//    return num1*num2
//  /* calculates the size of the numbers */
//  m = max(size_base10(num1), size_base10(num2))
//  m2 = m/2
//  /* split the digit sequences about the middle */
//  high1, low1 = split_at(num1, m2)
//  high2, low2 = split_at(num2, m2)
//  /* 3 calls made to numbers approximately half the size */
//  z0 = karatsuba(low1,low2)
//  z1 = karatsuba((low1+high1),(low2+high2))
//  z2 = karatsuba(high1,high2)
//  return (z2*10^(2*m2))+((z1-z2-z0)*10^(m2))+(z0)
//static void
//karatsuba (uint8_t *a, uint8_t *b, uint16_t size)
//{
//  assert(a != NULL);
//  assert(b != NULL);
//  assert(size > 0);
//
//  if (size == 1)
//    {
//      uint16_t t = (uint16_t) a[0] * (uint16_t) b[0];
//      printf ("done:%x\n", t);
//      return t;
//    }
//
//  uint16_t d = size / 2;
//
//  uint8_t z0[d];
//  uint8_t z1[d];
//  uint8_t z2[d];
//
//  uint8_t _t1[d];
//  uint8_t _t2[d];
//
//  uint1024_add_n (&a[0], &a[d], &_t1, d);
//  uint1024_add_n (&b[0], &b[d], &_t2, d);
//
//  printf ("d:%d\n", d);
//  z0[0] = karatsuba (&a[0], &b[0], d); // low
//  z1[0] = karatsuba (&_t1[0], &_t2[0], d);
//  z2[0] = karatsuba (&a[d], &b[d], d); // high
//
//  printf ("\nz0\n");
//  print_array (&z0, d);
//  printf ("\nz1\n");
//  print_array (&z1, d);
//  printf ("\nz2\n");
//  print_array (&z2, d);
//
////	return (z2 * 2 ^ size) + ((z1 - z2 - z0) * 2 ^ d) + z0;
//}

//Decimal:             Binary:
//5830  23958233       1011011000110  1011011011001001011011001
//2915  47916466       0101101100011  10110110110010010110110010
//1457  95832932       0010110110001  101101101100100101101100100
//728  191665864       0001011011000  1011011011001001011011001000
//364  383331728       0000101101100  10110110110010010110110010000
//182  766663456       0000010110110  101101101100100101101100100000
//91  1533326912       0000001011011  1011011011001001011011001000000
//45  3066653824       0000000101101  10110110110010010110110010000000
//22  6133307648       0000000010110  101101101100100101101100100000000
//11 12266615296       0000000001011  1011011011001001011011001000000000
//5  24533230592       0000000000101  10110110110010010110110010000000000
//2  49066461184       0000000000010  101101101100100101101100100000000000
//1  98132922368       0000000000001  1011011011001001011011001000000000000
//  ————————————          1022143253354344244353353243222210110 (before carry)
//  139676498390          1000001000010101011110001110011101011
static void
shift_and_add (const uint8_t *abytes, const uint8_t *bbytes, uint8_t *dest)
{

  // assert not needed.

  uint1024_t temp1;
  uint1024_t temp2;

  uint1024_set (&temp1, abytes);
  uint1024_set (&temp2, bbytes);
  uint1024_zeroize (dest);

  uint1024_print(&temp1);

  if (uint1024_isodd (&temp1))
    uint1024_add (&temp2, dest, dest);

  print_array (dest, MAX_SIZE_BYTES);


  while (!uint1024_iszero (&temp1))
    {
      uint1024_rshift (&temp1, &temp1, 1);
      uint1024_lshift (&temp2, &temp2, 1);

      if (uint1024_isodd (&temp1))
	uint1024_add (&temp2, dest, dest);
      print_array (dest, MAX_SIZE_BYTES);
    }
}

/**
 * Multiply parameter a and b and save the result in parameter dest.
 */
void
uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *dest)
{
  assert(a != NULL);
  assert(b != NULL);
  assert(dest != NULL);

  shift_and_add (a->bytes, b->bytes, dest->bytes);
}

/**
 * Arithmetic left shift big number n steps.
 */
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

/**
 * Arithmetic right shift big number n steps.
 */
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

  memset (bn->bytes, 0, MAX_SIZE_BYTES);
}

void
uint1024_print (uint1024_t *bn)
{
  assert(bn != NULL);

  print_array (bn->bytes, MAX_SIZE_BYTES);
}

