#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "TerrainVertex.h"

using namespace std;

// TerrainMesh is used to store variables and functions for creating the terrain mesh
class TerrainMesh
{
protected:
	// indexes for accessing vectors
	enum
	{
		POSITION_INDEX,
		TEXCOORD_INDEX,
		NORMAL_INDEX,
		ELEMENT_INDEX,

		NUM_BUFFERS
	};

	// OpenGL variables
	GLuint vertexArrayObject;
	GLuint vertexBufferObject[NUM_BUFFERS];
	GLuint normalsVAO;
	GLuint normalsVBO[1];

	// variables for storing mesh data
	int numVertices;
	vector<TerrainVertex> vertices;
	vector<unsigned short> indices;
	vector<glm::vec3> normals;
	vector<glm::vec3> normalsPos;

public:
	// functions to get mesh data
	vector<TerrainVertex> getVertices() { return vertices; }
	vector<unsigned short> getIndices() { return indices; }
	vector<glm::vec3> getNormals() { return normals; }
	vector<glm::vec3> getNormalsPos() { return normalsPos; }
	vector<glm::vec2> getTexCoords() {
		vector<glm::vec2> texCoords;
		for (const auto& vertex : vertices) {
			texCoords.push_back(vertex.texCoord);
		}
		return texCoords;
	}
};

inline float MapValue(float minimum, float maximum, float value)
{
	// create a map value by taking into account the maximum and minimum values, then multiply by a value (determined by PerlinNoise randomness)
	// "minimum +" keeps the value within the minimum and maximum range
	return minimum + (maximum - minimum) * value; 
}