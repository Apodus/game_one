/*
 * Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/core
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include "Fraction.h"
#include "Wider.h"

#include <cstdint>
#include <limits>
#include <cassert>
#include <cmath>
#include <type_traits>

#undef min
#undef max

// #define CORE_FIXEDPOINT_INF_NAN

namespace core
{
    /// FixedPoint.
    ///
    /// @details This is a fixed point type template for a given base type using a given number
    /// of fraction bits.
    ///
    /// This is a 32-bit sized fixed point type with a 18.14 format.
    /// With a 14-bit fraction part:
    ///   * 0.000061035156250 is the smallest double precision value that can be represented;
    ///   *
    template <typename BASE_TYPE, unsigned int FRACTION_BITS>
    class FixedPoint
    {
    public:
        using value_type = BASE_TYPE;
        static constexpr unsigned int FractionBits = FRACTION_BITS;
        static constexpr value_type ScaleFactor = static_cast<value_type>(1u << FractionBits);

        enum class ComparatorResult
        {
            Incomparable,
            Equal,
            LessThan,
            GreaterThan
        };

        static constexpr FixedPoint GetMin() noexcept
        {
            return FixedPoint{1, scalar_type{1}};
        }
        
        static constexpr FixedPoint GetInfinity() noexcept
        {
            return FixedPoint{numeric_limits::max(), scalar_type{1}};
        }
        
        static constexpr FixedPoint GetMax() noexcept
        {
            // max reserved for +inf
            return FixedPoint{numeric_limits::max() - 1, scalar_type{1}};
        }

        static constexpr FixedPoint GetNaN() noexcept
        {
            return FixedPoint{numeric_limits::lowest(), scalar_type{1}};
        }

        static constexpr FixedPoint GetNegativeInfinity() noexcept
        {
            // lowest reserved for NaN
            return FixedPoint{numeric_limits::lowest() + 1, scalar_type{1}};
        }
        
        static constexpr FixedPoint GetLowest() noexcept
        {
            // lowest reserved for NaN
            // lowest + 1 reserved for -inf
            return FixedPoint{numeric_limits::lowest() + 2, scalar_type{1}};
        }

        template <typename T>
        static constexpr value_type GetFromFloat(T val) noexcept
        {
            static_assert(std::is_floating_point<T>::value, "floating point value required");
            // Note: std::isnan(val) *NOT* constant expression, so can't use here!
            return !(val <= 0 || val >= 0)? GetNaN().m_value:
                (val > static_cast<long double>(GetMax()))? GetInfinity().m_value:
                (val < static_cast<long double>(GetLowest()))? GetNegativeInfinity().m_value:
                static_cast<value_type>(val * ScaleFactor);
        }
        
        template <typename T>
        static constexpr value_type GetFromSignedInt(T val) noexcept
        {
            static_assert(std::is_integral<T>::value, "integral value required");
            static_assert(std::is_signed<T>::value, "must be signed");
            return static_cast<value_type>((val > (GetMax().m_value / ScaleFactor))? GetInfinity().m_value:
                (val < (GetLowest().m_value / ScaleFactor))? GetNegativeInfinity().m_value:
                val * ScaleFactor);
        }
        
        template <typename T>
        static constexpr value_type GetFromUnsignedInt(T val) noexcept
        {
            static_assert(std::is_integral<T>::value, "integral value required");
            static_assert(!std::is_signed<T>::value, "must be unsigned");
            const auto max = static_cast<unsigned_wider_type>(GetMax().m_value / ScaleFactor);
            return (val > max)? GetInfinity().m_value: static_cast<value_type>(val) * ScaleFactor;
        }
        
        FixedPoint() = default;
        
        constexpr FixedPoint(long double val) noexcept:
            m_value{GetFromFloat(val)}
        {
        }
        
        constexpr FixedPoint(double val) noexcept:
            m_value{GetFromFloat(val)}
        {
        }

        constexpr FixedPoint(float val) noexcept:
            m_value{GetFromFloat(val)}
        {
        }
        
        constexpr FixedPoint(unsigned long long val) noexcept:
            m_value{GetFromUnsignedInt(val)}
        {
        }

        constexpr FixedPoint(unsigned long val) noexcept:
            m_value{GetFromUnsignedInt(val)}
        {
        }
        
        constexpr FixedPoint(unsigned int val) noexcept:
            m_value{GetFromUnsignedInt(val)}
        {
        }

        constexpr FixedPoint(long long val) noexcept:
            m_value{GetFromSignedInt(val)} {}

		template<typename T>
		constexpr FixedPoint(const Fraction<T>& f) noexcept :
			m_value{ static_cast<value_type>(static_cast<wider_type>(
			f.Numerator()) * ScaleFactor / f.Denominator()) } {}

        constexpr FixedPoint(int val) noexcept:
            m_value{GetFromSignedInt(val)}
        {
        }
        
        constexpr FixedPoint(short val) noexcept:
            m_value{GetFromSignedInt(val)}
        {
        }
		
		 constexpr FixedPoint(value_type val, unsigned int fraction) noexcept:
            m_value{static_cast<value_type>(static_cast<uint32_t>(val * ScaleFactor) | fraction)}
        {
        }

        template <typename BT, unsigned int FB>
        constexpr FixedPoint(const FixedPoint<BT, FB> val) noexcept:
            FixedPoint(static_cast<long double>(val))
        {

        }
        
        // Methods
        
        template <typename T>
        constexpr T ConvertTo() const noexcept
        {
            return isnan()? std::numeric_limits<T>::signaling_NaN():
                !isfinite()? std::numeric_limits<T>::infinity() * getsign():
                static_cast<T>(m_value) / ScaleFactor;
        }

        constexpr ComparatorResult Compare(const FixedPoint other) const noexcept
        {
            if (isnan() || other.isnan())
            {
                return ComparatorResult::Incomparable;
            }
            if (m_value < other.m_value)
            {
                return ComparatorResult::LessThan;
            }
            if (m_value > other.m_value)
            {
                return ComparatorResult::GreaterThan;
            }
            return ComparatorResult::Equal;
        }

        // Unary operations

        explicit constexpr operator long double() const noexcept
        {
            return ConvertTo<long double>();
        }
        
        explicit constexpr operator double() const noexcept
        {
            return ConvertTo<double>();
        }
        
        explicit constexpr operator float() const noexcept
        {
            return ConvertTo<float>();
        }
    
        explicit constexpr operator long long() const noexcept
        {
            return m_value / ScaleFactor;
        }
        
        explicit constexpr operator long() const noexcept
        {
            return m_value / ScaleFactor;
        }

        explicit constexpr operator unsigned long long() const noexcept
        {
            // Behavior is undefined if m_value is negative
            return static_cast<unsigned long long>(m_value / ScaleFactor);
        }

        explicit constexpr operator unsigned long() const noexcept
        {
            // Behavior is undefined if m_value is negative
            return static_cast<unsigned long>(m_value / ScaleFactor);
        }
        
        explicit constexpr operator unsigned int() const noexcept
        {
            // Behavior is undefined if m_value is negative
            return static_cast<unsigned int>(m_value / ScaleFactor);
        }

        explicit constexpr operator int() const noexcept
        {
            return static_cast<int>(m_value / ScaleFactor);
        }
        
        explicit constexpr operator short() const noexcept
        {
            return static_cast<short>(m_value / ScaleFactor);
        }

		template<typename T>
		explicit constexpr operator Fraction<T>() const noexcept
		{
			return Fraction<T>(m_value, ScaleFactor);
		}
        
        constexpr FixedPoint operator- () const noexcept
        {
#ifdef CORE_FIXEDPOINT_INF_NAN
            return (isnan())? *this: Fixed{-m_value, scalar_type{1}};
#else
			return FixedPoint{ -m_value, scalar_type{ 1 } };
#endif
        }
        
        constexpr FixedPoint operator+ () const noexcept
        {
            return *this;
        }
        
        explicit constexpr operator bool() const noexcept
        {
            return m_value != 0;
        }
        
        constexpr bool operator! () const noexcept
        {
            return m_value == 0;
        }
        
        constexpr FixedPoint& operator+= (FixedPoint val) noexcept
        {
#ifdef CORE_FIXEDPOINT_INF_NAN
            if (isnan() || val.isnan()
                || ((m_value == GetInfinity().m_value) && (val.m_value == GetNegativeInfinity().m_value))
                || ((m_value == GetNegativeInfinity().m_value) && (val.m_value == GetInfinity().m_value))
                )
            {
                *this = GetNaN();
            }
            else if (val.m_value == GetInfinity().m_value)
            {
                m_value = GetInfinity().m_value;
            }
            else if (val.m_value == GetNegativeInfinity().m_value)
            {
                m_value = GetNegativeInfinity().m_value;
            }
            else if (isfinite() && val.isfinite())
#endif
			{
                const auto result = wider_type{m_value} + val.m_value;
#ifdef CORE_FIXEDPOINT_INF_NAN
                if (result > GetMax().m_value)
                {
                    // overflow from max
                    m_value = GetInfinity().m_value;
                }
                else if (result < GetLowest().m_value)
                {
                    // overflow from lowest
                    m_value = GetNegativeInfinity().m_value;
                }
                else
#endif
                {
                    m_value = static_cast<value_type>(result);
                }
            }
            return *this;
        }

        constexpr FixedPoint& operator-= (FixedPoint val) noexcept
        {
#ifdef CORE_FIXEDPOINT_INF_NAN
            if (isnan() || val.isnan()
                || ((m_value == GetInfinity().m_value) && (val.m_value == GetInfinity().m_value))
                || ((m_value == GetNegativeInfinity().m_value) && (val.m_value == GetNegativeInfinity().m_value))
            )
            {
                *this = GetNaN();
            }
            else if (val.m_value == GetInfinity().m_value)
            {
                m_value = GetNegativeInfinity().m_value;
            }
            else if (val.m_value == GetNegativeInfinity().m_value)
            {
                m_value = GetInfinity().m_value;
            }
            else if (isfinite() && val.isfinite())
#endif
			{
                const auto result = wider_type{m_value} - val.m_value;
#ifdef CORE_FIXEDPOINT_INF_NAN
				if (result > GetMax().m_value)
                {
                    // overflow from max
                    m_value = GetInfinity().m_value;
                }
                else if (result < GetLowest().m_value)
                {
                    // overflow from lowest
                    m_value = GetNegativeInfinity().m_value;
                }
                else
#endif
                {
                    m_value = static_cast<value_type>(result);
                }
            }
            return *this;
        }

        constexpr FixedPoint& operator*= (FixedPoint val) noexcept
        {
#ifdef CORE_FIXEDPOINT_INF_NAN
            if (isnan() || val.isnan())
            {
                *this = GetNaN();
            }
            else if (!isfinite() || !val.isfinite())
            {
                if (m_value == 0 || val.m_value == 0)
                {
                    *this = GetNaN();
                }
                else
                {
                    *this = ((m_value > 0) != (val.m_value > 0))? -GetInfinity(): GetInfinity();
                }
            }
            else
#endif
			{
				const auto product = wider_type{ m_value } *wider_type{ val.m_value };

#if (((wider_type)-1) >> 1) == ((wider_type)-1)
				// Right shift of a negative signed number has implementation-defined behaviour.
				// Use shifting if compiler generates arithmetic shift (sign is not lost)
				const auto result = product >> FractionBits;
#else
				const auto result = product / ScaleFactor;
#endif
				// #ifdef CORE_FIXEDPOINT_INF_NAN                
				if (product != 0 && result == 0)
				{
					// underflow
					m_value = static_cast<value_type>(result);
                }
                else if (result > GetMax().m_value)
                {
                    // overflow from max
                    m_value = GetInfinity().m_value;
                }
                else if (result < GetLowest().m_value)
                {
                    // overflow from lowest
                    m_value = GetNegativeInfinity().m_value;
                }
                else
// #endif
                {
                    m_value = static_cast<value_type>(result);
                }
            }
            return *this;
        }

        constexpr FixedPoint& operator/= (FixedPoint val) noexcept
        {
#ifdef CORE_FIXEDPOINT_INF_NAN
            if (isnan() || val.isnan())
            {
                *this = GetNaN();
            }
            else if (!isfinite() && !val.isfinite())
            {
                *this = GetNaN();
            }
            else if (!isfinite())
            {
                *this = ((m_value > 0) != (val.m_value > 0))? -GetInfinity(): GetInfinity();
            }
            else if (!val.isfinite())
            {
                *this = 0;
            }
            else
#endif
            {
                const auto product = wider_type{m_value} * ScaleFactor;
                const auto result = product / val.m_value;
#ifdef CORE_FIXEDPOINT_INF_NAN
                if (product != 0 && result == 0)
                {
                    // underflow
                    m_value = static_cast<value_type>(result);
                }
                else if (result > GetMax().m_value)
                {
                    // overflow from max
                    m_value = GetInfinity().m_value;
                }
                else if (result < GetLowest().m_value)
                {
                    // overflow from lowest
                    m_value = GetNegativeInfinity().m_value;
                }
                else
#endif
                {
                    m_value = static_cast<value_type>(result);
                }
            }
            return *this;
        }
        
        constexpr FixedPoint& operator%= (FixedPoint val) noexcept
        {
            assert(!isnan());
            assert(!val.isnan());

            m_value %= val.m_value;
            return *this;
        }

    private:
		friend class FixedMath;

        using wider_type = typename core::Wider<value_type>::type;
        using unsigned_wider_type = typename std::make_unsigned<wider_type>::type;

        struct scalar_type
        {
            value_type value = 1;
        };
        
        using numeric_limits = std::numeric_limits<value_type>;
        
        constexpr FixedPoint(value_type val, scalar_type scalar) noexcept:
            m_value{val * scalar.value}
        {
        }
        
        constexpr bool isfinite() const noexcept
        {
            return (m_value > GetNegativeInfinity().m_value)
                && (m_value < GetInfinity().m_value);
        }

        constexpr bool isnan() const noexcept
        {
            return m_value == GetNaN().m_value;
        }
        
        constexpr int getsign() const noexcept
        {
            return (m_value >= 0)? +1: -1;
        }
        
        value_type m_value;
    };

#if 0
    template <typename BT, unsigned int FB>
    constexpr Fixed<BT, FB> operator+ (Fixed<BT, FB> lhs, Fixed<BT, FB> rhs) noexcept
    {
        lhs += rhs;
        return lhs;
    }
#endif

    using Fixed32 = FixedPoint<std::int32_t,14>;

    // Fixed32 free functions.
    
    constexpr Fixed32 operator+ (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        lhs += rhs;
        return lhs;
    }

    constexpr Fixed32 operator- (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        lhs -= rhs;
        return lhs;
    }
    
    constexpr Fixed32 operator* (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        lhs *= rhs;
        return lhs;
    }
    
    constexpr Fixed32 operator/ (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        lhs /= rhs;
        return lhs;
    }
    
    constexpr Fixed32 operator% (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        lhs %= rhs;
        return lhs;
    }    
    
    constexpr bool operator== (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        return lhs.Compare(rhs) == Fixed32::ComparatorResult::Equal;
    }
    
    constexpr bool operator!= (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        return lhs.Compare(rhs) != Fixed32::ComparatorResult::Equal;
    }
    
    constexpr bool operator <= (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return (result == Fixed32::ComparatorResult::LessThan) || (result == Fixed32::ComparatorResult::Equal);
    }
    
    constexpr bool operator >= (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return (result == Fixed32::ComparatorResult::GreaterThan) || (result == Fixed32::ComparatorResult::Equal);
    }
    
    constexpr bool operator < (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return result == Fixed32::ComparatorResult::LessThan;
    }
    
    constexpr bool operator > (Fixed32 lhs, Fixed32 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return result == Fixed32::ComparatorResult::GreaterThan;
    }

#ifndef _WIN32
    // Fixed64 free functions.

    using Fixed64 = Fixed<std::int64_t,24>;

    constexpr Fixed64 operator+ (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        lhs += rhs;
        return lhs;
    }
    
    constexpr Fixed64 operator- (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        lhs -= rhs;
        return lhs;
    }
    
    constexpr Fixed64 operator* (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        lhs *= rhs;
        return lhs;
    }
    
    constexpr Fixed64 operator/ (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        lhs /= rhs;
        return lhs;
    }
    
    constexpr Fixed64 operator% (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        lhs %= rhs;
        return lhs;
    }
    
    constexpr bool operator== (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        return lhs.Compare(rhs) == Fixed64::ComparatorResult::Equal;
    }
    
    constexpr bool operator!= (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        return lhs.Compare(rhs) != Fixed64::ComparatorResult::Equal;
    }
    
    constexpr bool operator <= (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return (result == Fixed64::ComparatorResult::LessThan) || (result == Fixed64::ComparatorResult::Equal);
    }
    
    constexpr bool operator >= (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return (result == Fixed64::ComparatorResult::GreaterThan) || (result == Fixed64::ComparatorResult::Equal);
    }
    
    constexpr bool operator < (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return result == Fixed64::ComparatorResult::LessThan;
    }
    
    constexpr bool operator > (Fixed64 lhs, Fixed64 rhs) noexcept
    {
        const auto result = lhs.Compare(rhs);
        return result == Fixed64::ComparatorResult::GreaterThan;
    }

    template<> struct Wider<Fixed32> { using type = Fixed64; };

#endif /* !_WIN32 */

	inline core::Fixed32 sqrt(core::Fixed32 t)
	{
		if (t * core::Fixed32(1000) < core::Fixed32(1))
		{
			return core::Fixed32(0u);
		}
		// approximates the square root quite nicely
		core::Fixed32 currentVal(t);
		currentVal *= core::Fraction32(1, 2);
		for (int i = 0; i < 10; ++i)
		{
			currentVal += t / currentVal;
			currentVal *= core::Fraction32(1, 2);
		}
		return currentVal;
	}
} // namespace core

namespace std
{
    // Fixed32

    template <>
    class numeric_limits<core::Fixed32>
    {
    public:
        static constexpr bool is_specialized = true;
        
        static constexpr core::Fixed32 min() noexcept { return core::Fixed32::GetMin(); }
        static constexpr core::Fixed32 max() noexcept    { return core::Fixed32::GetMax(); }
        static constexpr core::Fixed32 lowest() noexcept { return core::Fixed32::GetLowest(); }
        
        static constexpr int digits = 31 - core::Fixed32::FractionBits;
        static constexpr int digits10 = 31 - core::Fixed32::FractionBits;
        static constexpr int max_digits10 = 5; // TODO: check this
        
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = true;
        static constexpr int radix = 0;
        // static constexpr core::Fixed32 epsilon() noexcept { return core::Fixed32{0}; } // TODO
        // static constexpr core::Fixed32 round_error() noexcept { return core::Fixed32{0}; } // TODO
        
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        
        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = true;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr core::Fixed32 infinity() noexcept { return core::Fixed32::GetInfinity(); }
        static constexpr core::Fixed32 quiet_NaN() noexcept { return core::Fixed32::GetNaN(); }
        // static constexpr core::Fixed32 signaling_NaN() noexcept { return core::Fixed32{0}; }
        // static constexpr core::Fixed32 denorm_min() noexcept { return core::Fixed32{0}; }
        
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;
        static constexpr float_round_style round_style = round_toward_zero;
    };
    
#ifndef _WIN32

    template <>
    class numeric_limits<core::Fixed64>
    {
    public:
        static constexpr bool is_specialized = true;
        
        static constexpr core::Fixed64 min() noexcept { return core::Fixed64::GetMin(); }
        static constexpr core::Fixed64 max() noexcept    { return core::Fixed64::GetMax(); }
        static constexpr core::Fixed64 lowest() noexcept { return core::Fixed64::GetLowest(); }
        
        static constexpr int digits = 63 - core::Fixed64::FractionBits;
        static constexpr int digits10 = 63 - core::Fixed64::FractionBits;
        static constexpr int max_digits10 = 10; // TODO: check this
        
        static constexpr bool is_signed = true;
        static constexpr bool is_integer = false;
        static constexpr bool is_exact = true;
        static constexpr int radix = 0;
        static constexpr core::Fixed64 epsilon() noexcept { return core::Fixed64{0}; } // TODO
        static constexpr core::Fixed64 round_error() noexcept { return core::Fixed64{0}; } // TODO
        
        static constexpr int min_exponent = 0;
        static constexpr int min_exponent10 = 0;
        static constexpr int max_exponent = 0;
        static constexpr int max_exponent10 = 0;
        
        static constexpr bool has_infinity = true;
        static constexpr bool has_quiet_NaN = true;
        static constexpr bool has_signaling_NaN = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss = false;
        static constexpr core::Fixed64 infinity() noexcept { return core::Fixed64::GetInfinity(); }
        static constexpr core::Fixed64 quiet_NaN() noexcept { return core::Fixed64::GetNaN(); }
        static constexpr core::Fixed64 signaling_NaN() noexcept { return core::Fixed64{0}; }
        static constexpr core::Fixed64 denorm_min() noexcept { return core::Fixed64{0}; }
        
        static constexpr bool is_iec559 = false;
        static constexpr bool is_bounded = true;
        static constexpr bool is_modulo = false;
        
        static constexpr bool traps = false;
        static constexpr bool tinyness_before = false;
        static constexpr float_round_style round_style = round_toward_zero;
    };

#endif /* _WIN32 */

} // namespace std

