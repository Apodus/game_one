
#pragma once

#include "menu/MenuComponent.hpp"
#include "math/2d/shape.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "math/matrix/matrix4.hpp"

#include "graphics/renderer/meshrenderer.hpp"
#include "graphics/color.hpp"

#include "input/userio.hpp"

#include <memory>

namespace sa {

class MenuLogicalFrame : public MenuComponent {

public:
	MenuLogicalFrame(std::shared_ptr<Window> window, std::shared_ptr<UserIO> userio, const std::string& name, const vec3<float>& position, const vec3<float>& scale) : MenuComponent(window, userio, name, position, scale) {
	}

	template<typename Position, typename Scale>
	MenuLogicalFrame(MenuComponent* parent, const std::string& name, const Position& position, const Scale& scale) : MenuComponent(parent, name, position, scale) {
	}

private:
	virtual void childComponentCall(const std::string&, const std::string&, int = 0) override {
	}

	virtual void draw(std::shared_ptr<Graphics>) const override {
	}

	virtual void update(float) override
	{
	}
};

}
