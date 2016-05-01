
#include "meshhandler.hpp"

#include "util/logging.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include <fstream>

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

bool sa::MeshHandler::importModelsFromScene(const std::string& fileName)
{
	Assimp::Importer importer;

	std::ifstream fin(fileName.c_str());
	if (!fin.fail()) {
		fin.close();
	}
	else {
		LOG("Couldn't open file: %s", fileName.c_str());
		LOG("%s", importer.GetErrorString());
		return false;
	}

	const aiScene* scene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality);

	// If the import failed, report it
	if (!scene)
	{
		ASSERT(false, "%s", importer.GetErrorString());
		return false;
	}

	// Everything will be cleaned up by the importer destructor
	loadMeshesFromScene(scene);
	LOG("Import of scene %s succeeded.", fileName.c_str());
	return true;
}


void sa::MeshHandler::loadMeshesFromScene(const aiScene *sc) {

	for (unsigned int n = 0; n < sc->mNumMeshes; ++n)
	{
		const aiMesh* mesh = sc->mMeshes[n];
		sa::Mesh* saMesh = new sa::Mesh(mesh->mNumVertices, mesh->mNumFaces * 3);

		LOG("Starting to load mesh with name: %s", mesh->mName.C_Str());

		LOG("Faces: %d", mesh->mNumFaces);
		for (unsigned t = 0; t < mesh->mNumFaces; ++t) {
			const aiFace* face = &mesh->mFaces[t];
			saMesh->putTriangleIndices(face->mIndices[0], face->mIndices[1], face->mIndices[2]);
		}

		LOG("Vertices: %d", mesh->mNumVertices);
		for (unsigned t = 0; t < mesh->mNumVertices; ++t) {
			saMesh->putVertex(mesh->mVertices[t].x, mesh->mVertices[t].y, mesh->mVertices[t].z);
		}

		LOG("Normals: %d", mesh->mNumVertices);
		for (unsigned t = 0; t < mesh->mNumVertices; ++t) {
			saMesh->putNormal(mesh->mNormals[t].x, mesh->mNormals[t].y, mesh->mNormals[t].z);
		}

		for (int i = 0; i<10; ++i) {
			LOG("Has texCoords %d: %d", i, mesh->HasTextureCoords(i));
		}

		ASSERT(mesh->HasTextureCoords(0), "Model doesn't have tex coords?");
		if (!mesh->HasTextureCoords(0))
			continue;

		for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
			saMesh->putUVCoord(mesh->mTextureCoords[0][k].x, mesh->mTextureCoords[0][k].y);
		}

		LOG("Loaded mesh with name: %s", mesh->mName.C_Str());
		m_meshes[std::string(mesh->mName.C_Str())] = saMesh;

		/*
		// create material
		aiMaterial *mtl = sc->mMaterials[mesh->mMaterialIndex];
		aiString texPath;
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, 0, &texPath)) {
		// bind texture
		unsigned int texId = textureIdMap[texPath.data];
		aMesh.texIndex = texId;
		aMat.texCount = 1;
		}
		else {
		aMat.texCount = 0;
		}

		float c[4];
		set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
		aiColor4D diffuse;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
		memcpy(aMat.diffuse, c, sizeof(c));

		set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
		aiColor4D ambient;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
		memcpy(aMat.ambient, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D specular;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
		memcpy(aMat.specular, c, sizeof(c));

		set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
		aiColor4D emission;
		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
		memcpy(aMat.emissive, c, sizeof(c));

		float shininess = 0.0;
		unsigned int max;
		aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
		aMat.shininess = shininess;

		glGenBuffers(1,&(aMesh.uniformBlockIndex));
		glBindBuffer(GL_UNIFORM_BUFFER,aMesh.uniformBlockIndex);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(aMat), (void *)(&aMat), GL_STATIC_DRAW);

		myMeshes.push_back(aMesh);
		*/
	}
}

