#pragma once

#include "util/logging.hpp"
#include "util/vec3.hpp"
#include "FixedMath.h"

#ifdef BATTLESIM_DLL
#define BATTLESIM_API __declspec(dllimport)
#else
#define BATTLESIM_API __declspec(dllexport)
#endif

#define BATTLESIM_FIXED_POINT

namespace bs
{
	using Fraction = core::Fraction32;
#ifdef BATTLESIM_FIXED_POINT
	using Fixed32 = core::Fixed32;
	typedef Fixed32 Real;
#else
	typedef float Real;
#endif
	typedef ::sa::vec3<Real> Vec;
	typedef uint64_t U64;
	typedef uint32_t U32;
	typedef uint16_t U16;
	typedef uint8_t U8;
	typedef int16_t I16;

	typedef U16 TimerId;
	static const TimerId InvalidTimer = static_cast<TimerId>(~0);

	typedef U16 Tick;
	static const Tick InvalidTick = static_cast<Tick>(~0);

	struct BoundingBox
	{
		U32 left;
		U32 top;
		U32 right;
		U32 bottom;
		bool operator!=(const BoundingBox& other)
		{
			return left != other.left || right != other.right || top != other.top || bottom != other.bottom;
		}
	};
}

#ifdef BATTLESIM_FIXED_POINT
namespace sa
{
	namespace math
	{
		inline bs::Real sqrt(const bs::Real& val)
		{
			return core::sqrt(val);
		}

		using core::abs;
		using core::cos;
		using core::sin;
		using core::round;
	}
}
#endif