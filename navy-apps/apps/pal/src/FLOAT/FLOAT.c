#include "FLOAT.h"
#include <stdint.h>
#include <assert.h>

typedef union {
  struct {
    uint32_t m : 23;
    uint32_t e : 8;
    uint32_t s : 1;
  };
  uint32_t val;
} Float;

#define __sign(x) ((x) & 0x80000000)
#define __scale(x) (__sign(x) ? -(x) : (x))

FLOAT F_mul_F(FLOAT a, FLOAT b) {
  int64_t scale = ((int64_t)a * (int64_t)b) >> 16;
  return scale;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
  int64_t x = __scale(a) * 65536ll;
  int64_t y = __scale(b), ans = 0;

  for (int i = 31; i >= 0; i --) {
    int64_t t = y * (1ll << i);
    if (t <= x) {
      x -= t;
      ans += (1ll << i);
    }
  }

  ans *= ((__sign(a) ^ __sign(b)) ? -1 : 1);
  return ans;
}

FLOAT f2F(float a) {
  /* You should figure out how to convert `a' into FLOAT without
   * introducing x87 floating point instructions. Else you can
   * not run this code in NEMU before implementing x87 floating
   * point instructions, which is contrary to our expectation.
   *
   * Hint: The bit representation of `a' is already on the
   * stack. How do you retrieve it to another variable without
   * performing arithmetic operations on it directly?
   */

  Float f;
  void *temp = &a;
  f.val = *(uint32_t *)temp;
  uint32_t m = f.m | (1 << 23);
  int shift = 134 - (int)f.e;
//  assert(shift <= 23 && shift >= -7);
  if(shift < 0) {
    m <<= (-shift);
  }
  else {
    m >>= shift;
  }
  return (__sign(f.val) ? -m : m);
}

FLOAT Fabs(FLOAT a) {
  return __scale(a);
}

/* Functions below are already implemented */

FLOAT Fsqrt(FLOAT x) {
  FLOAT dt, t = int2F(2);

  do {
    dt = F_div_int((F_div_F(x, t) - t), 2);
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

FLOAT Fpow(FLOAT x, FLOAT y) {
  /* we only compute x^0.333 */
  FLOAT t2, dt, t = int2F(2);

  do {
    t2 = F_mul_F(t, t);
    dt = (F_div_F(x, t2) - t) / 3;
    t += dt;
  } while(Fabs(dt) > f2F(1e-4));

  return t;
}

