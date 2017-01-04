
#include "window.hpp"
#include "util/platform.hpp"

#if PLATFORM_WIN
#include <windows.h>
#endif

#include "graphics/opengl/opengl.hpp"
#include "util/logging.hpp"
#include "messagesystem/TypedMessage.hpp"
#include "graphics/window/messages.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <stdexcept>

using namespace std;

sa::Window::Window()
{
	m_fullscreen = false;
	m_userRequestedExit = false;
	m_pWindow = nullptr;
}

sa::Window::~Window()
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
	m_pWindow = nullptr;
}

std::map<GLFWwindow*, std::function<void(int, int)>> g_resizeEventMapper;

void global_scope_windowResizeCallback(GLFWwindow* window, int width, int height)
{
	g_resizeEventMapper[window](width, height);
}

void sa::Window::onResize(int width, int height)
{
	m_width = width;
	m_height = height;
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	LOG("aspectRatio: %f", m_aspectRatio);

	glViewport(0, 0, width, height);
	sendMessage(AspectRatioChanged(m_aspectRatio));
}

GLFWwindow* sa::Window::createWindow()
{
	GLFWmonitor* monitor = (m_fullscreen) ? glfwGetPrimaryMonitor() : nullptr;
	GLFWwindow* window = glfwCreateWindow(m_width, m_height, "SoulAim Game", monitor, nullptr);

	if (!window) {
		glfwTerminate();
		LOG("ERROR: GLFW window creation failed.");
		throw std::runtime_error("Failed to create GLFW window");
	}

	m_pWindow = window;
	g_resizeEventMapper[m_pWindow] = std::bind(&sa::Window::onResize, this, std::placeholders::_1, std::placeholders::_2);
	glfwSetWindowSizeCallback(window, global_scope_windowResizeCallback);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	onResize(width, height);
	return window;
}

void sa::Window::createWindow(int width, int height)
{
	ASSERT(width > 10, "width out of bounds");
	ASSERT(width <= 4096, "width out of bounds");
	ASSERT(height > 10, "height out of bounds");
	ASSERT(height <= 4096, "height out of bounds");

	m_width = width;
	m_height = height;

	// Initialize GLFW
	if (!glfwInit()) {
		LOG("ERROR: GLFW init failed");
		throw std::string("Failed to init GLFW");
	}

	// Window hints
	glfwWindowHint(GLFW_SAMPLES, 4); // FSAA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Min OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_pWindow = createWindow();

	// Make the window's context current
	glfwMakeContextCurrent(m_pWindow);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		LOG("ERROR: GLEW init failed.");
		throw std::string("Failed to init GLEW");
	}
}

size_t sa::Window::width() const
{
	return m_width;
}

size_t sa::Window::height() const
{
	return m_height;
}

void sa::Window::toggle_fullscreen()
{
	m_fullscreen = !m_fullscreen;
	m_pWindow = createWindow();
}

void sa::Window::swap_buffers() const
{
	glfwSwapBuffers(m_pWindow);
}

std::string next_screenshot_filename()
{
	static int next = 0;

	string filename;
	do
	{
		stringstream ss;
		ss << "screenshots/screenshot" << setw(3) << setfill('0') << next << ".bmp";
		filename = ss.str();
		++next;
	} while (ifstream(filename));

	return filename;
}

void sa::Window::enable_grab() const
{
	// TODO: Confine mouse cursor within window region
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void sa::Window::disable_grab() const
{
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool sa::Window::shouldClose() const
{
	return (glfwWindowShouldClose(m_pWindow) == 1) || m_userRequestedExit;
}

void sa::Window::pollEvents() const
{
	glfwPollEvents();
}

bool sa::Window::active() const
{
	return (glfwGetWindowAttrib(m_pWindow, GLFW_FOCUSED) != 0);
}

void sa::Window::hide() const
{
	disable_grab();
	glfwIconifyWindow(m_pWindow);
}

void sa::Window::requestExit()
{
	m_userRequestedExit = true;
}

GLFWwindow* sa::Window::getGLFWwindow() const
{
	return m_pWindow;
}

