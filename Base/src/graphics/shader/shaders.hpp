
#pragma once

#include "graphics/shader/shader.hpp"

#include <unordered_map>
#include <string>
#include <memory>

typedef int GLint;
typedef unsigned GLuint;

namespace sa {

class Shaders
{
public:
	Shaders();
	~Shaders();

	void init();
	void release();

	std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader);
	std::shared_ptr<Shader> loadShader(const std::string& name, const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader, GLuint geometryInputType, GLuint geometryOutputType, GLuint geometryOutputVertices);

	std::shared_ptr<Shader> switchToShader(const std::string& name);

	GLuint operator[](const std::string& program_name) const;
	std::shared_ptr<Shader> operator()(const std::string& program_name);

private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::shared_ptr<Shader> m_activeShader;
	std::string m_activeShaderName;
};

}