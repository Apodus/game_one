#pragma once

// #define CORE_SSE2_ENABLED
// #define CORE_SSE4_1_ENABLED

#include "emmintrin.h"

#include <algorithm>
#include <cstdint>
#include "intrin.h"

#include "FixedPoint.h"

namespace core
{
#if defined (__GNUC__) || defined(__clang__)
	static inline uint32_t bit_scan_reverse(uint32_t a) __attribute__((pure));
	static inline uint32_t bit_scan_reverse(uint32_t a) {
		uint32_t r;
		__asm("bsrl %1, %0" : "=r"(r) : "r"(a) : );
		return r;
	}
#else
	static inline uint32_t bit_scan_reverse(uint32_t a) {
		unsigned long r;
		_BitScanReverse(&r, a);                      // defined in intrin.h for MS and Intel compilers
		return r;
	}
#endif

	// encapsulate parameters for fast division on vector of 4 32-bit signed integers
#ifdef CORE_SSE2_ENABLED
	__declspec(align(16))
	class Divisor_i {
	protected:
		__m128i multiplier;                                    // multiplier used in fast division
		__m128i shift1;                                        // shift count used in fast division
		__m128i sign;                                          // sign of divisor
	public:
		Divisor_i() {};                                        // Default constructor
		 Divisor_i(int32_t d) {                                 // Constructor with divisor
			set(d);
		}
		Divisor_i(int m, int s1, int sgn) 
			:
			multiplier(_mm_set1_epi32(m)),
			shift1(_mm_cvtsi32_si128(s1)),
			sign(_mm_set1_epi32(sgn))
		{                    // Constructor with precalculated multiplier, shift and sign
			
		}
		void set(int32_t d) {                                  // Set or change divisor, calculate parameters
			const int32_t d1 = std::abs(d);
			int32_t sh = 0;
			int m = 0;
			if (d1 > 1) 
			{
				sh = bit_scan_reverse(d1 - 1);                   // shift count = ceil(log2(d1))-1 = (bit_scan_reverse(d1-1)+1)-1
				m = int32_t((int64_t(1) << (32 + sh)) / d1 - ((int64_t(1) << 32) - 1)); // calculate multiplier
			}
			else {
				m = 1;                                        // for d1 = 1
				sh = 0;
				if (d == 0)
				{
					d = 1;
				//	m /= d;                            // provoke error here if d = 0
				}
				if (uint32_t(d) == 0x80000000u) {              // fix overflow for this special case
					m = 0x80000001;
					sh = 30;
				}
			}
			multiplier = _mm_set1_epi32(m);                    // broadcast multiplier
			shift1 = _mm_setr_epi32(sh, 0, 0, 0);          // shift count
			sign = _mm_set1_epi32(d < 0 ? -1 : 0);       // sign of divisor
		}
		__m128i getm() const {                                 // get multiplier
			return multiplier;
		}
		__m128i gets1() const {                                // get shift count
			return shift1;
		}
		__m128i getsign() const {                              // get sign of divisor
			return sign;
		}
	};
#else
	typedef int32_t Divisor_i;
#endif


	// 16 byte integer streaming vector
	// template<typename T>
	class SVec16i
	{

	public:
#ifdef CORE_SSE2_ENABLED
		__m128i myData;
#else
		int32_t myData[4];
#endif

		SVec16i() {}
		SVec16i(int32_t a, int32_t b, int32_t c, int32_t d)
		{
			Store(a, b, c, d);
		}

		SVec16i(int32_t* data) 
		{
#ifdef CORE_SSE2_ENABLED
			myData = _mm_setr_epi32(data[0], data[1], data[2], data[3]);
#else
			memcpy(myData, data, 16);
#endif
		}

		// _mm_store_si128(reinterpret_cast<__m128i*>(a), vsum);    // store sum

		void Store(int32_t a)
		{
#ifdef CORE_SSE2_ENABLED
			myData = _mm_set1_epi32(a);
#else
			for (size_t i = 0; i < 4; i++)
			{
				myData[i] = a;
			}
#endif
		}

		void Store(int32_t a, int32_t b, int32_t c, int32_t d)
		{
#ifdef CORE_SSE2_ENABLED
			myData = _mm_setr_epi32(a, b, c, d);
#else
			myData[0] = a;
			myData[1] = b;
			myData[2] = c;
			myData[3] = d;
#endif
		}

		void Load(int32_t* p) const 
		{
#ifdef CORE_SSE2_ENABLED
			_mm_store_si128(reinterpret_cast<__m128i*>(p), myData);
#else
			memcpy(p, myData, 16);
#endif
		}

#ifdef CORE_SSE2_ENABLED
		operator __m128i() const { return myData; }
		SVec16i(__m128i const & data) { myData = data; }
#endif
	};

	static inline SVec16i operator + (SVec16i const & a, SVec16i const & b) 
	{
#ifdef CORE_SSE2_ENABLED
		return _mm_add_epi32(a, b);
#else
		SVec16i tmp;
		for (size_t i = 0; i < 4; i++)
		{
			tmp.myData[i] = a.myData[i] + b.myData[i];
		}
		return tmp;
#endif
	}

	static inline SVec16i & operator += (SVec16i & a, SVec16i const & b) 
	{
		a = a + b;
		return a;
	}

	static inline SVec16i operator - (SVec16i const & a, SVec16i const & b) 
	{
#ifdef CORE_SSE2_ENABLED
		return _mm_sub_epi32(a, b);
#else
		SVec16i tmp;
		for (size_t i = 0; i < 4; i++)
		{
			tmp.myData[i] = a.myData[i] - b.myData[i];
		}
		return tmp;
#endif
	}

	static inline SVec16i operator - (SVec16i const & a) 
	{
#ifdef CORE_SSE2_ENABLED
		return _mm_sub_epi32(_mm_setzero_si128(), a);
#else
		SVec16i tmp;
		for (size_t i = 0; i < 4; i++)
		{
			tmp.myData[i] = 0 - a.myData[i];
		}
		return tmp;
#endif

	}

	static inline SVec16i & operator -= (SVec16i & a, SVec16i const & b)
	{
		a = a - b;
		return a;
	}

	static inline SVec16i operator / (SVec16i const & a, Divisor_i const & d)
	{
#ifdef CORE_SSE2_ENABLED
		__m128i t1 = _mm_mul_epu32(a, d.getm());               // 32x32->64 bit unsigned multiplication of a[0] and a[2]
		__m128i t2 = _mm_srli_epi64(t1, 32);                   // high dword of result 0 and 2
		__m128i t3 = _mm_srli_epi64(a, 32);                    // get a[1] and a[3] into position for multiplication
		__m128i t4 = _mm_mul_epu32(t3, d.getm());              // 32x32->64 bit unsigned multiplication of a[1] and a[3]
		__m128i t5 = _mm_set_epi32(-1, 0, -1, 0);                // mask of dword 1 and 3
		__m128i t6 = _mm_and_si128(t4, t5);                    // high dword of result 1 and 3
		__m128i t7 = _mm_or_si128(t2, t6);                     // combine all four results of unsigned high mul into one vector
															   // convert unsigned to signed high multiplication (from: H S Warren: Hacker's delight, 2003, p. 132)
		__m128i u1 = _mm_srai_epi32(a, 31);                    // sign of a
		__m128i u2 = _mm_srai_epi32(d.getm(), 31);             // sign of m [ m is always negative, except for abs(d) = 1 ]
		__m128i u3 = _mm_and_si128(d.getm(), u1);             // m * sign of a
		__m128i u4 = _mm_and_si128(a, u2);                    // a * sign of m
		__m128i u5 = _mm_add_epi32(u3, u4);                   // sum of sign corrections
		__m128i u6 = _mm_sub_epi32(t7, u5);                   // high multiplication result converted to signed
		__m128i t8 = _mm_add_epi32(u6, a);                     // add a
		__m128i t9 = _mm_sra_epi32(t8, d.gets1());             // shift right arithmetic
		__m128i t10 = _mm_sub_epi32(u1, d.getsign());           // sign of a - sign of d
		__m128i t11 = _mm_sub_epi32(t9, t10);                   // + 1 if a < 0, -1 if d < 0
		return        _mm_xor_si128(t11, d.getsign());          // change sign if divisor negative
#else
SVec16i tmp;
for (size_t i = 0; i < 4; i++)
{
	tmp.myData[i] = a.myData[i] / d;
}
return tmp;
#endif
	}

	static inline SVec16i operator * (SVec16i const & a, SVec16i const & b) 
	{
#ifdef CORE_SSE2_ENABLED
#if INSTRSET >= 5  // SSE4.1 instruction set
		return _mm_mullo_epi32(a, b);
#else
		__m128i a13 = _mm_shuffle_epi32(a, 0xF5);          // (-,a3,-,a1)
		__m128i b13 = _mm_shuffle_epi32(b, 0xF5);          // (-,b3,-,b1)
		__m128i prod02 = _mm_mul_epu32(a, b);                 // (-,a2*b2,-,a0*b0)
		__m128i prod13 = _mm_mul_epu32(a13, b13);             // (-,a3*b3,-,a1*b1)
		__m128i prod01 = _mm_unpacklo_epi32(prod02, prod13);   // (-,-,a1*b1,a0*b0) 
		__m128i prod23 = _mm_unpackhi_epi32(prod02, prod13);   // (-,-,a3*b3,a2*b2) 
		return           _mm_unpacklo_epi64(prod01, prod23);   // (ab3,ab2,ab1,ab0)
#endif
#else
		SVec16i tmp;
		for (size_t i = 0; i < 4; i++)
		{
			tmp.myData[i] = a.myData[i] * b.myData[i];
		}
		return tmp;
#endif
	}

	static inline SVec16i & operator *= (SVec16i & a, SVec16i const & b) 
	{
		a = a * b;
		return a;
	}


}

