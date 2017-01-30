
#include "meshrenderer.hpp"
#include "graphics/opengl/opengl.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "math/2d/shape.hpp"
#include "graphics/camera/Camera.hpp"

sa::MeshRenderer::MeshRenderer(std::shared_ptr<sa::Shaders> shaders,
	std::shared_ptr<sa::Camera> camera)
{
	m_shaders = shaders;
	m_pCamera = camera;
	init();
}

sa::MeshRenderer::~MeshRenderer() {

}

void sa::MeshRenderer::init() {
	glClearColor(0.2f, 0.3f, 0.5f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDepthRange(0, 1);
	glClearDepth(1);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_rectangle = PolygonTesselator<vec3<float>>().tesselate(Shape::makeBox(2.f));
	m_rectangle->build();

	sa::vec4<float> uvLimits = sa::TextureHandler::getSingleton().textureLimits("Empty", vec4<float>(0.5f, 0.5f, 1.0f, 1.0f));
	m_line = PolygonTesselator<vec3<float>>().tesselate(Shape::makeBox(1.f), uvLimits);
	m_line->build();
	
	std::shared_ptr<Shader> shader2d = m_shaders->loadShader("renderer2d", "../shaders/2d_shader_130.vs.glsl", "../shaders/2d_shader_130.fs.glsl");
	m_shaders->switchToShader("renderer2d");

	m_modelUniform = shader2d->uniform("model");
	m_viewUniform  = shader2d->uniform("view");
	m_projectionUniform = shader2d->uniform("projection");
	m_colorUniform = shader2d->uniform("color");

	m_texSamplerUniform = shader2d->uniform("tex");
	glUniform1i(m_texSamplerUniform, 0);
}

void sa::MeshRenderer::setDepthTest(bool depthTestEnabled)
{
	if (depthTestEnabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void sa::MeshRenderer::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sa::MeshRenderer::setCamera(std::shared_ptr<sa::Camera> camera) {
	m_pCamera = camera;
}

void sa::MeshRenderer::drawLine(const sa::vec3<float>& p1, const sa::vec3<float>& p2, float width, const sa::vec4<float>& color) {
	sa::Matrix4 model_;
	sa::vec3<float> mid = (p1 + p2) * 0.5f;
	model_.makeTranslationMatrix(mid.x, mid.y, mid.z);
	model_.rotate(::atan((p1.y - p2.y) / (p1.x - p2.x)), 0, 0, 1);
	model_.scale((p1 - p2).length(), width, 1.0f);
	drawMesh(*m_line, model_, "Empty", color);
}

void sa::MeshRenderer::drawLine(const sa::vec3<float>& p1, const sa::vec3<float>& p2, const sa::Matrix4& model, float width, const sa::vec4<float>& color) {
	sa::Matrix4 model_;
	sa::vec3<float> mid = (p1 + p2) * 0.5f;
	model_.makeTranslationMatrix(mid.x, mid.y, mid.z);
	model_.rotate(::atan((p1.y - p2.y) / (p1.x - p2.x)), 0, 0, 1);
	model_.scale((p1 - p2).length(), width, 1.0f);
	model_ *= model;
	drawMesh(*m_line, model_, "Empty", color);
}

void sa::MeshRenderer::drawRectangle(const sa::Matrix4& model, const std::string& texture, const vec4<float>& color) {
	const vec4<float>& limits = TextureHandler::getSingleton().textureLimits(texture);
	float botCoordX = limits.data[0];
	float botCoordY = limits.data[1];
	float topCoordX = limits.data[0] + limits.data[2];
	float topCoordY = limits.data[1] + limits.data[3];

	m_rectangle->texCoords.clear();
	m_rectangle->putUVCoord(topCoordX, botCoordY);
	m_rectangle->putUVCoord(topCoordX, topCoordY);
	m_rectangle->putUVCoord(botCoordX, topCoordY);
	m_rectangle->putUVCoord(botCoordX, botCoordY);


	m_rectangle->bind();
	m_rectangle->rebuildTextureBuffer();
	
	drawMesh(*m_rectangle, model, texture, color);
}

void sa::MeshRenderer::cameraToGPU() {
	m_shaders->switchToShader("renderer2d");
	glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, m_pCamera->getView().data);
	glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, m_pCamera->getProjection().data);
}

void sa::MeshRenderer::drawMesh(const sa::Mesh& mesh, const sa::Matrix4& model, const std::string& texture, const sa::vec4<float>& color) {
	m_shaders->switchToShader("renderer2d");
	mesh.bind();
	sa::TextureHandler::getSingleton().bindTexture(0, texture);
	glUniformMatrix4fv(m_modelUniform, 1, GL_FALSE, model.data);
	glUniform4f(m_colorUniform, color.r, color.g, color.b, color.a);
	glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_SHORT, 0);
}