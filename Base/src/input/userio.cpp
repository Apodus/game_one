
#include "userio.hpp"
#include "util/logging.hpp"

#include <iostream>
#include <functional>
#include <memory>
#include "graphics/window/window.hpp"

#include <GLFW/glfw3.h>

namespace {
	std::function<void(int, int, int, int)> g_keyboardKeyEventHandler;
	std::function<void(double, double)> g_mouseScrollEventHandler;
	std::function<void(double, double)> g_mouseMoveEventHandler;
	std::function<void(int, int, int)> g_mouseKeyEventHandler;
	std::function<void(int)> g_mouseEnteredHandler;
}

sa::UserIO::UserIO(std::shared_ptr<sa::Window> window)
{
	ASSERT(!g_keyboardKeyEventHandler, "Multiple UserIO initialisations not allowed :( Never fix");
	
	for(auto& buttonState : m_buttonStates)
		buttonState = 0;

	m_window = window;
	m_mouseInScreen = true;

	g_keyboardKeyEventHandler = std::bind(&UserIO::onKeyEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	g_mouseScrollEventHandler = std::bind(&UserIO::onMouseScrollEvent, this, std::placeholders::_1, std::placeholders::_2);
	g_mouseMoveEventHandler = std::bind(&UserIO::onMouseMoveEvent, this, std::placeholders::_1, std::placeholders::_2);
	g_mouseKeyEventHandler = std::bind(&UserIO::onMouseButtonEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	g_mouseEnteredHandler = std::bind(&UserIO::onMouseEnterEvent, this, std::placeholders::_1);

	glfwSetKeyCallback(window->getGLFWwindow(), keyCallbackDummy);
	glfwSetScrollCallback(window->getGLFWwindow(), scrollCallbackDummy);
	glfwSetCursorPosCallback(window->getGLFWwindow(), cursorPosCallbackDummy);
	glfwSetMouseButtonCallback(window->getGLFWwindow(), mouseButtonCallbackDummy);
	glfwSetCursorEnterCallback(window->getGLFWwindow(), cursorEnterCallbackDummy);
}

sa::UserIO::~UserIO() {
	g_keyboardKeyEventHandler = nullptr;
	g_mouseScrollEventHandler = nullptr;
	g_mouseMoveEventHandler = nullptr;
	g_mouseKeyEventHandler = nullptr;
	g_mouseEnteredHandler = nullptr;
}

#if PLATFORM_WIN
#pragma warning (disable : 4800) // forcing int value to boolean
#endif

bool sa::UserIO::isKeyClicked(int key) const {
	return m_buttonStates[key] & KEY_CLICK;
}

bool sa::UserIO::isKeyPressed(int key) const {
	return m_buttonStates[key] & KEY_PRESSED;
}

bool sa::UserIO::isKeyDown(int key) const {
	return m_buttonStates[key] & KEY_DOWN;
}

bool sa::UserIO::isKeyRepeat(int key) const {
	return m_buttonStates[key] & KEY_REPEAT;
}

bool sa::UserIO::isKeyReleased(int key) const {
	return m_buttonStates[key] & KEY_RELEASED;
}

bool sa::UserIO::isKeyConsumed(int key) const {
	return m_buttonStates[key] & KEY_CONSUMED;
}

void sa::UserIO::consume(int key) {
	m_buttonStates[key] |= KEY_CONSUMED;
}


int sa::UserIO::getAnyClicked() {
	for(unsigned index = 0; index < m_buttonStates.size(); ++index) {
		if(m_buttonStates[index] & KEY_PRESSED) {
			return index;
		}
	}
	return 0;
}

int sa::UserIO::getAnyReleased() {
	for(unsigned index = 0; index < m_buttonStates.size(); ++index) {
		if(m_buttonStates[index] & KEY_RELEASED) {
			return index;
		}
	}
	return 0;
}

float sa::UserIO::getMouseScroll() const {
	return m_mouseScroll;
}

#if PLATFORM_WIN
#pragma warning (default : 4800) // forcing int value to boolean
#endif

void sa::UserIO::update() {
	for(uint8_t& keyState : m_buttonStates) {
		keyState &= ~(KEY_PRESSED | KEY_RELEASED | KEY_REPEAT | KEY_CLICK | KEY_CONSUMED);
	}
	m_mouseScroll = 0;
}

void sa::UserIO::onKeyEvent(int key, int scancode, int action, int mods) {
	if (key < 0)
		return;
	if (action == GLFW_PRESS) {
		m_buttonStates[key] |= KEY_PRESSED | KEY_DOWN;
	}
	else if (action == GLFW_RELEASE) {
		m_buttonStates[key] = KEY_RELEASED | KEY_CLICK;
	}
	else if (action == GLFW_REPEAT) {
		m_buttonStates[key] |= KEY_REPEAT;
	}
}

void sa::UserIO::onMouseButtonEvent(int key, int action, int mods) {
	if (action == GLFW_PRESS) {
		m_buttonStates[key + 256] |= KEY_PRESSED | KEY_DOWN;
		m_mousePosition_clickBegin = m_mousePosition;
	}
	if (action == GLFW_RELEASE) {
		uint8_t value = KEY_RELEASED;
		if ((m_mousePosition - m_mousePosition_clickBegin).lengthSquared() < 0.02f * 0.02f)
		{
			value |= KEY_CLICK;
		}

		m_buttonStates[key + 256] = value;
	}
}

void sa::UserIO::onMouseScrollEvent(double xoffset, double yoffset) {
	m_mouseScroll = static_cast<float>(yoffset);
}

void sa::UserIO::onMouseMoveEvent(double xpos, double ypos) {
	m_mousePosition.x = 2.0f * static_cast<float>(xpos) / m_window->width() - 1.0f;
	m_mousePosition.y = 1.0f - 2.0f * static_cast<float>(ypos) / m_window->height();
}

void sa::UserIO::onMouseEnterEvent(int entered) {
	if (entered == GL_TRUE) {
		m_mouseInScreen = true;
	}
	if (entered == GL_FALSE) {
		m_mouseInScreen = false;
	}
}





void sa::UserIO::keyCallbackDummy(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(g_keyboardKeyEventHandler)
		g_keyboardKeyEventHandler(key, scancode, action, mods);
}

void sa::UserIO::mouseButtonCallbackDummy(GLFWwindow* window, int button, int action, int mods) 
{
	if(g_mouseKeyEventHandler)
		g_mouseKeyEventHandler(button, action, mods);
}

void sa::UserIO::scrollCallbackDummy(GLFWwindow* window, double xoffset, double yoffset)
{
	if(g_mouseScrollEventHandler)
		g_mouseScrollEventHandler(xoffset, yoffset);
}

void sa::UserIO::cursorPosCallbackDummy(GLFWwindow* window, double xpos, double ypos)
{
	if(g_mouseMoveEventHandler)
		g_mouseMoveEventHandler(xpos, ypos);
}

void sa::UserIO::cursorEnterCallbackDummy(GLFWwindow* window, int entered) 
{
	if(g_mouseEnteredHandler)
		g_mouseEnteredHandler(entered);
}

