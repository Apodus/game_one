
#include "util/platform.hpp"

#if PLATFORM_WIN

#include "../Fiber.hpp"
#include <windows.h>

Fiber::Fiber(void* platformFiber) {
	m_fiber = platformFiber;
	m_isThread = true;
}

Fiber::Fiber() {
	m_fiber = CreateFiber(0, Fiber::fiberStartingFunction, this);
	m_isThread = false;
}

Fiber::~Fiber() {
	if(!m_isThread) {
		DeleteFiber(m_fiber);
	}
	m_fiber = nullptr;
}

void Fiber::switchToFiber(void* fromPtr, void* toPtr) {
	SwitchToFiber(toPtr);
}

Fiber* Fiber::enterFiberMode() {
	return new Fiber(ConvertThreadToFiber(0));
}

void Fiber::exitFiberMode(Fiber* mainFiber) {
	if(!ConvertFiberToThread()) {
		// todo
	}

	delete mainFiber;
}

void* Fiber::getCurrentPlatformFiber() {
	return GetCurrentFiber();
}

void Fiber::fiberStartingFunction(void* data) {
	Fiber* fiber = reinterpret_cast<Fiber*>(data);
	while(true) {
		fiber->m_func(fiber);
		fiber->yield();
	}
}

#endif