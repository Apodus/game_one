
#pragma once

#include "menu/MenuFrame.hpp"
#include "menu/MenuComponent.hpp"

#include "math/matrix/matrix4.hpp"
#include "math/2d/mesh.hpp"
#include "math/2d/shape.hpp"
#include "math/2d/polygon.hpp"
#include "math/2d/polygonTesselator.hpp"

#include "graphics/graphics.hpp"

#include "util/vec3.hpp"

namespace sa {

class MenuButton : public MenuComponent {

	float targetAlpha;
	std::string m_texture;
	std::string m_text;
	vec4<float> m_color;
	vec4<float> m_textColor;

	Matrix4 m_model;
	
	MenuFrameBackground m_frame;
	std::unique_ptr<Mesh> m_mesh;

	TextRenderer::Align m_align;

public:
	MenuButton(
		MenuComponent* parent,
		const std::string& name,
		const vec3<float>& position,
		const vec3<float>& scale,
		const std::string& texture,
		const std::string& text,
		const vec4<float>& color = Color::WHITE,
		const vec4<float>& textColor = Color::GOLDEN,
		const TextRenderer::Align& align = TextRenderer::CENTER
	) : MenuComponent(parent, name, position, scale)
		, m_frame(this, "BG", texture)
	{
		this->m_texture = texture;
		this->m_text = text;
		this->m_color = color;
		this->m_textColor = textColor;
		this->m_align = align;
		
		vec4<float> uvLimits = sa::TextureHandler::getSingleton().textureLimits(texture, vec4<float>(0.5f, 0.5f, 1.0f, 1.0f));
		m_mesh = PolygonTesselator<vec3<float>>().tesselate(Shape::makeBox(1.f), uvLimits);
		m_mesh->build();

		targetAlpha = 1;
	}

private:
	virtual void childComponentCall(const std::string&, const std::string&, int = 0) override {
	}

	virtual void draw(std::shared_ptr<Graphics> graphics) const override {
		m_frame.draw(graphics);
		if(!m_text.empty()) {
			const vec3<float>& m_pos = getPosition();
			graphics->m_pTextRenderer->drawText(m_text, m_pos.x, m_pos.y, m_worldScale.y * 0.75f, m_textColor, m_align, graphics->m_fontLenka); 
		}
	}

	virtual void update(float dt) override {
		m_frame.tick(dt);

		m_model.makeTranslationMatrix(getPosition());
		m_model.scale(getScale());

		if(hasFocus() && isMouseOver()) {
			setTargetScale(m_defaultScale * 1.2f);
			targetAlpha = 1.0f;

			if(m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(0))) {
				callParent("click", 0);
			}
			if(m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(1))) {
				callParent("click", 1);
			}
			if(m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(2))) {
				callParent("click", 2);
			}
		}
		else {
			setTargetScale(m_defaultScale);
			targetAlpha = 0.3f;
		}

		m_color.a += (targetAlpha - m_color.a) * dt;
	}
};

}
