
#include "Fraction.h"

namespace core
{
	class Math
	{
	public:
		// http://www.math.illinois.edu/~ajh/453/pi-cf.pdf
		template<typename T = int32_t>
		static constexpr inline Fraction<T> Pi() { return core::Fraction32(80143857, 25510582); }
		static constexpr inline Fraction<int64_t> Pi() { return core::Fraction64(80143857, 25510582); }
		
		template<typename T>
		static constexpr inline T WrappedDelta(T from, T to, T range)
		{
			auto delta = from - to;
			if (delta > T(0))
			{
				if (delta <= range)
				{
					return delta;
				}
				else
				{
					return -(range * T(2) - delta);
				}
			}
			else
			{
				if (delta >= -range)
				{
					return delta;
				}
				else
				{
					return (range * T(2) + delta);
				}
			}
		}

	};

}