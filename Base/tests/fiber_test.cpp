#include "test/testframework.hpp"

#include "thread/fiber/Fiber.hpp"

TEST(FiberTests, FiberBasics)
{
	CheckMemory memory;
	{
		Fiber* mainFiber = Fiber::enterFiberMode();
		int value = 0;
		
		{
			Fiber fiber;

			EXPECT_TRUE(value == 0);
			++value;

			fiber.go(
				[&value](Fiber* pFiber) {
				
				EXPECT_TRUE(value == 1);
				++value;
				
				pFiber->yield();
				
				EXPECT_TRUE(value == 3);
				++value;
			}
			);

			EXPECT_TRUE(value == 2);
			++value;

			fiber.resume();

			EXPECT_TRUE(value == 4);
		}

		Fiber::exitFiberMode(mainFiber);
	}
}




TEST(FiberTests, FiberInFiber)
{
	CheckMemory memory;
	{
		Fiber* mainFiber = Fiber::enterFiberMode();
		
		bool inFirst = false;
		bool inSecond = false;

		{
			Fiber fiber;

			fiber.go(
				[&inFirst, &inSecond](Fiber* pFiber) {

				EXPECT_FALSE(inFirst);
				EXPECT_FALSE(inSecond);
				
				inFirst = true;

				Fiber fiber;

				fiber.go(
					[&inFirst, &inSecond](Fiber* pFiber) {
						inSecond = true;
						pFiber->yield();
						inSecond = false;
					}
				);

				EXPECT_TRUE(inFirst);
				EXPECT_TRUE(inSecond);
				
				fiber.resume();

				EXPECT_TRUE(inFirst);
				EXPECT_FALSE(inSecond);

				pFiber->yield();

				EXPECT_FALSE(inFirst);
				EXPECT_FALSE(inSecond);
			}
			);

			inFirst = false;
			fiber.resume();
		}

		Fiber::exitFiberMode(mainFiber);
	}
}