#pragma once

#include "graphics/shader/shaders.hpp"
#include "graphics/texture/texturehandler.hpp"
#include "graphics/camera/Camera.hpp"
#include "math/matrix/matrix4.hpp"

#include <memory>

namespace sa {

class Camera;
class Mesh;

class MeshRenderer {
	std::unique_ptr<sa::Mesh> m_rectangle;
	std::unique_ptr<sa::Mesh> m_line;

	std::shared_ptr<sa::Shaders> m_shaders;
	std::shared_ptr<sa::Camera> m_pCamera;

	GLuint m_modelUniform, m_viewUniform, m_projectionUniform;
	GLuint m_texSamplerUniform, m_colorUniform;

public:
	MeshRenderer(std::shared_ptr<sa::Shaders> shaders, std::shared_ptr<sa::Camera> camera);
	~MeshRenderer();

	void setDepthTest(bool depthTestEnabled);

	void clearScreen();
	void cameraToGPU();
	void setCamera(std::shared_ptr<sa::Camera> camera);
	void drawLine(const sa::vec3<float>& p1, const sa::vec3<float>& p2, float width, const sa::vec4<float>& color);
	void drawLine(const sa::vec3<float>& p1, const sa::vec3<float>& p2, const sa::Matrix4& model, float width, const sa::vec4<float>& color);
	void drawRectangle(const sa::Matrix4& model, const std::string& texture, const sa::vec4<float>& color = sa::vec4<float>(1, 1, 1, 1));
	void drawMesh(const sa::Mesh& mesh, const sa::Matrix4& model, const std::string& texture, const sa::vec4<float>& color = sa::vec4<float>(1, 1, 1, 1));

private:
	void init();
};

}