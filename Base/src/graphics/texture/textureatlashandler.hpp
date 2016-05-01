
#pragma once

#include "util/vec4.hpp"
#include "graphics/texture/textureatlas.hpp"

#include <string>
#include <unordered_map>

namespace sa {

class TextureAtlasHandler {

	struct SubTexture {
		sa::vec4<float> subTextureCoordinates;
		std::string realTextureID;

		SubTexture() {
		}

		SubTexture(const sa::vec4<float>& subTextureCoordinates, const std::string& realTextureID) {
			this->subTextureCoordinates = subTextureCoordinates;
			this->realTextureID = realTextureID;
		}
	};

	std::unordered_map<std::string, SubTexture> subTextures;
	sa::vec4<float> DEFAULT_TEXTURE_COORDINATES;

public:
	TextureAtlasHandler() {
		DEFAULT_TEXTURE_COORDINATES = sa::vec4<float>(0.5f, 0.5f, 1.0f, 1.0f);
	}

	void addTextureAtlas(const TextureAtlas& textureAtlas) {
		const std::string& realTextureID = textureAtlas.getRealTextureID();
		for(const std::string& subTextureID : textureAtlas.getSubTextureTextureIDs()) {
			sa::vec4<float> subTextureCoordinates = textureAtlas.getSubTextureCoordinates(subTextureID);
			SubTexture subTexture(subTextureCoordinates, realTextureID);
			subTextures[subTextureID] = subTexture;
			LOG("Added subtexture: %s", subTextureID.c_str());
		}
	}

	const std::string& getRealTextureID(const std::string& subTextureID) const {
		auto it = subTextures.find(subTextureID);
		if(it != subTextures.end())
			return it->second.realTextureID;
		return subTextureID;
	}

	/**
	* Returns texture coordinates for a texture atlas slot.
	* @param subTextureID Id for the slot.
	* @return Vec4 which contains {x, y, width, height}.
	*/
	const sa::vec4<float>& getTextureCoordinateLimits(const std::string& subTextureID) const {
		auto it = subTextures.find(subTextureID);
		if(it == subTextures.end()) {
			LOG("Could not find %s", subTextureID.c_str());
			return DEFAULT_TEXTURE_COORDINATES;
		}
		return it->second.subTextureCoordinates;
	}

	sa::vec4<float> combineUVLimits(const sa::vec4<float>& uvLimits, const sa::vec4<float>& atlasLimits) const {
		sa::vec4<float> limits;
		limits.data[0] = uvLimits.data[0] * atlasLimits.data[2] + atlasLimits.data[0];
		limits.data[1] = uvLimits.data[1] * atlasLimits.data[3] + atlasLimits.data[1];
		limits.data[2] = uvLimits.data[2] * atlasLimits.data[2];
		limits.data[3] = uvLimits.data[3] * atlasLimits.data[3];
		return limits;
	}

	/**
	* Combines two texture coordinates: one that defines a texture atlas slot, and one that defines an area
	* within the slot.
	* @param textureID Id for the texture atlas slot.
	* @param uvLimits Coordinates inside the slot {x, y, width, height}.
	* @return Vec4 which contains {x, y, width, height}.
	*/
	sa::vec4<float> getTextureCoordinateLimits(const std::string& textureID, const sa::vec4<float>& uvLimits) const {
		sa::vec4<float> atlasLimits = getTextureCoordinateLimits(textureID);
		return combineUVLimits(uvLimits, atlasLimits);
	}

};

}