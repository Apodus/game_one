
#include "graphics/texture/texturehandler.hpp"
#include "graphics/opengl/opengl.hpp"
#include "util/logging.hpp"
#include "util/platform.hpp"

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

sa::TextureHandler::TextureHandler()
{
	int max_texture_units = 0;
#if PLATFORM_WIN
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);
	LOG("Max texture units: %d", max_texture_units);
	if(max_texture_units == 0)
		max_texture_units = 4; // pray
#else
	max_texture_units = 4; // make a wild guess :DD
#endif
	current_textures.resize(max_texture_units);
}

sa::TextureHandler::~TextureHandler()
{
	deleteAllTextures();
}

sa::TextureHandler& sa::TextureHandler::getSingleton()
{
	static sa::TextureHandler s_TexHandler;
	return s_TexHandler;
}

const std::string& sa::TextureHandler::getCurrentTexture(size_t texture_unit) const
{
	ASSERT(texture_unit < current_textures.size(), "Texture unit out of bounds! Requested: %u, size %u", static_cast<unsigned>(texture_unit), static_cast<unsigned>(current_textures.size()));
	return current_textures[texture_unit];
}

void sa::TextureHandler::createTextures(const std::string& filename)
{
	std::ifstream in(filename.c_str());

	std::string line;
	while(getline(in, line))
	{
		if(line.empty() || line[0] == '#')
		{
			continue;
		}
		std::stringstream ss(line);
		std::string name;
		std::string file;
		ss >> name >> file;
		if(name == "atlas") {
			std::string atlasFileName;
			ss >> atlasFileName;
			sa::TextureAtlas atlas(file, textureSizes[file].x, textureSizes[file].y);
			std::ifstream atlasIn(atlasFileName);
			while(getline(atlasIn, line))
			{
				if(line.empty() || line[0] == '#')
				{
					continue;
				}

				std::stringstream atlasSS(line);
				int x, y, maxx, maxy;
				atlasSS >> name >> x >> y >> maxx >> maxy;
				atlas.insertTexture(name, x, y, maxx, maxy);
			}

			insertAtlas(atlas);
		}
		else {
			createTexture(name, file);
		}
	}
}

unsigned sa::TextureHandler::createTexture(const std::string& name, const std::string& filename)
{
	ASSERT(!name.empty(), "create texture called with empty name");

	Image img;
	img.loadImage(filename);
	
	if((img.sizeX == 0) || (img.sizeY == 0))
	{
		LOG("Failed to load texture '%s' from file '%s'. File doesn't exist?", name.c_str(), filename.c_str());
		return 0;
	}
	
	LOG("Loading %dx%d texture '%s' from file '%s'", static_cast<int>(img.sizeX), static_cast<int>(img.sizeY), name.c_str(), filename.c_str());
	return createTexture(name, img);
}


unsigned sa::TextureHandler::getTextureID(const std::string& name) const
{
	ASSERT(!name.empty(), "get texture called with an empty name!");
	std::string realName = m_atlasHandler.getRealTextureID(name);
	auto it = textures.find(realName);
	ASSERT(it != textures.end(), "texture for '%s' not found! (real texture name: '%s')", name.c_str(), realName.c_str());
	return it->second;
}

namespace
{
	struct Color
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
}

void buildDebugMipmaps(size_t x, size_t y)
{
	ASSERT(x == y, "debug mipmaps don't work with non-square textures");

	int lod = 0;
	do
	{
		size_t bit = 1;
		while(bit < x)
			bit <<= 1;

		unsigned char r = 255 - bit * 255 / 8;
		unsigned char g = bit * 255 / 8;
		unsigned char b = 0;
		Color color = { r, g, b, 255 };

		std::vector<Color> data(x*y, color);
		glTexImage2D(GL_TEXTURE_2D, lod++, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);

		x /= 2;
		y /= 2;
	}
	while(x != 0 && y != 0);
}


unsigned sa::TextureHandler::createFloatTexture(const std::string& name, int width, int height)
{
	ASSERT(!name.empty(), "create float texture called with empty name");

	glGenTextures(1, &(textures[name]));
	glBindTexture(GL_TEXTURE_2D, textures[name]);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR); // scale linearly when image smalled than texture
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	int value;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &value);
	return textures[name];
}

unsigned sa::TextureHandler::createTexture(const std::string& name, int width, int height)
{
	ASSERT(!name.empty(), "create texture called with empty name");

	glGenTextures(1, &(textures[name]));
	glBindTexture(GL_TEXTURE_2D, textures[name]);
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	int value;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &value);
	textureSizes[name] = sa::vec4<short>(width, height, 0, 0);
	return textures[name];
}

unsigned sa::TextureHandler::createDepthTexture(const std::string& name, int width, int height)
{
	ASSERT(!name.empty(), "create depth texture called with empty name");

	glGenTextures(1, &(textures[name]));
	glBindTexture(GL_TEXTURE_2D, textures[name]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

#ifndef _WIN32
	// not really sure if this is 100% necessary anyway..
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_NONE);
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	int value;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &value);
	return textures[name];
}

unsigned sa::TextureHandler::createTexture(const std::string& name, Image& img)
{
	ASSERT(!name.empty(), "create texture called with an empty name");

	if(img.data == 0)
	{
		LOG("ERROR: Trying to build texture of image pointer -> 0");
		return 0;
	}
	
	if(textureExists(name))
	{
		LOG("Texture for \"%s\" is already loaded!", name.c_str());
		return textures[name];
	}

	glGenTextures(1, &(textures[name]));
	glBindTexture(GL_TEXTURE_2D, textures[name]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // scale linearly when image bigger than texture
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // Deprecated in OpenGL >= 3.0.

	// 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
	// border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
	if(img.hasAlpha)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.sizeX, img.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.sizeX, img.sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
//		buildDebugMipmaps(img.sizeX, img.sizeY);
	}
	
	textureSizes[name] = sa::vec4<short>(static_cast<short>(img.sizeX), static_cast<short>(img.sizeY), 0, 0);
	img.unload();
	
	return textures[name];
}


void sa::TextureHandler::deleteTexture(const std::string& name)
{
	ASSERT(!name.empty(), "deleting texture with an empty name?");

	std::string realName = m_atlasHandler.getRealTextureID(name);

	if(textureExists(realName))
	{
		glDeleteTextures(1, &textures[realName]);
		textures.erase(realName);
	}
	else
	{
		LOG("WARNING: tried to delete a texture that doesnt exist: \"%s\"", name.c_str());
	}
}


void sa::TextureHandler::unbindTexture(size_t texture_unit)
{
	glActiveTexture(GL_TEXTURE0 + texture_unit);
	glDisable(GL_TEXTURE_2D);
	current_textures[texture_unit] = "";
}

int sa::TextureHandler::bindTexture(size_t texture_unit, const std::string& name)
{
	ASSERT(texture_unit < current_textures.size(), "texture unit out of bounds: %d >= %d", static_cast<int>(texture_unit), static_cast<int>(current_textures.size()));
	ASSERT(!name.empty(), "empty name for bind texture");

	std::string realName = m_atlasHandler.getRealTextureID(name);

	if(current_textures[texture_unit] == realName)
		return 1;

	if(textureExists(realName))
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[realName]);
		current_textures[texture_unit] = realName;
		return 1;
	}
	else
	{
		LOG("Trying to bind to a texture that does not exist: \"%s\"", name.c_str());
		ASSERT(!textures.empty(), "please load at least one texture...");
		std::string default_texture = textures.begin()->first;
		LOG("Loading default texture '%s'", default_texture.c_str());
		ASSERT(!default_texture.empty(), "failed to load default texture :(");
		textures[name] = textures[default_texture];
		bindTexture(texture_unit, default_texture);
		return 0;
	}
}

void sa::TextureHandler::deleteAllTextures()
{
	for(auto iter = textures.begin(); iter != textures.end(); iter++)
	{
		glDeleteTextures(1, &iter->second);
	}
	textures.clear();
}

bool sa::TextureHandler::textureExists(const std::string& name) const
{
	std::string realName = m_atlasHandler.getRealTextureID(name);
	return textures.find(name) != textures.end();
}

const sa::vec4<float>& sa::TextureHandler::textureLimits(const std::string& name) const {
	return m_atlasHandler.getTextureCoordinateLimits(name);
}

sa::vec4<float> sa::TextureHandler::textureLimits(const std::string& name, const sa::vec4<float>& uvLimits) const {
	return m_atlasHandler.getTextureCoordinateLimits(name, uvLimits);
}

void sa::TextureHandler::insertAtlas(const sa::TextureAtlas& atlas) {
	m_atlasHandler.addTextureAtlas(atlas);
}

