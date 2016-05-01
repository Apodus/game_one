
#pragma once

#include "math/2d/mesh.hpp"

#include <string>
#include <unordered_map>

struct aiScene;

namespace sa {
class MeshHandler {
	std::unordered_map<std::string, sa::Mesh*> m_meshes;
	
public:
	const sa::Mesh* operator[](const std::string& name) const {
		auto it = m_meshes.find(name);
		ASSERT(it != m_meshes.end(), "looking for model that doesn't exist: %s", name.c_str());
		return it->second;
	}

	sa::Mesh* operator[](const std::string& name) {
		auto it = m_meshes.find(name);
		ASSERT(it != m_meshes.end(), "looking for model that doesn't exist: %s", name.c_str());
		return it->second;
	}

	bool importModelsFromScene(const std::string& fileName);

private:

	void loadMeshesFromScene(const aiScene *sc);
};

}
