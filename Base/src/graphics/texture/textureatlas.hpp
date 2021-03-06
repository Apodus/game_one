
#pragma once

#include "util/vec4.hpp"
#include "util/logging.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace sa {

class TextureAtlas {

	std::string textureID;
	std::unordered_map<std::string, sa::vec4<float>> textureCoordinates;
	std::vector<std::string> textureIDs;

	float halfPixelWidth;
	float halfPixelHeight;

public:
	TextureAtlas(const std::string& realTexture, int pixelCountX, int pixelCountY) {
		textureID = realTexture;
		ASSERT(pixelCountX * pixelCountY > 0, "TextureAtlas pointing to bad texture name: %s", realTexture.c_str());
		halfPixelWidth = 1.0f / pixelCountX;
		halfPixelHeight = 1.0f / pixelCountY;
	}

	void insertTexture(const std::string& subTextureID, int slotX, int slotY, int slotCountX, int slotCountY) {
		vec4<float> texCoord;
		texCoord[0] = (slotX + 0.0f) / slotCountX + halfPixelWidth;  // x0
		texCoord[1] = 1.0f - ((slotY + 0.0f) / slotCountY + halfPixelHeight); // y0
		texCoord[2] = (slotX + 1.0f) / slotCountX - halfPixelWidth;  // x1
		texCoord[3] = 1.0f - ((slotY + 1.0f) / slotCountY - halfPixelHeight); // y1

		// Pre-compute subtexture size.
		texCoord[2] -= texCoord[0]; // x1 - x0
		texCoord[3] -= texCoord[1]; // y1 - y0

		ASSERT(textureCoordinates.find(subTextureID) == textureCoordinates.end(), "inserting subtexture that already exists: %s", subTextureID.c_str());

		textureCoordinates[subTextureID] = texCoord;
		textureIDs.push_back(subTextureID);
	}

	const std::string& getRealTextureID() const {
		return textureID;
	}

	const vec4<float>& getSubTextureCoordinates(const std::string& subTexture) const {
		auto it = textureCoordinates.find(subTexture);
		ASSERT(it != textureCoordinates.end(), "subtexture not found: %s", subTexture.c_str());
		return it->second;
	}


	const std::vector<std::string>& getSubTextureTextureIDs() const {
		return textureIDs;
	}
};

}