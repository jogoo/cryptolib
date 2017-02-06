#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "../src/uintN.h"

static void
test_add_simple ()
{
  uintN_t a =
    { 0 };
  uintN_t b =
    { 0 };

  uintN_t c, d;
  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a.parts[i] = i;
      b.parts[i] = i;

      uintN_add (&a, &b, &c);
      uintN_sub (&c, &b, &d);

      assert(uintN_isequal (&a, &d) == 1);
    }
}

static void
test_add_complex ()
{

  uintN_t a1;
  uintN_t c1;

  uintN_zeroize (&a1);
  uintN_zeroize (&c1);

  uintN_t b1 =
    { 0x02 };

  uintN_sub (&a1, &b1, &c1);
  uintN_add (&c1, &b1, &c1);

  assert(uintN_isequal (&a1, &c1) == 1);

  uintN_t a2 =
    { 0x0 };
  uintN_t b2 =
    { 0x02 };
  uintN_t c2;
  uintN_zeroize (&c2);

  uintN_sub (&a2, &b2, &c2);
  uintN_add (&c2, &b2, &c2);

  assert(uintN_isequal (&a2, &c2) == 1);
}

static void
test_rotr ()
{
  uint64_t a1 = 0x01;
  uint64_t b1;
  uint64_t c1 = 0x8000000000000000;

  uintp_rotr (&a1, 1, &b1);
  assert(b1 == c1);

  uint64_t a2 = 0x17;
  uint64_t b2;
  uint64_t c2 = 0xb800000000000000;

  uintp_rotr (&a2, 5, &b2);
  assert(b2 == c2);
}

static void
test_rotl ()
{
  uint64_t a1 = 0x8000000000000000;
  uint64_t b1;
  uint64_t c1 = 0x01;

  uintp_rotl (&a1, 1, &b1);
  assert(b1 == c1);

  uint64_t a2 = 0xb800000000000000;
  uint64_t b2;
  uint64_t c2 = 0x17;

  uintp_rotl (&a2, 5, &b2);
  assert(b2 == c2);
}

static void
test_lshift_simple ()
{
  uintN_t a =
    { 0xff };
  uintN_t b;

  uintN_t c1 =
    { 0x01fe };
  uintN_lshift (&a, 1, &b);
  assert(uintN_isequal (&b, &c1) == 1);

  uintN_t c2 =
    { 0x7f80 };
  uintN_lshift (&a, 7, &b);
  assert(uintN_isequal (&b, &c2) == 1);

  uintN_t c3 =
    { 0x00, 0xff };
  uintN_lshift (&a, PART_SIZE_BITS, &b);
  assert(uintN_isequal (&b, &c3) == 1);

  uintN_t c4 =
    { 0x00, 0x8000000000000000, 0xfe };
  uintN_lshift (&a, 2 * PART_SIZE_BITS - 1, &b);
  assert(uintN_isequal (&b, &c4) == 1);

//  uintN_t c5 =
//    { 0x00, 0x00, 0x00, 0x80, 0x7f };
//  uintN_lshift (&a, 31, &b);
//  assert(uintN_isequal (&b, &c5) == 1);

// 5F8EF09 * 2 = BF1DE12
  uintN_t a6 =
    { 0x05f8ef09 };
  uintN_t c6 =
    { 0x0bf1de12 };
  uintN_lshift (&a6, 1, &b);
  assert(uintN_isequal (&b, &c6) == 1);
}

static void
test_rshift_simple ()
{
  uintN_t a =
    { 0x0fff };
  uintN_t b;

  uintN_t c1 =
    { 0x07ff };
  uintN_rshift (&a, 1, &b);
  assert(uintN_isequal (&b, &c1) == 1);

  uintN_t a2 =
    { 0xEA535255 };
  uintN_t c2 =
    { 0x7529A9 };
  uintN_rshift (&a2, 9, &b);
  assert(uintN_isequal (&b, &c2) == 1);

  uintN_t a3;
  uintN_zeroize (&a3);
  a3.parts[NUMBER_OF_PARTS - 1] = 0x80;

  uintN_t c3;
  uintN_zeroize (&c3);
  c3.parts[NUMBER_OF_PARTS - 1] = 0x40;

  uintN_rshift (&a3, 1, &b);
  assert(uintN_isequal (&b, &c3) == 1);

  uintN_t a4;
  uintN_zeroize (&a4);

  a4.parts[0] = 0x01;
  uintN_t c4;
  uintN_zeroize (&c4);

  uintN_rshift (&a4, 1, &b);
  assert(uintN_isequal (&b, &c4) == 1);
}

static void
test_shift_complex ()
{

  uintN_t bn;
  uintN_t check;

  uintN_zeroize (&bn);
  uintN_zeroize (&check);

  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS - NUMBER_OF_PARTS / 8 - 1; i++)
    {
      bn.parts[i] = 0xff;

      if (i == 0)
	continue;

      uintN_lshift (&bn, i, &check);
      uintN_rshift (&check, i, &check);
      assert(uintN_isequal (&bn, &check) == 1);
    }
}

static void
test_greater ()
{
  uintN_t a1;
  uintN_t b1;

  uintN_zeroize (&a1);
  uintN_zeroize (&b1);

  uint16_t i;
  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a1.parts[i] = i;
      b1.parts[i] = i + 1;
      assert(uintN_isgreat (&a1, &b1) == 0);
    }

  uintN_t a2;
  uintN_t b2;

  uintN_zeroize (&a2);
  uintN_zeroize (&b2);

  for (i = 0; i < NUMBER_OF_PARTS; i++)
    {
      a2.parts[i] = i + 1;
      b2.parts[i] = i;
      assert(uintN_isgreat (&a2, &b2) == 1);
    }
}

static void
test_oddeven ()
{
  uintN_t a;

  uintN_zeroize (&a);

  uint16_t i;
  for (i = 0; i < 0xff; i++)
    {
      a.parts[0] = i % 2;
      if (i % 2 == 1)
	assert(uintN_isodd (&a) == 1);
      else
	assert(uintN_iseven (&a) == 1);
    }
}

static void
test_mul ()
{
  uintN_t a1 =
    { 0x0b };
  uintN_t b1 =
    { 0x03 };
  uintN_t check =
    { 0x21 };
  uintN_t c;

  uintN_zeroize (&c);
  uintN_mul (&a1, &b1, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_mul_2 ()
{
  uintN_t a1 =
    { 0x16c6 };
  uintN_t b1 =
    { 0x016d92d9 };
  uintN_t check =
    { 0x20855e39d6 };
  uintN_t c;

  uintN_zeroize (&c);
  uintN_mul (&a1, &b1, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_gcd ()
{
  uintN_t a =
    { 0x36 };
  uintN_t b =
    { 0x18 };
  uintN_t check =
    { 0x06 };

  uintN_t c;
  uintN_zeroize (&c);

  uintN_gcd (&a, &b, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_mod ()
{
  uintN_t a =
    { 0x0b };
  uintN_t b =
    { 0x02 };
  uintN_t check =
    { 0x01 };
  uintN_t c;
  uintN_zeroize (&c);

  uintN_mod (&a, &b, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_mod_2 ()
{
  uintN_t a = // 2DDE08C3B
	{ 0x02dde08c3b };
  uintN_t b = // 2BDF
	{ 0x2bdf };
  uintN_t check = // 129B
	{ 0x129b };
  uintN_t c;
  uintN_zeroize (&c);

  uintN_mod (&a, &b, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_pow ()
{
  uintN_t a = // 2DDE08C3B
	{ 0x02dde08c3b };
  uintN_t b = // 2BDF
	{ 0x2bdf };
  uintN_t check = // 129B
	{ 0x129b };
  uintN_t c;
  uintN_zeroize (&c);

  uintN_mod (&a, &b, &c);
  assert(uintN_isequal (&c, &check) == 1);
}

static void
test_modp ()
{
  char *n_str =
      "a63fb6b665165b254ed49b84bfdb1912d900eb55d302a649c55a5640533c4bc22ace842e2ff7d396ddca4ac226bcae5d390163c2b1599e81aa736a9fa0fad3ed006efd0666769988c99753c92882c4cefcd0586dd0c7fb01027225cbcdb6a5638dd414ee69b9db1a4ce3089349b8c83ce7e84da0e7073351100f64a738c999f11ccb6276d2f67bd199bbd31f2d5cdfe8155edd0e2733e8a324116ca535c622e788334e75911dd79e88da82655522e82ed42d5f4c7b78f0ee5ea6beb26fb718f7df1408da7d4051c24e4cb7e0f4ddcd6bf98039eacd92d02217b2ad8dcbab196c0799f79e352a487626f389cd180075d8a8d1a59161692675499c1c65e14f3fe5";
  char *d_str =
      "1635d1dfa93ea4dba59df2cef7e0ba07521574db48ef042f3bddf742edbbd2f53449d5cfe3d9ac9b6db30e6cc4c715565ffcc70aa62dee66ad52710eb56f7d2b9f10b4de0b8751b8bc11eb002758dd19381e4f8a1047ff4921be053da6947da100bc3235adcb4631cbced300f66ae8d976340b56f1367d8d1963ad13481b6ae4db90cfddd45f20148aefe1462271e484d9a8e5ece9b7dae558c5467d37869cf43e7ac7b10bd89825743b1c3ad10a25012dee4fadc23a5b277dd083e11bc40e40a035dffa2b44af2affafc7941448349a3ab1abc3cbcb584a900c8bffdfd5a077475dca2e0d52e7e70863f86e190eddfbb1837b2075db28d2c561b7957e95894b";
  char *EM_str =
      "ff3fb6b665165b254ed49b84bfdb1912d900eb55d302a649c55a5640533c4bc22ace842e2ff7d396ddca4ac226bcae5d390163c2b1599e81aa736a9fa0fad3ed006efd0666769988c99753c92882c4cefcd0586dd0c7fb01027225cbcdb6a5638dd414ee69b9db1a4ce3089349b8c83ce7e84da0e7073351100f64a738c999f11ccb6276d2f67bd199bbd31f2d5cdfe8155edd0e2733e8a324116ca535c622e788334e75911dd79e88da82655522e82ed42d5f4c7b78f0ee5ea6beb26fb718f7df1408da7d4051c24e4cb7e0f4ddcd6bf98039eacd92d02217b2ad8dcbab196c0799f79e352a487626f389cd180075d8a8d1a59161692675499c1c65e14f40bc";

  uintN_t n, d, mod, EM;

  uintN_zeroize (&mod);
  mod.parts[0] = 0x800;

  uintN_readstr (n_str, &n);
  uintN_readstr (d_str, &d);
  uintN_readstr (EM_str, &EM);

  uintN_t c;
  uintN_zeroize (&c);

  uintN_modp (&n, &d, &mod, &c);
  uintN_print (&c);

  assert(uintN_isequal (&c, &EM) == 1);
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

  test_pow ();

  test_mod ();
  test_mod_2 ();

  test_modp ();

  printf ("Testfall avklarade.");
}
