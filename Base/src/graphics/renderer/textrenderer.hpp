
#pragma once

#include "util/vec4.hpp"
#include "math/matrix/matrix4.hpp"
#include "graphics/color.hpp"
#include "util/dynarray.hpp"

#include <memory>
#include <functional>

typedef int GLint;
typedef unsigned GLuint;

namespace sa {
	class Camera;
	class Shaders;
	class Font;
}

namespace sa {

	class TextRenderer {

		static const int MAX_TEXT_LENGTH = 1024;

		std::dynarray<float> m_vertexBuffer;
		std::dynarray<float> m_texCoordBuffer;
		std::dynarray<float> m_colorBuffer;

		sa::Matrix4 projectionViewMatrix;

		vec4<float> activeColor;
		std::shared_ptr<sa::Shaders> m_shaders;
		std::shared_ptr<sa::Camera> m_pCamera;
		GLuint vao;
		GLuint vbo, cbo, tbo;

	public:

		enum Align {
			LEFT,
			CENTER,
			RIGHT
		};

		TextRenderer(std::shared_ptr<sa::Shaders> shaders, std::shared_ptr<sa::Camera> camera);

        void drawText(const std::string& text, float x, float y, float lineHeight, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom = [](const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&){});
        void drawText(const std::string& text, float x, float y, float lineHeight, const vec4<float>& color, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom = [](const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&){});
        void drawText(const std::string& text, float x, float y, float lineHeight, const vec4<float>& color, Align align, const Font& font, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom = [](const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&){});

		void drawTextBuffers(int textLength);
        int fillTextBuffers(const std::string& text, const Font& font, float x, float y, float scaleX, float scaleY, Align align, std::function<void(const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&)> custom = [](const std::string&, int, float&, float&, float&, float&, sa::vec4<float>&){});

		void fillColorBuffer(const vec4<float>& activeColor) {
			for (int i = 0; i < 6; ++i) {
				m_colorBuffer.push_back(activeColor.r);
				m_colorBuffer.push_back(activeColor.g);
				m_colorBuffer.push_back(activeColor.b);
				m_colorBuffer.push_back(activeColor.a);
			}
		}

		void fillCoordinates(float x, float y, float charWidth, float charHeight) {
			m_vertexBuffer.push_back(x); m_vertexBuffer.push_back(y);
			m_vertexBuffer.push_back(x + charWidth); m_vertexBuffer.push_back(y);
			m_vertexBuffer.push_back(x + charWidth); m_vertexBuffer.push_back(y + charHeight);

			m_vertexBuffer.push_back(x + charWidth); m_vertexBuffer.push_back(y + charHeight);
			m_vertexBuffer.push_back(x); m_vertexBuffer.push_back(y + charHeight);
			m_vertexBuffer.push_back(x); m_vertexBuffer.push_back(y);
		}

		void fillTextureCoordinates(const Font& font, char c);
	};

}
