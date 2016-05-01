
#pragma once

#include "graphics/text/glyph.hpp"
#include "util/vec4.hpp"
#include "util/logging.hpp"

#include <string>


class FontData {
public:
    std::string textureID;
    int totalWidth;
    int totalHeight;
    int lineHeight;
    int baseLine;

    Glyph glyphData[256];
    sa::vec4<float> textureCoordinates[256];

    FontData() {
        // Derived class should set all variables in its constructor.
    }

    void fillTextureCoordinates() {
        for(Glyph glyph : glyphData) {
			if(glyph.id == -1) {
                continue;
            }

            int c = glyph.id;

            float x = glyph.x;
            float y = glyph.y;
            float width = glyph.width;
            float height = glyph.height;

            float left = x * 1.0f / totalWidth;
            float right = (x + width) * 1.0f / totalWidth;
            float bottom = y * 1.0f / totalHeight;
            float top = (y + height) * 1.0f / totalHeight;

			textureCoordinates[c] = sa::vec4<float>(left, right, 1.0f - top, 1.0f - bottom);
        }
    }

	const Glyph& operator [](int index) const {
		ASSERT(index >= 0 && index < 256, "index out of bounds");
		return glyphData[index];
	}

	Glyph& operator [](int index) {
		ASSERT(index >= 0 && index < 256, "index out of bounds");
		return glyphData[index];
	}
};

