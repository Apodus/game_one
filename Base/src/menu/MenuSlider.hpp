
#pragma once

#include "menu/MenuComponent.hpp"
#include "util/logging.hpp"

#include "graphics/graphics.hpp"
#include "math/matrix/matrix4.hpp"
#include "math/2d/polygonTesselator.hpp"

#include <string>
#include <functional>

namespace sa {

class MenuSlideBarVertical : public MenuComponent {

    std::string m_knobTexture;
    std::string m_baseTexture;
	std::function<void(float)> m_callback;

	mutable Matrix4 m_modelMatrix;
	std::unique_ptr<Mesh> sliderMesh;

    float m_minValue;
    float m_maxValue;
    float m_currentValue;
	float m_roundTo;

	bool m_mouseDragActive;

public:
	MenuSlideBarVertical(MenuComponent* parent, const std::string& name, const vec3<float>& position, const vec3<float>& scale, const std::string& knobTexture, const std::string& baseTexture, std::function<void(float)> callback, float minValue = 0, float maxValue = 1, float initialValue = 0.5f)
		: MenuComponent(parent, name, position, scale)
	{
        ASSERT(maxValue > minValue, "slider max value should be bigger than min value?");
		sliderMesh = PolygonTesselator<vec3<float>>().tesselate(Shape::makeBox(1.f), TextureHandler::getSingleton().textureLimits(knobTexture, vec4<float>(0.5f, 0.5f, 1.0f, 1.0f)));
		sliderMesh->build();

        m_knobTexture = knobTexture;
        m_baseTexture = baseTexture;
        m_minValue = minValue;
        m_maxValue = maxValue;
        m_name = name;
		m_mouseDragActive = false;

		m_callback = callback;
        setValue(initialValue);
    }

	virtual void childComponentCall(const std::string&, const std::string&, int = 0) override {
    }

	virtual void draw(std::shared_ptr<Graphics> graphics) const override {
        const auto& position = getPosition();
		const auto& dimensions = getScale();

        float lineWidth = dimensions.x;
        float lineHeight = dimensions.y;
        float markerWidth = lineHeight;

		float markerX = position.x + dimensions.x * (m_currentValue - 0.5f);

		m_modelMatrix.makeTranslationMatrix(position);
		m_modelMatrix.scale(lineWidth, lineHeight * 0.2f, 1);
		graphics->m_pRenderer->drawMesh(*sliderMesh, m_modelMatrix, m_baseTexture, Color::WHITE);
		
		m_modelMatrix.makeTranslationMatrix(markerX, position.y, 0);
		m_modelMatrix.scale(markerWidth, lineHeight, 1);
		graphics->m_pRenderer->drawMesh(*sliderMesh, m_modelMatrix, m_knobTexture, Color::GREY);
    }

	virtual void update(float) override {
		if(isMouseOver()) {
			int mouseKey1 = m_pUserIO->getMouseKeyCode(0);
			if(m_pUserIO->isKeyPressed(mouseKey1))
				m_mouseDragActive = true;
			if(m_pUserIO->isKeyReleased(mouseKey1))
				m_mouseDragActive = false;

			vec3<float> cursorPos;
			m_pUserIO->getCursorPosition(cursorPos);

			if(m_mouseDragActive) {
				onActivate(cursorPos.x, cursorPos.y);
			}
		}
		else {
			m_mouseDragActive = false;
		}
    }

    void onActivate(float x, float /* y */) {
        float length = getScale().x;
        if(length > 0.0f) {
            x -= getPosition().x;
			m_currentValue = (x / length + 0.5f);
			m_callback(m_currentValue * (m_maxValue - m_minValue) + m_minValue);
        }
    }

    void setValue(float value) {
        m_currentValue = (value - m_minValue) / (m_maxValue - m_minValue);
		m_callback(m_currentValue * (m_maxValue - m_minValue) + m_minValue);
    }
};

}
