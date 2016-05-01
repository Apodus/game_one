
#include "localization/localization.hpp"
#include "util/logging.hpp"

#include <unordered_map>
#include <fstream>

namespace {
	std::unordered_map<std::string, std::string> g_translations;
	std::string g_currentLanguage;
}

void sa::localization::setLanguage(const std::string& s) {
	if(s == g_currentLanguage)
		return;
	g_currentLanguage = s;
	g_translations.clear();

	std::ifstream in(std::string("../localization/") + s + std::string(".dat"));
	std::string key;
	std::string line;
	
	while(in >> key) {
		in.ignore();
		getline(in, line);
		g_translations[key] = line;
	}
}

const std::string& sa::localization::get(const std::string& key) {
	auto it = g_translations.find(key);
	if(it != g_translations.end()) {
		return it->second;
	}

	g_translations[key] = std::string("!!") + key;
	LOG("%s", g_translations[key].c_str());
	return g_translations[key];
}