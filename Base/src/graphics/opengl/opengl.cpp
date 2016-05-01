
#include "graphics/opengl/opengl.hpp"

#include "util/logging.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>

OpenGL::OpenGL()
{
	GLenum status = glewInit();
	if(status == GLEW_OK)
	{
		LOG("GLEW Init OK");
	}
	else
	{
		std::stringstream ss;
		LOG("GLEW initialization failed: '%s'", glewGetErrorString(status));
		throw std::runtime_error(ss.str());
	}

	if(!GLEW_VERSION_3_0)
	{
		throw std::runtime_error("Hardware does not support OpenGL 3.0");
	}
}

