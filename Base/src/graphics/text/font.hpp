
#pragma once

#include "graphics/text/fontdata.hpp"

namespace sa {

	class Font {

		FontData fontData;

	public:

		Font(const FontData& fontData) {
			this->fontData = fontData;
			this->fontData.fillTextureCoordinates();
		}

		float totalWidth(const std::string& text) const {
			float sum = 0.0f;
			for (unsigned i = 0; i < text.length(); ++i) {
				char c = text[i];
				if (c == '^') {
					++i;
					continue;
				}
				sum += fontData[c].advanceX * 1.0f / fontData.lineHeight;
			}
			return sum;
		}

		/**
		 * Returns the width of a text in world space units.
		 *
		 * @param text The text.
		 * @param lineHeight Height of a line in world space units.
		 * @return Width of the text in world space units.
		 */
		float getLength(const std::string& text, float lineHeight) const {
			float totalWidth = this->totalWidth(text);
			return totalWidth * lineHeight;
		}

		sa::vec4<float> getTextureCoordinates(char c) const {
			return fontData.textureCoordinates[c];
		}

		/**
		 * Returns the width of a character in world space units.
		 *
		 * @param character The character.
		 * @param lineHeight The height of a line in world space units.
		 * @return Width the character in world space units.
		 */
		float width(char character, float lineHeight) const {
			return fontData.glyphData[character].width * 1.0f / fontData.lineHeight * lineHeight;
		}

		float height(char c, float lineHeight) const {
			return fontData.glyphData[c].height * 1.0f / fontData.lineHeight * lineHeight;
		}

		float advance(char c, float lineHeight) const {
			return fontData.glyphData[c].advanceX * 1.0f / fontData.lineHeight * lineHeight;
		}

		float offsetX(char c, float lineHeight) const {
			return fontData.glyphData[c].offsetX * 1.0f / fontData.lineHeight * lineHeight;
		}

		float offsetY(char c, float lineHeight) const {
			float offsetY = fontData.lineHeight - fontData.glyphData[c].height - fontData.glyphData[c].offsetY;
			return offsetY * 1.0f / fontData.lineHeight * lineHeight;
		}

		const std::string& getTextureID() const {
			return fontData.textureID;
		}
	};

}