#pragma once

#include <cstdint>
#include "Wider.h"

namespace core
{
	template<typename TSigned>
	class Fraction
	{
	public:
		typedef typename std::make_unsigned<TSigned>::type TUnsigned;

		explicit constexpr Fraction(TSigned numerator, TUnsigned denominator)
			: myNumerator(numerator), myDenominator(denominator) { }

		constexpr Fraction(short val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(unsigned short val)
			: myNumerator(val), myDenominator(1) { }

		constexpr Fraction(int val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(unsigned int val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(long val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(unsigned long val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(long long val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr Fraction(unsigned long  long val)
			: myNumerator(val), myDenominator(1) { }

		explicit constexpr operator float() const noexcept
		{
			return static_cast<float>(myNumerator) / myDenominator;
		}

		explicit constexpr operator double() const noexcept
		{
			return static_cast<double>(myNumerator) / myDenominator;
		}

		constexpr TSigned Numerator() const { return myNumerator; }
		constexpr TUnsigned Denominator() const { return myDenominator; }

	private:
		TSigned myNumerator;
		TUnsigned myDenominator;
	};

	template<typename T>
	Fraction<T> operator* (Fraction<T> lhs, Fraction<T> rhs) noexcept
	{
		if (rhs.Numerator() == 0 || lhs.Numerator() == 0)
		{
			lhs = Fraction<T>(0);
		}
		else
		{
			T num = lhs.Numerator();
			Fraction<T>::TUnsigned den = lhs.Denominator();
			T n2;
			Fraction<T>::TUnsigned d2;
			int iter = 0; // Numerator/Denominator simplification
			do
			{
				n2 = (num * rhs.Numerator()) >> iter;
				d2 = (den * rhs.Denominator()) >> iter;
				iter++;
			} while (std::abs(n2) < std::abs(num) || d2 < den);

			lhs = Fraction<T>(n2, d2);
		}
		return lhs;
	}

	typedef Fraction<int64_t> Fraction64;
	typedef Fraction<int32_t> Fraction32;
}