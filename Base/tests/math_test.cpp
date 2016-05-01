
#include "test/testframework.hpp"

#include "util/fixedpoint.hpp"
#include "math/2d/math.hpp"
#include "math/2d/LineSegment.hpp"
#include "util/vec3.hpp"

float sqr(float val) {
	return val * val;
}

TEST(FixedPoint, TrigonometryCasual)
{
	sa::math::fixedpoint::initialize();

	{
		for (int i = 0; i < 1000; ++i) {
			FixedPoint<> fp_value(i, 100);
			float f_value(i / 100.0f);

			float fp_sin = sa::math::sin(fp_value);
			float f_sin = sa::math::sin(f_value);
			EXPECT_TRUE(sqr(fp_sin - f_sin) < 0.001f);
		}
	}
}

TEST(FixedPoint, TrigonometryExtreme)
{
	sa::math::fixedpoint::initialize();

	// Accuracy decreases when going further away from zero, since the fixed point PI value is not very accurate.
	{
		FixedPoint<> fp_value(-10000);
		float f_value(-10000);

		float fp_sin = sa::math::sin(fp_value);
		float f_sin = sa::math::sin(f_value);
		EXPECT_TRUE(sqr(fp_sin - f_sin) < 0.001f);
	}

	{
		FixedPoint<> fp_value(+10000);
		float f_value(+10000);

		float fp_sin = sa::math::sin(fp_value);
		float f_sin = sa::math::sin(f_value);
		EXPECT_TRUE(sqr(fp_sin - f_sin) < 0.001f);
	}


	// At this point, accuracy is no longer expected.
	{
		FixedPoint<> fp_value(+100000);
		float f_value(+100000);

		float fp_sin = sa::math::sin(fp_value);
		float f_sin = sa::math::sin(f_value);
		EXPECT_FALSE(sqr(fp_sin - f_sin) < 0.001f);
	}
}

TEST(LineSegment, Intersect)
{
	sa::LineSegment<sa::vec3<float>> seg_1(sa::vec3<float>(0, 0, 0), sa::vec3<float>(0, 3, 0));
	sa::LineSegment<sa::vec3<float>> seg_2(sa::vec3<float>(1, 1, 0), sa::vec3<float>(-1, 1, 0));
	sa::LineSegment<sa::vec3<float>> seg_3(sa::vec3<float>(10, 10, 0), sa::vec3<float>(-10, 10, 0));

	EXPECT_TRUE(seg_1.intersects(seg_2));
	EXPECT_FALSE(seg_1.intersects(seg_3));
}


TEST(FixedPoint, Conversions)
{
	FixedPoint<1024> a(1, 2);
	FixedPoint<2048> b(1, 2);
	FixedPoint<512> c(1, 2);

	EXPECT_FALSE(a != b);
	EXPECT_FALSE(a != c);
	EXPECT_TRUE(a + b == FixedPoint<>(2, 2));
	EXPECT_TRUE(a * c == FixedPoint<>(1, 4));
	EXPECT_TRUE(a * c == b * c);
}
