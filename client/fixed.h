// fixed.h

#ifndef __FIXED_H__
#define __FIXED_H__

#include <errno.h>

/***************************************/
/************ Math routines ************/
/***************************************/

#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

typedef long fixed;

__INLINE__ fixed itofix(int x)
{ 
   return x << 16;
}


__INLINE__ int fixtoi(fixed x) 
{ 
   return (x >> 16) + ((x & 0x8000) >> 15);
}


__INLINE__ fixed ftofix(double x) 
{ 
   if (x > 32767.0) {
      errno = ERANGE;
      return 0x7FFFFFFF;
   }

   if (x < -32768.0) {
      errno = ERANGE;
      return -0x80000000; 
   }

   return (long)(x * 65536.0 + (x < 0 ? -0.5 : 0.5)); 
}


__INLINE__ double fixtof(fixed x) 
{ 
   return (double)x / 65536.0; 
}


fixed fsqrt(fixed x);
fixed fatan(fixed x);
fixed fatan2(fixed y, fixed x);

extern fixed _cos_tbl[];
extern fixed _tan_tbl[];
extern fixed _acos_tbl[];


__INLINE__ fixed fcos(fixed x)
{
   return _cos_tbl[((x + 0x4000) >> 15) & 0x1FF];
}


__INLINE__ fixed fsin(fixed x) 
{ 
   return _cos_tbl[((x - 0x400000 + 0x4000) >> 15) & 0x1FF];
}


__INLINE__ fixed ftan(fixed x) 
{ 
   return _tan_tbl[((x + 0x4000) >> 15) & 0xFF];
}


__INLINE__ fixed facos(fixed x)
{
   /* equivalent to if((x < -65536) || (x > 65536)) */
   if ((fixed)(x-0x80000000+65536) > (fixed)(-0x80000000+65536+65536)) {
      errno = EDOM;
      return 0;
   }

   return _acos_tbl[(x+65536+127)>>8];
}


__INLINE__ fixed fasin(fixed x) 
{ 
   /* equivalent to if((x < -65536) || (x > 65536)) */
   if ((fixed)(x-0x80000000+65536) > (fixed)(-0x80000000+65536+65536)) {
      errno = EDOM;
      return 0;
   }

   return 0x00400000 - _acos_tbl[(x+65536+127)>>8];
}


__INLINE__ fixed fadd(fixed x, fixed y)
{
#if defined __GNUC__ && defined __i386__
   /* for gcc on i386 */
   fixed result;

   asm (
      "  addl %2, %0 ; "                  /* do the addition */
      "  jno 0f ; "                       /* check for overflow */

      "  movl %3, %4 ; "                  /* on overflow, set errno */
      "  movl $0x7FFFFFFF, %0 ; "         /* and return MAXINT */
      "  cmpl $0, %2 ; " 
      "  jg 0f ; "
      "  negl %0 ; "

      " 0: "                              /* finished */

   : "=r" (result)                        /* result in a register */

   : "0" (x),                             /* x in the output register */
     "rm" (y),                            /* y can go in register or memory */
     "i" (ERANGE),
     "m" (errno)

   : "%cc", "memory"                      /* clobbers flags and errno */
   );

   return result;
#else
   /* for other compilers/platforms */
   fixed result = x + y;

   if (result >= 0) {
      if ((x < 0) && (y < 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFFL;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y > 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFFL;
      }
      else
	 return result;
   }
#endif
}


__INLINE__ fixed fsub(fixed x, fixed y)
{
#if defined __GNUC__ && defined __i386__
   /* for gcc on i386 */
   fixed result;

   asm (
      "  subl %2, %0 ; "                  /* do the subtraction */
      "  jno 0f ; "                       /* check for overflow */

      "  movl %3, %4 ; "                  /* on overflow, set errno */
      "  movl $0x7FFFFFFF, %0 ; "         /* and return MAXINT */
      "  cmpl $0, %2 ; " 
      "  jl 0f ; "
      "  negl %0 ; "

      " 0: "                              /* finished */

   : "=r" (result)                        /* result in a register */

   : "0" (x),                             /* x in the output register */
     "rm" (y),                            /* y can go in register or memory */
     "i" (ERANGE),
     "m" (errno)

   : "%cc", "memory"                      /* clobbers flags and errno */
   );

   return result;
#else
   /* for other compilers/platforms */
   fixed result = x - y;

   if (result >= 0) {
      if ((x < 0) && (y > 0)) {
	 errno = ERANGE;
	 return -0x7FFFFFFF;
      }
      else
	 return result;
   }
   else {
      if ((x > 0) && (y < 0)) {
	 errno = ERANGE;
	 return 0x7FFFFFFFL;
      }
      else
	 return result;
   }
#endif
}


__INLINE__ fixed fmul(fixed x, fixed y)
{
#if defined __GNUC__ && defined __i386__
   /* for gcc on i386 */
   fixed edx __attribute__ ((__unused__));
   fixed result;

   asm (
      "  movl %2, %0 ; "
      "  imull %3 ; "                     /* do the multiply */
      "  shrdl $16, %%edx, %0 ; "

      "  shrl $16, %%edx ; "              /* check for overflow */
      "  jz 0f ; "
      "  cmpw $0xFFFF, %%dx ; "
      "  je 0f ; "

      "  movl %4, %5 ; "                  /* on overflow, set errno */
      "  movl $0x7FFFFFFF, %0 ; "         /* and return MAXINT */
      "  cmpl $0, %2 ; " 
      "  jge 1f ; "
      "  negl %0 ; "
      " 1: "
      "  cmpl $0, %3 ; " 
      "  jge 0f ; "
      "  negl %0 ; "

      " 0: "                              /* finished */

   : "=&a" (result),                      /* the result has to go in eax */
     "=&d" (edx)                          /* reliably reserve edx */

   : "mr" (x),                            /* x and y can be regs or mem */
     "mr" (y),
     "i" (ERANGE),
     "m" (errno)

   : "%cc", "memory"                      /* clobbers flags and errno */
   );

   return result;
#else
   /* for other compilers/platforms */
   return ftofix (fixtof (x) * fixtof (y));
#endif
}


__INLINE__ fixed fdiv(fixed x, fixed y)
{
#if defined __GNUC__ && defined __i386__
   /* for gcc on i386 */
   fixed edx __attribute__ ((__unused__));
   fixed reg __attribute__ ((__unused__));
   fixed result;

   asm (
      "  testl %0, %0 ; "                 /* test sign of x */
      "  js 3f ; "

      "  testl %2, %2 ; "                 /* test sign of y */
      "  jns 4f ; "
      "  negl %2 ; "

      " 0: "                              /* result will be negative */
      "  movl %0, %%edx ; "               /* check the range is ok */
      "  shrl $16, %%edx ; "
      "  shll $16, %0 ; "
      "  cmpl %2, %%edx ; "
      "  jae 1f ; "

      "  divl %2 ; "                      /* do the divide */
      "  testl %0, %0 ; "
      "  jns 2f ; "

      " 1: "
      "  movl %5, %6 ; "                  /* on overflow, set errno */
      "  movl $0x7FFFFFFF, %0 ; "         /* and return MAXINT */

      " 2: "
      "  negl %0 ; "                      /* fix up the sign of the result */
      "  jmp 6f ; "

      "  .align 4, 0x90 ; "

      " 3: ; "                            /* x is negative */
      "  negl %0 ; "
      "  testl %2, %2 ; "                 /* test sign of y */
      "  jns 0b ; "
      "  negl %2 ; "

      " 4: "                              /* result will be positive */
      "  movl %0, %%edx ; "               /* check the range is ok */
      "  shrl $16, %%edx ; "
      "  shll $16, %0 ; "
      "  cmpl %2, %%edx ; "
      "  jae 5f ; "

      "  divl %2 ; "                      /* do the divide */
      "  testl %0, %0 ; "
      "  jns 6f ; "

      " 5: "
      "  movl %5, %6 ; "                  /* on overflow, set errno */
      "  movl $0x7FFFFFFF, %0 ; "         /* and return MAXINT */

      " 6: "                              /* finished */

   : "=a" (result),                       /* the result has to go in eax */
     "=&d" (edx),                         /* reliably reserve edx */
     "=r" (reg)                           /* input operand will be clobbered */

   : "0" (x),                             /* x in eax */
     "2" (y),                             /* y in register */
     "i" (ERANGE),
     "m" (errno)

   : "%cc", "memory"                      /* clobbers flags and memory  */
   );

   return result;
#else
   /* for other compilers/platforms */
   if (y == 0) {
      errno == ERANGE;
      return (x < 0) ? -0x7FFFFFFFL : 0x7FFFFFFFL;
   }
   else
      return ftofix (fixtof (x) / fixtof (y));
#endif
}

class fix      /* C++ wrapper for the fixed point routines */
{
public:
   fixed v;

   fix()                                     { }
   fix(const fix &x)                         { v = x.v; }
   fix(const int x)                          { v = itofix(x); }
   fix(const long x)                         { v = itofix(x); }
   fix(const unsigned int x)                 { v = itofix(x); }
   fix(const unsigned long x)                { v = itofix(x); }
   fix(const float x)                        { v = ftofix(x); }
   fix(const double x)                       { v = ftofix(x); }

   operator int() const                      { return fixtoi(v); }
   operator long() const                     { return fixtoi(v); }
   operator unsigned int() const             { return fixtoi(v); }
   operator unsigned long() const            { return fixtoi(v); }
   operator float() const                    { return fixtof(v); }
   operator double() const                   { return fixtof(v); }

   fix& operator = (const fix &x)            { v = x.v;           return *this; }
   fix& operator = (const int x)             { v = itofix(x);     return *this; }
   fix& operator = (const long x)            { v = itofix(x);     return *this; }
   fix& operator = (const unsigned int x)    { v = itofix(x);     return *this; }
   fix& operator = (const unsigned long x)   { v = itofix(x);     return *this; }
   fix& operator = (const float x)           { v = ftofix(x);     return *this; }
   fix& operator = (const double x)          { v = ftofix(x);     return *this; }

   fix& operator +=  (const fix x)           { v += x.v;          return *this; }
   fix& operator +=  (const int x)           { v += itofix(x);    return *this; }
   fix& operator +=  (const long x)          { v += itofix(x);    return *this; }
   fix& operator +=  (const float x)         { v += ftofix(x);    return *this; }
   fix& operator +=  (const double x)        { v += ftofix(x);    return *this; }

   fix& operator -=  (const fix x)           { v -= x.v;          return *this; }
   fix& operator -=  (const int x)           { v -= itofix(x);    return *this; }
   fix& operator -=  (const long x)          { v -= itofix(x);    return *this; }
   fix& operator -=  (const float x)         { v -= ftofix(x);    return *this; }
   fix& operator -=  (const double x)        { v -= ftofix(x);    return *this; }

   fix& operator *=  (const fix x)           { v = fmul(v, x.v);           return *this; }
   fix& operator *=  (const int x)           { v *= x;                     return *this; }
   fix& operator *=  (const long x)          { v *= x;                     return *this; }
   fix& operator *=  (const float x)         { v = ftofix(fixtof(v) * x);  return *this; }
   fix& operator *=  (const double x)        { v = ftofix(fixtof(v) * x);  return *this; }

   fix& operator /=  (const fix x)           { v = fdiv(v, x.v);           return *this; }
   fix& operator /=  (const int x)           { v /= x;                     return *this; }
   fix& operator /=  (const long x)          { v /= x;                     return *this; }
   fix& operator /=  (const float x)         { v = ftofix(fixtof(v) / x);  return *this; }
   fix& operator /=  (const double x)        { v = ftofix(fixtof(v) / x);  return *this; }

   fix& operator <<= (const int x)           { v <<= x;           return *this; }
   fix& operator >>= (const int x)           { v >>= x;           return *this; }

   fix& operator ++ ()                       { v += itofix(1);    return *this; }
   fix& operator -- ()                       { v -= itofix(1);    return *this; }

   fix operator ++ (int)                     { fix t;  t.v = v;   v += itofix(1);  return t; }
   fix operator -- (int)                     { fix t;  t.v = v;   v -= itofix(1);  return t; }

   fix operator - () const                   { fix t;  t.v = -v;  return t; }

   inline friend fix operator +  (const fix x, const fix y)    { fix t;  t.v = x.v + y.v;        return t; }
   inline friend fix operator +  (const fix x, const int y)    { fix t;  t.v = x.v + itofix(y);  return t; }
   inline friend fix operator +  (const int x, const fix y)    { fix t;  t.v = itofix(x) + y.v;  return t; }
   inline friend fix operator +  (const fix x, const long y)   { fix t;  t.v = x.v + itofix(y);  return t; }
   inline friend fix operator +  (const long x, const fix y)   { fix t;  t.v = itofix(x) + y.v;  return t; }
   inline friend fix operator +  (const fix x, const float y)  { fix t;  t.v = x.v + ftofix(y);  return t; }
   inline friend fix operator +  (const float x, const fix y)  { fix t;  t.v = ftofix(x) + y.v;  return t; }
   inline friend fix operator +  (const fix x, const double y) { fix t;  t.v = x.v + ftofix(y);  return t; }
   inline friend fix operator +  (const double x, const fix y) { fix t;  t.v = ftofix(x) + y.v;  return t; }

   inline friend fix operator -  (const fix x, const fix y)    { fix t;  t.v = x.v - y.v;        return t; }
   inline friend fix operator -  (const fix x, const int y)    { fix t;  t.v = x.v - itofix(y);  return t; }
   inline friend fix operator -  (const int x, const fix y)    { fix t;  t.v = itofix(x) - y.v;  return t; }
   inline friend fix operator -  (const fix x, const long y)   { fix t;  t.v = x.v - itofix(y);  return t; }
   inline friend fix operator -  (const long x, const fix y)   { fix t;  t.v = itofix(x) - y.v;  return t; }
   inline friend fix operator -  (const fix x, const float y)  { fix t;  t.v = x.v - ftofix(y);  return t; }
   inline friend fix operator -  (const float x, const fix y)  { fix t;  t.v = ftofix(x) - y.v;  return t; }
   inline friend fix operator -  (const fix x, const double y) { fix t;  t.v = x.v - ftofix(y);  return t; }
   inline friend fix operator -  (const double x, const fix y) { fix t;  t.v = ftofix(x) - y.v;  return t; }

   inline friend fix operator *  (const fix x, const fix y)    { fix t;  t.v = fmul(x.v, y.v);           return t; }
   inline friend fix operator *  (const fix x, const int y)    { fix t;  t.v = x.v * y;                  return t; }
   inline friend fix operator *  (const int x, const fix y)    { fix t;  t.v = x * y.v;                  return t; }
   inline friend fix operator *  (const fix x, const long y)   { fix t;  t.v = x.v * y;                  return t; }
   inline friend fix operator *  (const long x, const fix y)   { fix t;  t.v = x * y.v;                  return t; }
   inline friend fix operator *  (const fix x, const float y)  { fix t;  t.v = ftofix(fixtof(x.v) * y);  return t; }
   inline friend fix operator *  (const float x, const fix y)  { fix t;  t.v = ftofix(x * fixtof(y.v));  return t; }
   inline friend fix operator *  (const fix x, const double y) { fix t;  t.v = ftofix(fixtof(x.v) * y);  return t; }
   inline friend fix operator *  (const double x, const fix y) { fix t;  t.v = ftofix(x * fixtof(y.v));  return t; }

   inline friend fix operator /  (const fix x, const fix y)    { fix t;  t.v = fdiv(x.v, y.v);           return t; }
   inline friend fix operator /  (const fix x, const int y)    { fix t;  t.v = x.v / y;                  return t; }
   inline friend fix operator /  (const int x, const fix y)    { fix t;  t.v = x / fixtoi(y.v);          return t; }
   inline friend fix operator /  (const fix x, const long y)   { fix t;  t.v = x.v / y;                  return t; }
   inline friend fix operator /  (const long x, const fix y)   { fix t;  t.v = x / fixtoi(y.v);          return t; }
   inline friend fix operator /  (const fix x, const float y)  { fix t;  t.v = ftofix(fixtof(x.v) / y);  return t; }
   inline friend fix operator /  (const float x, const fix y)  { fix t;  t.v = ftofix(x / fixtof(y.v));  return t; }
   inline friend fix operator /  (const fix x, const double y) { fix t;  t.v = ftofix(fixtof(x.v) / y);  return t; }
   inline friend fix operator /  (const double x, const fix y) { fix t;  t.v = ftofix(x / fixtof(y.v));  return t; }

   inline friend fix operator << (const fix x, const int y)    { fix t;  t.v = x.v << y;    return t; }
   inline friend fix operator >> (const fix x, const int y)    { fix t;  t.v = x.v >> y;    return t; }

   inline friend int operator == (const fix x, const fix y)    { return (x.v == y.v);       }
   inline friend int operator == (const fix x, const int y)    { return (x.v == itofix(y)); }
   inline friend int operator == (const int x, const fix y)    { return (itofix(x) == y.v); }
   inline friend int operator == (const fix x, const long y)   { return (x.v == itofix(y)); }
   inline friend int operator == (const long x, const fix y)   { return (itofix(x) == y.v); }
   inline friend int operator == (const fix x, const float y)  { return (x.v == ftofix(y)); }
   inline friend int operator == (const float x, const fix y)  { return (ftofix(x) == y.v); }
   inline friend int operator == (const fix x, const double y) { return (x.v == ftofix(y)); }
   inline friend int operator == (const double x, const fix y) { return (ftofix(x) == y.v); }

   inline friend int operator != (const fix x, const fix y)    { return (x.v != y.v);       }
   inline friend int operator != (const fix x, const int y)    { return (x.v != itofix(y)); }
   inline friend int operator != (const int x, const fix y)    { return (itofix(x) != y.v); }
   inline friend int operator != (const fix x, const long y)   { return (x.v != itofix(y)); }
   inline friend int operator != (const long x, const fix y)   { return (itofix(x) != y.v); }
   inline friend int operator != (const fix x, const float y)  { return (x.v != ftofix(y)); }
   inline friend int operator != (const float x, const fix y)  { return (ftofix(x) != y.v); }
   inline friend int operator != (const fix x, const double y) { return (x.v != ftofix(y)); }
   inline friend int operator != (const double x, const fix y) { return (ftofix(x) != y.v); }

   inline friend int operator <  (const fix x, const fix y)    { return (x.v < y.v);        }
   inline friend int operator <  (const fix x, const int y)    { return (x.v < itofix(y));  }
   inline friend int operator <  (const int x, const fix y)    { return (itofix(x) < y.v);  }
   inline friend int operator <  (const fix x, const long y)   { return (x.v < itofix(y));  }
   inline friend int operator <  (const long x, const fix y)   { return (itofix(x) < y.v);  }
   inline friend int operator <  (const fix x, const float y)  { return (x.v < ftofix(y));  }
   inline friend int operator <  (const float x, const fix y)  { return (ftofix(x) < y.v);  }
   inline friend int operator <  (const fix x, const double y) { return (x.v < ftofix(y));  }
   inline friend int operator <  (const double x, const fix y) { return (ftofix(x) < y.v);  }

   inline friend int operator >  (const fix x, const fix y)    { return (x.v > y.v);        }
   inline friend int operator >  (const fix x, const int y)    { return (x.v > itofix(y));  }
   inline friend int operator >  (const int x, const fix y)    { return (itofix(x) > y.v);  }
   inline friend int operator >  (const fix x, const long y)   { return (x.v > itofix(y));  }
   inline friend int operator >  (const long x, const fix y)   { return (itofix(x) > y.v);  }
   inline friend int operator >  (const fix x, const float y)  { return (x.v > ftofix(y));  }
   inline friend int operator >  (const float x, const fix y)  { return (ftofix(x) > y.v);  }
   inline friend int operator >  (const fix x, const double y) { return (x.v > ftofix(y));  }
   inline friend int operator >  (const double x, const fix y) { return (ftofix(x) > y.v);  }

   inline friend int operator <= (const fix x, const fix y)    { return (x.v <= y.v);       }
   inline friend int operator <= (const fix x, const int y)    { return (x.v <= itofix(y)); }
   inline friend int operator <= (const int x, const fix y)    { return (itofix(x) <= y.v); }
   inline friend int operator <= (const fix x, const long y)   { return (x.v <= itofix(y)); }
   inline friend int operator <= (const long x, const fix y)   { return (itofix(x) <= y.v); }
   inline friend int operator <= (const fix x, const float y)  { return (x.v <= ftofix(y)); }
   inline friend int operator <= (const float x, const fix y)  { return (ftofix(x) <= y.v); }
   inline friend int operator <= (const fix x, const double y) { return (x.v <= ftofix(y)); }
   inline friend int operator <= (const double x, const fix y) { return (ftofix(x) <= y.v); }

   inline friend int operator >= (const fix x, const fix y)    { return (x.v >= y.v);       }
   inline friend int operator >= (const fix x, const int y)    { return (x.v >= itofix(y)); }
   inline friend int operator >= (const int x, const fix y)    { return (itofix(x) >= y.v); }
   inline friend int operator >= (const fix x, const long y)   { return (x.v >= itofix(y)); }
   inline friend int operator >= (const long x, const fix y)   { return (itofix(x) >= y.v); }
   inline friend int operator >= (const fix x, const float y)  { return (x.v >= ftofix(y)); }
   inline friend int operator >= (const float x, const fix y)  { return (ftofix(x) >= y.v); }
   inline friend int operator >= (const fix x, const double y) { return (x.v >= ftofix(y)); }
   inline friend int operator >= (const double x, const fix y) { return (ftofix(x) >= y.v); }

   inline friend fix sqrt(fix x)          { fix t;  t.v = fsqrt(x.v);  return t; }
   inline friend fix cos(fix x)           { fix t;  t.v = fcos(x.v);   return t; }
   inline friend fix sin(fix x)           { fix t;  t.v = fsin(x.v);   return t; }
   inline friend fix tan(fix x)           { fix t;  t.v = ftan(x.v);   return t; }
   inline friend fix acos(fix x)          { fix t;  t.v = facos(x.v);  return t; }
   inline friend fix asin(fix x)          { fix t;  t.v = fasin(x.v);  return t; }
   inline friend fix atan(fix x)          { fix t;  t.v = fatan(x.v);  return t; }
   inline friend fix atan2(fix x, fix y)  { fix t;  t.v = fatan2(x.v, y.v);  return t; }
};



#endif

