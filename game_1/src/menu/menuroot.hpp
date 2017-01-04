
#pragma once

#include "script/Scripter.hpp"
#include "graphics/window/window.hpp"
#include "input/userio.hpp"
#include "util/vec3.hpp"

#include "menu/mainmenu.hpp"
#include "menu/optionsmenu.hpp"
#include "menu/MenuLogicalFrame.hpp"
#include "menu/gameline.hpp"

#include <memory>

namespace sa {
class MenuRoot : public sa::MenuLogicalFrame {
public:
	MenuRoot(
		std::shared_ptr<sa::Window> window,
		std::shared_ptr<sa::UserIO> userio,
		const std::string& name,
		const sa::vec3<float>& position,
		const sa::vec3<float>& scale
	) : sa::MenuLogicalFrame(window, userio, name, position, scale)
	{
	}
};
}