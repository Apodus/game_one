
#pragma once

#include "math/2d/mesh.hpp"
#include "util/logging.hpp"
#include <string>
#include <unordered_map>

namespace sa {
	class MeshCollection {
		static std::unordered_map<std::string, sa::Mesh> m_collection;

	public:
		static sa::Mesh& getMesh(const std::string& name) {
			auto it = m_collection.find(name);
			ASSERT(it != m_collection.end(), "Could not find model %s", name.c_str());
			return it->second;
		}

		static void setMesh(const std::string& name, const sa::Mesh& mesh) {
			m_collection[name] = mesh;
		}
	};
}
