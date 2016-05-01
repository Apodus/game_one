
#include "graphics/shader/shaders.hpp"
#include "graphics/opengl/opengl.hpp"
#include "util/logging.hpp"

#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;

sa::Shaders::Shaders() {
	GLint n;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &n);
	LOG("Max geometry shader output vertices: %d", n);
	glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &n);
	LOG("Max geometry shader output components: %d", n);
}

sa::Shaders::~Shaders() {
	release();
}

void sa::Shaders::release()
{
	m_shaders.clear();
}

std::shared_ptr<sa::Shader> sa::Shaders::switchToShader(const std::string& name) {
	if(m_activeShaderName == name)
		return m_activeShader;

	if(m_activeShader)
		m_activeShader->stop();

	auto it = m_shaders.find(name);
	ASSERT(it != m_shaders.end(), "trying to start a shader that has not been loaded: '%s'", name.c_str());
	m_activeShader = it->second;
	m_activeShader->start();
	m_activeShaderName = name;
	return m_activeShader;
}

std::shared_ptr<sa::Shader> sa::Shaders::loadShader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader) {
	LOG("loading shader %s", name.c_str());
	std::shared_ptr<Shader> pShader(new Shader(vertexShader, fragmentShader));
	m_shaders[name] = pShader;
	return pShader;
}

std::shared_ptr<sa::Shader> sa::Shaders::loadShader(const std::string& name, const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader, GLuint geometryInputType, GLuint geometryOutputType, GLuint geometryOutputVertices) {
	LOG("loading shader %s", name.c_str());
	std::shared_ptr<Shader> pShader(new Shader(vertexShader, geometryShader, fragmentShader, geometryInputType, geometryOutputType, geometryOutputVertices));
	m_shaders[name] = pShader;
	return pShader;
}

GLuint sa::Shaders::operator[](const string& program_name) const
{
	auto it = m_shaders.find(program_name);
	ASSERT(it != m_shaders.end(), "shader program '%s' not found!", program_name.c_str());
	return it->second->get_program();
}

std::shared_ptr<sa::Shader> sa::Shaders::operator()(const std::string& program_name)
{
	auto it = m_shaders.find(program_name);
	ASSERT(it != m_shaders.end(), "shader program '%s' not found!", program_name.c_str());
	return it->second;
}

