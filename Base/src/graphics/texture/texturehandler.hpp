
#pragma once

#include "graphics/texture/image.hpp"
#include "graphics/texture/textureatlashandler.hpp"

#include <map>
#include <string>
#include <vector>

namespace sa {

class TextureHandler
{
	public:
		static TextureHandler& getSingleton();
		
		void createTextures(const std::string& filename);
		unsigned createTexture(const std::string&, const std::string&);
		unsigned createTexture(const std::string&, Image& img);
		unsigned createTexture(const std::string&, int width, int height);
		unsigned createDepthTexture(const std::string& name, int width, int height);
		unsigned createFloatTexture(const std::string& name, int width, int height);
		
		int bindTexture(size_t texture_unit, const std::string&);
		void unbindTexture(size_t texture_unit);

		bool textureExists(const std::string& name) const;
		const std::string& getCurrentTexture(size_t texture_unit) const;
		unsigned getTextureID(const std::string&) const;

		const vec4<float>& textureLimits(const std::string&) const;
		vec4<float> textureLimits(const std::string& name, const sa::vec4<float>& uvLimits) const;

		void insertAtlas(const TextureAtlas& atlas);
		
		void deleteTexture(const std::string& name);
		void deleteAllTextures();

	private:
		TextureHandler();
		~TextureHandler();

		TextureAtlasHandler m_atlasHandler;

		TextureHandler(const TextureHandler&); // Disabled.
		TextureHandler& operator=(const TextureHandler&); // Disabled.
		
		std::map<std::string, sa::vec4<short>> textureSizes;
		std::map<std::string, unsigned> textures;
		std::vector<std::string> current_textures;

};

}
