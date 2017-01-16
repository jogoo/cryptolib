#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
  for (i = 0; i < MAX_SIZE_BYTES; i++)
    {
      a.bytes[i] = (uint8_t) i;
      b.bytes[i] = (uint8_t) i;

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
    { 0xff, 0xff, 0xff, 0xfe };
  uint1024_t b2 =
    { 0x02 };
  uint1024_t c2;

  uint1024_zeroize (&c2);

  uint1024_sub (&a2, &b2, &c2);
  uint1024_add (&c2, &b2, &c2);

  assert(uint1024_isequal (&a2, &c2) == 1);
}

static void
test_lshift_simple ()
{
  uint1024_t a =
    { 0xff };
  uint1024_t b;

  uint1024_t c1 =
    { 0xfe, 0x01 };
  uint1024_lshift (&a, &b, 1);
  assert(uint1024_isequal (&b, &c1) == 1);

  uint1024_t c2 =
    { 0x80, 0x7f };
  uint1024_lshift (&a, &b, 7);
  assert(uint1024_isequal (&b, &c2) == 1);

  uint1024_t c3 =
    { 0x00, 0xff };
  uint1024_lshift (&a, &b, 8);
  assert(uint1024_isequal (&b, &c3) == 1);

  uint1024_t c4 =
    { 0x00, 0x80, 0x7f };
  uint1024_lshift (&a, &b, 15);
  assert(uint1024_isequal (&b, &c4) == 1);

  uint1024_t c5 =
    { 0x00, 0x00, 0x00, 0x80, 0x7f };
  uint1024_lshift (&a, &b, 31);
  assert(uint1024_isequal (&b, &c5) == 1);

  // 5F8EF09 * 2 = BF1DE12
  uint1024_t a6 =
    { 0x09, 0xef, 0xf8, 0x05 };
  uint1024_t c6 =
    { 0x12, 0xde, 0xf1, 0x0b };
  uint1024_lshift (&a6, &b, 1);
  assert(uint1024_isequal (&b, &c6) == 1);
}

static void
test_rshift_simple ()
{
  uint1024_t a =
    { 0xff, 0x0f };
  uint1024_t b;

  uint1024_t c1 =
    { 0xff, 0x07 };
  uint1024_rshift (&a, &b, 1);
  assert(uint1024_isequal (&b, &c1) == 1);

  uint1024_t a2 =
    { 0x55, 0x52, 0x53, 0xEA };
  uint1024_t c2 =
    { 0xA9, 0x29, 0x75, 0x00, };
  uint1024_rshift (&a2, &b, 9);
  assert(uint1024_isequal (&b, &c2) == 1);

  uint1024_t a3;
  uint1024_zeroize (&a3);
  a3.bytes[MAX_SIZE_BYTES - 1] = 0x80;

  uint1024_t c3;
  uint1024_zeroize (&c3);
  c3.bytes[MAX_SIZE_BYTES - 1] = 0x40;

  uint1024_rshift (&a3, &b, 1);
  assert(uint1024_isequal (&b, &c3) == 1);

  uint1024_t a4;
  uint1024_zeroize (&a4);

  a4.bytes[0] = 0x01;
  uint1024_t c4;
  uint1024_zeroize (&c4);

  uint1024_rshift (&a4, &b, 1);
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
  for (i = 0; i < MAX_SIZE_BYTES - MAX_SIZE_BYTES / 8 - 1; i++)
    {
      bn.bytes[i] = 0xff;

      if (i == 0)
	continue;

      uint1024_lshift (&bn, &check, i);
      uint1024_rshift (&check, &check, i);
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
  for (i = 0; i < 0xff; i++)
    {
      a1.bytes[i] = i;
      b1.bytes[i] = i + 1;
      assert(uint1024_isgreater (&a1, &b1) == 0);
    }

  uint1024_t a2;
  uint1024_t b2;

  uint1024_zeroize (&a2);
  uint1024_zeroize (&b2);

  for (i = 0; i < 0xff; i++)
    {
      a2.bytes[i] = i + 1;
      b2.bytes[i] = i;
      assert(uint1024_isgreater (&a2, &b2) == 1);
    }
}

static void
test_mul ()
{
  uint1024_t a1 =
    { 0x0b };
  uint1024_t b1 =
    { 0x03 };
  uint1024_t c;

  uint1024_zeroize (&c);

  printf ("%x\n", (0x03 * 0x0b));

  uint1024_mul (&a1, &b1, &c);

  uint1024_print (&c);
}

void
test ()
{
  test_add_simple ();
  test_add_complex ();

  test_lshift_simple ();
  test_rshift_simple ();
  test_shift_complex ();

  test_greater ();

  test_mul ();

  printf ("Testfall avklarade.");
}
