#pragma once

#include "util/platform.hpp"

extern "C" {
#include <GL/glew.h>
}

#if PLATFORM_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#elif PLATFORM_LINUX || PLATFORM_WIN
#include <GL/gl.h>
#include <GL/glu.h>
#else
#error Platform not handled :(
#endif

#ifdef APIENTRY // winapi definition collides with glfw3 definition. they both define it as __stdcall
#undef APIENTRY
#endif

#include <GLFW/glfw3.h> // must be after glew and glu

class OpenGL
{
public:
	OpenGL();
};
