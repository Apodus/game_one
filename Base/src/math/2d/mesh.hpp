
#pragma once

typedef int GLint;
typedef unsigned GLuint;

#include "util/dynarray.hpp"

namespace sa {

class Mesh {
public:
	Mesh(int vertices, int indices);
	Mesh();

	void putVertex(float x, float y, float z);
	void putUVCoord(float u, float v);
	void putNormal(float x, float y, float z);
	void putTriangleIndices(int i1, int i2, int i3);

	std::dynarray<float> vertices;
	std::dynarray<float> texCoords;
	std::dynarray<float> normals;
    std::dynarray<short> indices;

	int getVertexCount() const;
	int getIndexCount() const;

	void build();
	void rebuildTextureBuffer();
	void rebuildVertexBuffer();
	void bind() const;

private:
	/*
		Shaders must use following attribute layout:
			layout(location = 0) in vec3 position;
			layout(location = 1) in vec2 texCoord;
			layout(location = 2) in vec3 normals;
	*/
	enum Attributes { POSITION, TEXCOORD, NORMALS };

	GLuint vao;
	GLuint vbo, tbo, nbo, ibo;	// vertices, texcoords, normals, indices

    int numVertices;
    int numIndices;
};

}
