// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
//
// This file has been modified from Ken Silverman's original release
// by Jonathon Fowler (jf@jonof.id.au)
// by the EDuke32 team (development@voidpoint.com)

#include "templates.h"
#ifndef pragmas_h_
#define pragmas_h_


#define EDUKE32_GENERATE_PRAGMAS                                                                            \
    EDUKE32_SCALER_PRAGMA(1)  EDUKE32_SCALER_PRAGMA(2)  EDUKE32_SCALER_PRAGMA(3)  EDUKE32_SCALER_PRAGMA(4)  \
    EDUKE32_SCALER_PRAGMA(5)  EDUKE32_SCALER_PRAGMA(6)  EDUKE32_SCALER_PRAGMA(7)  EDUKE32_SCALER_PRAGMA(8)  \
    EDUKE32_SCALER_PRAGMA(9)  EDUKE32_SCALER_PRAGMA(10) EDUKE32_SCALER_PRAGMA(11) EDUKE32_SCALER_PRAGMA(12) \
    EDUKE32_SCALER_PRAGMA(13) EDUKE32_SCALER_PRAGMA(14) EDUKE32_SCALER_PRAGMA(15) EDUKE32_SCALER_PRAGMA(16) \
    EDUKE32_SCALER_PRAGMA(17) EDUKE32_SCALER_PRAGMA(18) EDUKE32_SCALER_PRAGMA(19) EDUKE32_SCALER_PRAGMA(20) \
    EDUKE32_SCALER_PRAGMA(21) EDUKE32_SCALER_PRAGMA(22) EDUKE32_SCALER_PRAGMA(23) EDUKE32_SCALER_PRAGMA(24) \
    EDUKE32_SCALER_PRAGMA(25) EDUKE32_SCALER_PRAGMA(26) EDUKE32_SCALER_PRAGMA(27) EDUKE32_SCALER_PRAGMA(28) \
    EDUKE32_SCALER_PRAGMA(29) EDUKE32_SCALER_PRAGMA(30) EDUKE32_SCALER_PRAGMA(31) EDUKE32_SCALER_PRAGMA(32)

extern int32_t reciptable[2048];

// break the C version of divscale out from the others
// because asm version overflows in drawmapview()

#define qw(x) ((int64_t)(x))  // quadword cast
#define dw(x) ((int32_t)(x))  // doubleword cast
#define wo(x) ((int16_t)(x))  // word cast
#define by(x) ((uint8_t)(x))  // byte cast

#define DIVTABLESIZE 16384

static inline int32_t divscale(int32_t eax, int32_t ebx, int32_t ecx) { return (int64_t(eax) << ecx) / ebx; }
static inline double fdivscale(double eax, double ebx, int32_t ecx) { return (eax * (double)(qw(1) << ecx)) / ebx; }

static inline int64_t divscale64(int64_t eax, int64_t ebx, int64_t ecx) { return (eax << ecx) / ebx; }

#define EDUKE32_SCALER_PRAGMA(a) \
    static FORCE_INLINE int32_t divscale##a(int32_t eax, int32_t ebx) { return divscale(eax, ebx, a); } \
    static FORCE_INLINE double fdivscale##a(double eax, double ebx) { return fdivscale(eax, ebx, a); }
EDUKE32_GENERATE_PRAGMAS
#undef EDUKE32_SCALER_PRAGMA

static inline int32_t scale(int32_t eax, int32_t edx, int32_t ecx)
{
    return int64_t(eax) * edx / ecx;
}


//
// Generic C
//

#define EDUKE32_SCALER_PRAGMA(a)                                                                                                     \
    static FORCE_INLINE int32_t mulscale##a(int32_t eax, int32_t edx) { return dw((qw(eax) * edx) >> by(a)); }             \
    static FORCE_INLINE double fmulscale##a(double eax, double edx) { return (eax * edx) / (double)(qw(1) << a); }         \
    static FORCE_INLINE int32_t dmulscale##a(int32_t eax, int32_t edx, int32_t esi, int32_t edi)                           \
    {                                                                                                                                \
        return dw(((qw(eax) * edx) + (qw(esi) * edi)) >> by(a));                                                                     \
    }                                                                                                                                \
    static FORCE_INLINE double fdmulscale##a(double eax, double edx, double esi, double edi)                               \
    {                                                                                                                                \
        return ((eax * edx) + (esi * edi)) / (double)(qw(1) << a);                                                                   \
    }                                                                                                                                \
    static FORCE_INLINE int32_t tmulscale##a(int32_t eax, int32_t edx, int32_t ebx, int32_t ecx, int32_t esi, int32_t edi) \
    {                                                                                                                                \
        return dw(((qw(eax) * edx) + (qw(ebx) * ecx) + (qw(esi) * edi)) >> by(a));                                                   \
    }                                                                                                                                \
    static FORCE_INLINE double ftmulscale##a(double eax, double edx, double ebx, double ecx, double esi, double edi)       \
    {                                                                                                                                \
        return ((eax * edx) + (ebx * ecx) + (esi * edi)) / (double)(qw(1) << a);                                                     \
    }

EDUKE32_GENERATE_PRAGMAS

#undef EDUKE32_SCALER_PRAGMA


#define klabs(x) abs(x)
static inline constexpr int ksgn(int32_t a) { return (a > 0) - (a < 0); }

inline int sgn(int32_t a) { return (a > 0) - (a < 0); }

static inline int32_t mulscale(int32_t eax, int32_t edx, int32_t ecx) { return dw((qw(eax) * edx) >> by(ecx)); }
static inline double fmulscale(double eax, double edx, int32_t ecx) { return (eax * edx) / (double)(qw(1) << ecx); }
static inline int32_t dmulscale(int32_t eax, int32_t edx, int32_t esi, int32_t edi, int32_t ecx)
{
    return dw(((qw(eax) * edx) + (qw(esi) * edi)) >> by(ecx));
}
static inline double fdmulscale(double eax, double edx, double esi, double edi, int32_t ecx)
{
    return ((eax * edx) + (esi * edi)) / (double)(qw(1) << ecx);
}

static inline int32_t krecipasm(int32_t i)
{
    // Ken did this
    union { int32_t i; float f; } x;
    x.f = (float)i;
    i = x.i;
    return ((reciptable[(i >> 12) & 2047] >> (((i - 0x3f800000) >> 23) & 31)) ^ (i >> 31));
}

#undef qw
#undef dw
#undef wo
#undef by

#endif  // pragmas_h_
