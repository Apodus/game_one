
#include "Fraction.h"

namespace core
{
	class Math
	{
	public:
		// http://turner.faculty.swau.edu/mathematics/materialslibrary/pi/pirat.html
		template<typename T = int32_t>
		static constexpr inline Fraction<T> Pi() { return core::Fraction32(1146408, 364913); }	
		static constexpr inline Fraction<int64_t> Pi() { return core::Fraction64(3126535, 995207); }
		
		template<typename T = int32_t>
		static constexpr inline Fraction<T> Pi2() { return Pi<T>* Pi<T>; }

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