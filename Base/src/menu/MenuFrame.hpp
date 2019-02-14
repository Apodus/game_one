
#pragma once

#include "menu/MenuComponent.hpp"
#include "math/2d/mesh.hpp"
#include "math/matrix/matrix4.hpp"
#include "util/vec4.hpp"
#include "graphics/color.hpp"
#include "graphics/texture/texturehandler.hpp"
#include "graphics/graphics.hpp"

namespace sa {
  class MenuFrameBackground : public MenuComponent {
    
		vec4<float> color;
		std::string textureID;
    Matrix4 modelMatrix;
    Mesh backgroundMesh;
    float edgeSize;

    void initMesh() {
      const vec4<float>& limits = TextureHandler::getSingleton().textureLimits(textureID);

      float edgeUV_x = edgeSize * limits.data[2];
      float edgeUV_y = edgeSize * limits.data[3];

      float botCoordX = limits.data[0];
      float botCoordY = limits.data[1];
      float topCoordX = limits.data[0] + limits.data[2];
      float topCoordY = limits.data[1] + limits.data[3];

      backgroundMesh.putUVCoord(botCoordX, topCoordY);
      backgroundMesh.putUVCoord(botCoordX + edgeUV_x, topCoordY);
      backgroundMesh.putUVCoord(topCoordX - edgeUV_x, topCoordY);
      backgroundMesh.putUVCoord(topCoordX, topCoordY);

      backgroundMesh.putUVCoord(botCoordX, topCoordY - edgeUV_y);
      backgroundMesh.putUVCoord(botCoordX + edgeUV_x, topCoordY - edgeUV_y);
      backgroundMesh.putUVCoord(topCoordX - edgeUV_x, topCoordY - edgeUV_y);
      backgroundMesh.putUVCoord(topCoordX, topCoordY - edgeUV_y);

      backgroundMesh.putUVCoord(botCoordX, botCoordY + edgeUV_y);
      backgroundMesh.putUVCoord(botCoordX + edgeUV_x, botCoordY + edgeUV_y);
      backgroundMesh.putUVCoord(topCoordX - edgeUV_x, botCoordY + edgeUV_y);
      backgroundMesh.putUVCoord(topCoordX, botCoordY + edgeUV_y);

      backgroundMesh.putUVCoord(botCoordX, botCoordY);
      backgroundMesh.putUVCoord(botCoordX + edgeUV_x, botCoordY);
      backgroundMesh.putUVCoord(topCoordX - edgeUV_x, botCoordY);
      backgroundMesh.putUVCoord(topCoordX, botCoordY);

      for(int x=0; x<3; ++x)
      {
        for(int y=0; y<3; ++y)
        {
          int index = x + 4 * y;
          backgroundMesh.putTriangleIndices(index, index + 5, index + 4);
          backgroundMesh.putTriangleIndices(index, index + 1, index + 5);
        }
      }
    }

public:
    MenuFrameBackground(MenuComponent* parent, const std::string& name, const std::string& textureID, const vec4<float>& color = Color::WHITE, float edgeSize = 0.20f) : MenuComponent(parent, name, vec3<float>(), vec3<float>()), backgroundMesh(16, 9 * 2 * 3) {
      this->color = color;
      this->textureID = textureID;
			this->edgeSize = edgeSize;

      initMesh();
      buildMesh(0.5f, 0.5f);
      backgroundMesh.build();
    }

    void buildMesh(float size_x, float size_y)
    {
      float edgeSizeX = 0.05f / size_x;
      float edgeSizeY = 0.05f / size_y;

      backgroundMesh.vertices.clear();

      backgroundMesh.putVertex(-1, -1, 0);
      backgroundMesh.putVertex(-1 + edgeSizeX, -1, 0);
      backgroundMesh.putVertex(+1 - edgeSizeX, -1, 0);
      backgroundMesh.putVertex(+1, -1, 0);

      backgroundMesh.putVertex(-1, -1 + edgeSizeY, 0);
      backgroundMesh.putVertex(-1 + edgeSizeX, -1 + edgeSizeY, 0);
      backgroundMesh.putVertex(+1 - edgeSizeX, -1 + edgeSizeY, 0);
      backgroundMesh.putVertex(+1, -1 + edgeSizeY, 0);

      backgroundMesh.putVertex(-1, +1 - edgeSizeY, 0);
      backgroundMesh.putVertex(-1 + edgeSizeX, +1 - edgeSizeY, 0);
      backgroundMesh.putVertex(+1 - edgeSizeX, +1 - edgeSizeY, 0);
      backgroundMesh.putVertex(+1, +1 - edgeSizeY, 0);

      backgroundMesh.putVertex(-1, +1, 0);
      backgroundMesh.putVertex(-1 + edgeSizeX, +1, 0);
      backgroundMesh.putVertex(+1 - edgeSizeX, +1, 0);
      backgroundMesh.putVertex(+1, +1, 0);

      backgroundMesh.rebuildVertexBuffer();
    }

    vec4<float>& getColor()
    {
      return color;
    }

    virtual void childComponentCall(const std::string&, const std::string&, int = 0) override
    {
    }

    virtual void update(float) override
    {
      const auto& parentPosition = m_pParent->getPosition();
      const auto& parentDimensions = m_pParent->getScale();

	  setPosition(parentPosition);
	  setScale(parentDimensions);

      buildMesh(parentDimensions.x, parentDimensions.y);
      modelMatrix.makeTranslationMatrix(parentPosition.x, parentPosition.y, 0);
      modelMatrix.scale(parentDimensions.x * .5f, parentDimensions.y * .5f, 1);
    }

    virtual void draw(std::shared_ptr<Graphics> graphics) const override
    {
      graphics->m_pRenderer->drawMesh(backgroundMesh, modelMatrix, textureID, color);
    }
  };
}
