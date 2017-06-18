#pragma once

#include "types.h"

namespace core
{
	template<typename TSigned>
	class Fraction
	{
		typedef typename std::make_unsigned<TSigned>::type TUnsigned;
	public:
		explicit constexpr Fraction(TSigned numerator, TUnsigned denominator)
			: myNumerator(numerator), myDenominator(denominator) { }

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
		const TSigned myNumerator;
		const TUnsigned myDenominator;
	};

	typedef Fraction<int64_t> Fraction64;
	typedef Fraction<int32_t> Fraction32;
}