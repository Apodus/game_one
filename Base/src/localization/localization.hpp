
#pragma once

#include <string>

namespace sa {
	namespace localization {
		void setLanguage(const std::string& s);
		const std::string& get(const std::string& key);
	}
}
