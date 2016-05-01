
#include "Fiber.hpp"
#include "util/platform.hpp"

void* Fiber::getFiberAddress() {
	return m_fiber;
}

void Fiber::go(std::function<void(Fiber*)> func) {
	m_func = func;
	resume();
}

void Fiber::resume() {
#if PLATFORM_WIN
	m_callingFiber = Fiber::getCurrentPlatformFiber();
#endif
	Fiber::switchToFiber(m_callingFiber, m_fiber);
}

void Fiber::yield() const {
	Fiber::switchToFiber(m_fiber, m_callingFiber);
}
