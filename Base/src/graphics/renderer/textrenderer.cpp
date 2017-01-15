
#include "textrenderer.hpp"
#include "graphics/opengl/opengl.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shader/shaders.hpp"
#include "graphics/texture/texturehandler.hpp"
#include "graphics/text/font.hpp"

void getColorByCode(char c, sa::vec4<float>& color) {
    switch (c) {
    case 'w': color = Color::WHITE;
    case 's': color = Color::BLACK;
    case 'g': color = Color::GREEN;
    case 'r': color = Color::RED;
    case 'b': color = Color::BLUE;
    case 'c': color = Color::CYAN;
    case 'y': color = Color::YELLOW;
    case 'o': color = Color::ORANGE;
    case 'R': color = Color::DARK_RED;
    case 'x': color = Color::GREY;
    default:
        color = Color::WHITE;
    }
}

const vec4<float>& getColorByCode(char c) {
	switch (c) {
	case 'w': return Color::WHITE;
	case 's': return Color::BLACK;
	case 'g': return Color::GREEN;
	case 'r': return Color::RED;
	case 'b': return Color::BLUE;
	case 'c': return Color::CYAN;
	case 'y': return Color::YELLOW;
	case 'o': return Color::ORANGE;
	case 'R': return Color::DARK_RED;
	case 'x': return Color::GREY;
	default:
		return Color::WHITE;
	}
}

sa::TextRenderer::TextRenderer(std::shared_ptr<sa::Shaders> shaders, std::shared_ptr<sa::Camera> camera) :
m_shaders(shaders),
m_vertexBuffer(6 * 2 * MAX_TEXT_LENGTH),
m_texCoordBuffer(6 * 2 * MAX_TEXT_LENGTH),
m_colorBuffer(6 * 4 * MAX_TEXT_LENGTH)
{
	auto fontShader = m_shaders->loadShader("font", "../shaders/font_130.vs.glsl", "../shaders/font_130.fs.glsl");
	m_shaders->switchToShader("font");

	m_pCamera = camera;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	int vposIndex = fontShader->attribute("vertexPosition");
	int vcolIndex = fontShader->attribute("vertexColor");
	int vtexIndex = fontShader->attribute("textureCoordinate");

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.max_size() * sizeof(float), NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(vposIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vposIndex);

	glGenBuffers(1, &cbo);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, m_colorBuffer.max_size() * sizeof(float), NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(vcolIndex, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vcolIndex);

	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, m_texCoordBuffer.max_size() * sizeof(float), NULL, GL_STREAM_DRAW);
	glVertexAttribPointer(vtexIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vtexIndex);

	glBindVertexArray(0);
}



void sa::TextRenderer::drawText(const std::string& text, float x, float y, float lineHeight, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom) {
	drawText(text, x, y, lineHeight, Color::WHITE, font, custom);
}

void sa::TextRenderer::drawText(const std::string& text, float x, float y, float lineHeight, const vec4<float>& color, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom) {
	drawText(text, x, y, lineHeight, color, Align::CENTER, font, custom);
}

void sa::TextRenderer::drawText(const std::string& text, float x, float y, float lineHeight, const vec4<float>& color, Align align, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom) {
	activeColor = color;
	ASSERT(text.length() < MAX_TEXT_LENGTH, "too long text to render!");
	if (text.length() >= MAX_TEXT_LENGTH) {
		return;
	}

	sa::TextureHandler::getSingleton().bindTexture(0, font.getTextureID());
	int length = fillTextBuffers(text, font, x, y, lineHeight, lineHeight, align, custom);
	drawTextBuffers(length);
}



void sa::TextRenderer::drawTextBuffers(int textLength) {
	int coordinatesPerVertex = 2;
	std::shared_ptr<sa::Shader> textShader = m_shaders->switchToShader("font");

	const Matrix4& projectionMatrix = m_pCamera->getProjection();
	const Matrix4& viewMatrix = m_pCamera->getView();

	projectionViewMatrix.storeMultiplication(projectionMatrix, viewMatrix);
	sa::ShaderMemory::setUniformMat4(*textShader.get(), "MVPMatrix", projectionViewMatrix);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* m_vertexBuffer.size(), &m_vertexBuffer[0]);

	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* m_colorBuffer.size(), &m_colorBuffer[0]);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)* m_texCoordBuffer.size(), &m_texCoordBuffer[0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 2 * 3 * textLength);
	glBindVertexArray(0);

	m_vertexBuffer.clear();
	m_texCoordBuffer.clear();
	m_colorBuffer.clear();
}

int sa::TextRenderer::fillTextBuffers(const std::string& text, const Font& font, float x, float y, float scaleX, float scaleY, Align align, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom) {
	int skippedCharacters = 0;
	int length = text.length();
	float textHeight = scaleY;
	float textWidth = font.getLength(text, scaleY);

	switch (align) {
	case CENTER:
		x -= textWidth * 0.5f;
		break;
	case LEFT:
		break;
	case RIGHT:
		x -= textWidth;
		break;
	}
	y -= textHeight * 0.5f;

	for (int i = 0; i < length; ++i) {
		char c = text[i];
		if (c == '^') {
			++i;
			skippedCharacters += 2;
			c = text[i];
			getColorByCode(c, activeColor);
			continue;
		}

		custom(text, i, x, y, scaleX, scaleY, activeColor);

		float width = font.width(c, scaleX);
		float height = font.height(c, scaleY);
		float advance = font.advance(c, scaleX);
		float offsetX = font.offsetX(c, scaleX);
		float offsetY = font.offsetY(c, scaleY);

		fillCoordinates(x + offsetX, y + offsetY, width, height);
		fillColorBuffer(activeColor);
		fillTextureCoordinates(font, c);

		// TODO: Add kerning to advance here.

		x += advance;
	}

	return length - skippedCharacters;
}


void sa::TextRenderer::fillTextureCoordinates(const Font& font, char c) {
	const vec4<float>& textureCoordinates = font.getTextureCoordinates(c);

	m_texCoordBuffer.push_back(textureCoordinates.left); m_texCoordBuffer.push_back(textureCoordinates.top);
	m_texCoordBuffer.push_back(textureCoordinates.right); m_texCoordBuffer.push_back(textureCoordinates.top);
	m_texCoordBuffer.push_back(textureCoordinates.right); m_texCoordBuffer.push_back(textureCoordinates.bottom);

	m_texCoordBuffer.push_back(textureCoordinates.right); m_texCoordBuffer.push_back(textureCoordinates.bottom);
	m_texCoordBuffer.push_back(textureCoordinates.left); m_texCoordBuffer.push_back(textureCoordinates.bottom);
	m_texCoordBuffer.push_back(textureCoordinates.left); m_texCoordBuffer.push_back(textureCoordinates.top);
}