# Cryptolib
Cryptolib is a simple cryptography library in C.

## The library includes

###uint1024_t
A self implemented big integer module.
+ uint1_t uint1024_isgreater (const uint1024_t *a, const uint1024_t *b)
+ uint1_t uint1024_isless (const uint1024_t *a, const uint1024_t *b)
+ uint1_t uint1024_isequal (const uint1024_t *a, const uint1024_t *b)
+ uint1_t uint1024_iseven (const uint1024_t *bn)
+ uint1_t uint1024_isodd (const uint1024_t *bn)
+ void uint1024_set (const uint1024_t *bn, const uint8_t *val)
+ void uint1024_add (const uint1024_t *a, const uint1024_t *b, uint1024_t *c)
+ void uint1024_sub (const uint1024_t *a, const uint1024_t *b, uint1024_t *c)
+ void uint1024_mul (const uint1024_t *a, const uint1024_t *b, uint1024_t *c)
+ void uint1024_mod (const uint1024_t *base, const uint1024_t *mod, uint1024_t *c)
+ void uint1024_modp (const uint1024_t *base, const uint1024_t *exp, const uint1024_t *mod, uint1024_t *c)
+ void uint1024_lshift (const uint1024_t *bn, uint16_t n, uint1024_t *c)
+ void uint1024_rshift (const uint1024_t *bn, uint16_t n, uint1024_t *c)
+ void uint1024_zeroize (const uint1024_t *bn)
+ void uint1024_print (uint1024_t *bn)