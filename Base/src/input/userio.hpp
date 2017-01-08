
#pragma once

#include <array>
#include <string>
#include <cstdlib>
#include <memory>

#include "util/vec3.hpp"

struct GLFWwindow;

namespace sa {

class Window;

class UserIO
{
	void onKeyEvent(int key, int scancode, int action, int mods);
	void onMouseButtonEvent(int key, int action, int mods);
	void onMouseScrollEvent(double xoffset, double yoffset);
	void onMouseMoveEvent(double xpos, double ypos);
	void onMouseEnterEvent(int entered);

	std::array<int, 512> m_buttonStates;
	sa::vec3<float> m_mousePosition;
	std::shared_ptr<Window> m_window;
	float m_mouseScroll = 0;
	bool m_mouseInScreen = true;

	enum {
		KEY_CLICKED = 2,
		KEY_DOWN = 1,
		KEY_REPEAT = 4,
		KEY_RELEASED = 8
	};

public:
	UserIO(std::shared_ptr<Window> window);
	~UserIO();

	inline int getMouseKeyCode(int mouseButton) const {
		return mouseButton + 256;
	}

	template<class T>
	inline void getCursorPosition(T& t) const {
		t.x = m_mousePosition.x;
		t.y = m_mousePosition.y;
	}

	sa::vec3<float> getCursorPosition() const
	{
		return m_mousePosition;
	}

	bool isKeyClicked(int key);
	bool isKeyDown(int key);
	bool isKeyRepeat(int key);
	bool isKeyReleased(int key);
	float getMouseScroll() const;

	int getAnyClicked();
	int getAnyReleased();

	void update();

	static void keyCallbackDummy(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallbackDummy(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallbackDummy(GLFWwindow* window, double xoffset, double yoffset);
	static void cursorPosCallbackDummy(GLFWwindow* window, double xpos, double ypos);
	static void cursorEnterCallbackDummy(GLFWwindow* window, int entered);
};

}