#pragma once

#include <cstddef>
#include <memory>

struct GLFWwindow;

namespace sa {

class Window
{
public:
	Window();
	~Window();

	size_t width() const;
	size_t height() const;

	void toggle_fullscreen();
	void swap_buffers() const;

	void createWindow(int, int);
	
	void enable_grab() const;
	void disable_grab() const;

	void requestExit();
	bool shouldClose() const;
	void pollEvents() const;

	bool active() const;
	void hide() const;

	inline float getAspectRatio() const {
		return m_aspectRatio;
	}

	GLFWwindow* getGLFWwindow() const;

private:
	Window& operator=(const Window&); // Disabled.
	Window(const Window&); // Disabled.
	
  GLFWwindow* createWindow();
  void onResize(int width, int height);

	size_t m_width;
	size_t m_height;

	bool m_fullscreen;
	bool m_userRequestedExit;

  float m_aspectRatio;

	GLFWwindow* m_pWindow;
};

}