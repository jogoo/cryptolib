#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "../src/uint1024.h"

static void
test_add_simple ()
{
  uint1024_t a =
    { 0 };
  uint1024_t b =
    { 0 };

  uint1024_t c, d;
  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a.parts[i] = i;
      b.parts[i] = i;

      uint1024_add (&a, &b, &c);
      uint1024_sub (&c, &b, &d);

      assert(uint1024_isequal (&a, &d) == 1);
    }
}

static void
test_add_complex ()
{

  uint1024_t a1;
  uint1024_t c1;

  uint1024_zeroize (&a1);
  uint1024_zeroize (&c1);

  uint1024_t b1 =
    { 0x02 };

  uint1024_sub (&a1, &b1, &c1);
  uint1024_add (&c1, &b1, &c1);

  assert(uint1024_isequal (&a1, &c1) == 1);

  uint1024_t a2 =
    { 0x0 };
  uint1024_t b2 =
    { 0x02 };
  uint1024_t c2;
  uint1024_zeroize (&c2);

  uint1024_sub (&a2, &b2, &c2);
  uint1024_add (&c2, &b2, &c2);

  assert(uint1024_isequal (&a2, &c2) == 1);
}

static void
test_rotr ()
{
  uint_p a1 = 0x01;
  uint_p b1;
  uint_p c1 = 0x8000000000000000;

  uintp_rotr (&a1, 1, &b1);
  assert(b1 == c1);

  uint_p a2 = 0x17;
  uint_p b2;
  uint_p c2 = 0xb800000000000000;

  uintp_rotr (&a2, 5, &b2);
  assert(b2 == c2);
}

static void
test_rotl ()
{
  uint_p a1 = 0x8000000000000000;
  uint_p b1;
  uint_p c1 = 0x01;

  uintp_rotl (&a1, 1, &b1);
  assert(b1 == c1);

  uint_p a2 = 0xb800000000000000;
  uint_p b2;
  uint_p c2 = 0x17;

  uintp_rotl (&a2, 5, &b2);
  assert(b2 == c2);
}

static void
test_lshift_simple ()
{
  uint1024_t a =
    { 0xff };
  uint1024_t b;

  uint1024_t c1 =
    { 0x01fe };
  uint1024_lshift (&a, 1, &b);
  assert(uint1024_isequal (&b, &c1) == 1);

  uint1024_t c2 =
    { 0x7f80 };
  uint1024_lshift (&a, 7, &b);
  assert(uint1024_isequal (&b, &c2) == 1);

  uint1024_t c3 =
    { 0x00, 0xff };
  uint1024_lshift (&a, PART_SIZE_BITS, &b);
  assert(uint1024_isequal (&b, &c3) == 1);

  uint1024_t c4 =
    { 0x00, 0x8000000000000000, 0xfe };
  uint1024_lshift (&a, 2 * PART_SIZE_BITS - 1, &b);
  assert(uint1024_isequal (&b, &c4) == 1);

//  uint1024_t c5 =
//    { 0x00, 0x00, 0x00, 0x80, 0x7f };
//  uint1024_lshift (&a, 31, &b);
//  assert(uint1024_isequal (&b, &c5) == 1);

// 5F8EF09 * 2 = BF1DE12
  uint1024_t a6 =
    { 0x05f8ef09 };
  uint1024_t c6 =
    { 0x0bf1de12 };
  uint1024_lshift (&a6, 1, &b);
  assert(uint1024_isequal (&b, &c6) == 1);
}

static void
test_rshift_simple ()
{
  uint1024_t a =
    { 0x0fff };
  uint1024_t b;

  uint1024_t c1 =
    { 0x07ff };
  uint1024_rshift (&a, 1, &b);
  assert(uint1024_isequal (&b, &c1) == 1);

  uint1024_t a2 =
    { 0xEA535255 };
  uint1024_t c2 =
    { 0x7529A9 };
  uint1024_rshift (&a2, 9, &b);
  assert(uint1024_isequal (&b, &c2) == 1);

  uint1024_t a3;
  uint1024_zeroize (&a3);
  a3.parts[NUMBER_OF_PARTS - 1] = 0x80;

  uint1024_t c3;
  uint1024_zeroize (&c3);
  c3.parts[NUMBER_OF_PARTS - 1] = 0x40;

  uint1024_rshift (&a3, 1, &b);
  assert(uint1024_isequal (&b, &c3) == 1);

  uint1024_t a4;
  uint1024_zeroize (&a4);

  a4.parts[0] = 0x01;
  uint1024_t c4;
  uint1024_zeroize (&c4);

  uint1024_rshift (&a4, 1, &b);
  assert(uint1024_isequal (&b, &c4) == 1);
}

static void
test_shift_complex ()
{

  uint1024_t bn;
  uint1024_t check;

  uint1024_zeroize (&bn);
  uint1024_zeroize (&check);

  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS - NUMBER_OF_PARTS / 8 - 1; i++)
    {
      bn.parts[i] = 0xff;

      if (i == 0)
	continue;

      uint1024_lshift (&bn, i, &check);
      uint1024_rshift (&check, i, &check);
      assert(uint1024_isequal (&bn, &check) == 1);
    }
}

static void
test_greater ()
{
  uint1024_t a1;
  uint1024_t b1;

  uint1024_zeroize (&a1);
  uint1024_zeroize (&b1);

  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a1.parts[i] = i;
      b1.parts[i] = i + 1;
      assert(uint1024_isgreat (&a1, &b1) == 0);
    }

  uint1024_t a2;
  uint1024_t b2;

  uint1024_zeroize (&a2);
  uint1024_zeroize (&b2);

  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a2.parts[i] = i + 1;
      b2.parts[i] = i;
      assert(uint1024_isgreat (&a2, &b2) == 1);
    }
}

static void
test_oddeven ()
{
  uint1024_t a;

  uint1024_zeroize (&a);

  uint16_t i;
  for (i = 0; i < 0xff; i++)
    {
      a.parts[0] = i % 2;
      if (i % 2 == 1)
	assert(uint1024_isodd (&a) == 1);
      else
	assert(uint1024_iseven (&a) == 1);
    }
}

static void
test_mul ()
{
  uint1024_t a1 =
    { 0x0b };
  uint1024_t b1 =
    { 0x03 };
  uint1024_t check =
    { 0x21 };
  uint1024_t c;

  uint1024_zeroize (&c);
  uint1024_mul (&a1, &b1, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

static void
test_mul_2 ()
{
  uint1024_t a1 =
    { 0x16c6 };
  uint1024_t b1 =
    { 0x016d92d9 };
  uint1024_t check =
    { 0x20855e39d6 };
  uint1024_t c;

  uint1024_zeroize (&c);
  uint1024_mul (&a1, &b1, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

static void
test_gcd ()
{
  uint1024_t a =
    { 0x36 };
  uint1024_t b =
    { 0x18 };
  uint1024_t check =
    { 0x06 };

  uint1024_t c;
  uint1024_zeroize (&c);

  uint1024_gcd (&a, &b, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

static void
test_mod ()
{
  uint1024_t a =
    { 0x0b };
  uint1024_t b =
    { 0x02 };
  uint1024_t check =
    { 0x01 };
  uint1024_t c;
  uint1024_zeroize (&c);

  uint1024_mod (&a, &b, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

static void
test_mod_2 ()
{
  uint1024_t a = // 2DDE08C3B
	{ 0x02dde08c3b };
  uint1024_t b = // 2BDF
	{ 0x2bdf };
  uint1024_t check = // 129B
	{ 0x129b };
  uint1024_t c;
  uint1024_zeroize (&c);

  uint1024_mod (&a, &b, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

static void
test_modp ()
{
  uint1024_t a =
	{ 0x05 };
  uint1024_t b =
	{ 0x37 };
  uint1024_t m =
	{ 0xDD };
  uint1024_t check =
	{ 0x70 };
  uint1024_t c;
  uint1024_zeroize (&c);

  uint1024_modp (&a, &b, &m, &c);
  assert(uint1024_isequal (&c, &check) == 1);
}

void
test ()
{
//  printf ("%d\n", NUMBER_OF_BITS);
//  printf ("%d\n", NUMBER_OF_PARTS);
//  printf ("%d\n", PART_SIZE_BYTES);
//  printf ("%d\n", PART_SIZE_BITS);

  test_add_simple ();
  test_add_complex ();
  test_rotr ();
  test_rotl ();

  test_lshift_simple ();
  test_rshift_simple ();
  test_shift_complex ();

  test_oddeven ();

  test_greater ();

  test_mul ();
  test_mul_2 ();

  test_gcd ();

  test_mod ();
  test_mod_2 ();

  test_modp ();

  printf ("Testfall avklarade.");
}
