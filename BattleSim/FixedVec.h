#pragma once

#include "FixedPoint.h"
#include "SVec16i.h"

namespace core
{
	class FixedVec32
	{
#ifdef CORE_SSE2_ENABLED
		SVec16i myIntVec;
#else
		Fixed32 myIntVec[4];
#endif
	public:
		size_t Size() const { return sizeof(SVec16i) / sizeof(Fixed32); }
		FixedVec32() {}

#ifdef CORE_SSE2_ENABLED
		FixedVec32(const SVec16i& intVec) : myIntVec(intVec) {}

		operator SVec16i() const { return myIntVec; }
#endif

		FixedVec32(const Fixed32& fs)
		{
#ifdef CORE_SSE2_ENABLED
			myIntVec.Store(fs.m_value);
#else
			for (size_t i = 0; i < 4; i++)
			{
				myIntVec[i] = fs;
			}
#endif
		}

		FixedVec32(const Fixed32* fs)
		{
			Store(fs);
		}

		FixedVec32(const Fixed32& a, const Fixed32& b, const Fixed32& c, const Fixed32& d)
		{
			Store(a, b, c, d);
		}

		void Store(const Fixed32* fs)
		{
#ifdef CORE_SSE2_ENABLED
			Store(fs[0], fs[1], fs[2], fs[3]);
#else
			memcpy(myIntVec, fs, 16);
#endif
		}

		void Store(const Fixed32& a, const Fixed32& b, const Fixed32& c, const Fixed32& d)
		{
#ifdef CORE_SSE2_ENABLED
			myIntVec.Store(a.m_value, b.m_value, c.m_value, d.m_value);
#else
			myIntVec[0] = a;
			myIntVec[1] = b;
			myIntVec[2] = c;
			myIntVec[3] = d;
#endif
		}

		void Load(Fixed32& a, Fixed32& b, Fixed32& c, Fixed32& d) const
		{
#ifdef CORE_SSE2_ENABLED
			int32_t tmp[4];
			myIntVec.Load(tmp);
			a.m_value = tmp[0];
			b.m_value = tmp[1];
			c.m_value = tmp[2];
			d.m_value = tmp[3];
#else
			a = myIntVec[0];
			b = myIntVec[1];
			c = myIntVec[2];
			d = myIntVec[3];
#endif
		}

#ifndef CORE_SSE2_ENABLED
		const inline Fixed32& operator[] (const size_t index) const
		{
			return myIntVec[index];
		}
#endif

		void Load(Fixed32* a) const
		{
#ifdef CORE_SSE2_ENABLED
			myIntVec.Load(reinterpret_cast<int32_t*>(a));
#else
			memcpy(a, myIntVec, 16);
#endif
		}
	};

	static inline FixedVec32 operator + (FixedVec32 const & a, Fixed32 const & b)
	{
#ifdef CORE_SSE2_ENABLED
		return SVec16i(a) + SVec16i(FixedVec32(b));
#else
		return FixedVec32(a[0] + b, a[1] + b, a[2] + b, a[3] + b);
#endif
	}

	static inline FixedVec32 operator + (FixedVec32 const & a, FixedVec32 const & b)
	{
#ifdef CORE_SSE2_ENABLED
		return SVec16i(a) + SVec16i(b);
#else
		return FixedVec32(a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]);
#endif
	}

	static inline FixedVec32 & operator += (FixedVec32 & a, FixedVec32 const & b)
	{
		a = a + b;
		return a;
	}

	static inline FixedVec32 operator - (FixedVec32 const & a, FixedVec32 const & b)
	{
#ifdef CORE_SSE2_ENABLED
		return SVec16i(a) - SVec16i(b);
#else
		return FixedVec32(a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]);
#endif
	}

	static inline FixedVec32 operator - (FixedVec32 const & a)
	{
#ifdef CORE_SSE2_ENABLED
		return -SVec16i(a);
#else
		return FixedVec32(-a[0], -a[1], -a[2], -a[3]);
#endif
	}

	static inline FixedVec32 & operator -= (FixedVec32 & a, FixedVec32 const & b)
	{
		a = a - b;
		return a;
	}

	// Because no _mm_mul_epi32() in SSE2, we emulate it here.
	// Multiplies 4 32-bit integers from a by 4 32-bit intergers from b.
	// The 4 multiplication results should be represented within 32-bit
	// integers, otherwise they would be overflow.
	static inline  __m128i Multiply32_SSE2(const __m128i& a, const __m128i& b) {
		// Calculate results of a0 * b0 and a2 * b2.
		__m128i r1 = _mm_mul_epu32(a, b);
		// Calculate results of a1 * b1 and a3 * b3.
		__m128i r2 = _mm_mul_epu32(_mm_srli_si128(a, 4), _mm_srli_si128(b, 4));
		// Shuffle results to [63..0] and interleave the results.
		__m128i r = _mm_unpacklo_epi32(_mm_shuffle_epi32(r1, _MM_SHUFFLE(0, 0, 2, 0)),
			_mm_shuffle_epi32(r2, _MM_SHUFFLE(0, 0, 2, 0)));
		return r;
	}



	static inline FixedVec32 operator * (FixedVec32 const & a, FixedVec32 const & b)
	{
#ifdef CORE_SSE2_ENABLED
#if (((int64_t)-1) >> 1) == ((int64_t)-1) && defined(CORE_SSE4_1_ENABLED)
		// bytes 0 & 2 multipled to two 64 byte values
		auto evnp = _mm_mul_epi32(SVec16i(a), SVec16i(b));
		
		// bytes 1 & 3 multipled to two 64 byte values
		__m128i odda = _mm_srli_epi64(SVec16i(a), 32);
		__m128i oddb = _mm_srli_epi64(SVec16i(b), 32);
		auto oddp = _mm_mul_epi32(odda, oddb);

		// divide by scale factor
		evnp = _mm_srli_epi64(evnp, Fixed32::FractionBits);
		oddp = _mm_srli_epi64(oddp, Fixed32::FractionBits);

		// Merge
		__m128i evn_mask = _mm_setr_epi32(-1, 0, -1, 0);
		__m128i evn_result = _mm_and_si128(evnp, evn_mask);
		__m128i odd_result = _mm_slli_epi64(oddp, 32);
		__m128i res = _mm_or_si128(evn_result, odd_result);
		return SVec16i(res);
#else
		Fixed32 num[4];
		Fixed32 den[4];
		a.Load(num);
		b.Load(den);
		for (size_t i = 0; i < 4; i++)
		{
			num[i] = num[i] * den[i];
		}
		return FixedVec32(num);
#endif
#else
		return FixedVec32(a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]);
#endif
	}

	static inline FixedVec32 operator / (FixedVec32 const & a, FixedVec32 const & b)
	{
#ifdef CORE_SSE2_ENABLED
		Fixed32 num[4];
		Fixed32 den[4];
		a.Load(num);
		b.Load(den);
		for (size_t i = 0; i < 4; i++)
		{
			num[i] = den[i] ? num[i] / den[i] : 0;
		}
		return FixedVec32(num);
#else
		Fixed32 tmp[4];
		for (size_t i = 0; i < 4; i++)
		{
			tmp[i] = b[i] ? a[i] / b[i] : 0;
		}
		return FixedVec32(tmp[0], tmp[1], tmp[2], tmp[3]);
#endif
	}

	static inline FixedVec32 operator * (FixedVec32 const & a, Fixed32 const & b)
	{
		return a * FixedVec32(b);
	}
}