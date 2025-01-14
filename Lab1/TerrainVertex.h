#pragma once

#include <glm/glm.hpp>
#include <vector>

using namespace std;

// TerrainVertex is used to store variables and functions for creating the terrain vertices
class TerrainVertex
{
public:
	// vertex variables
	glm::vec3 pos;
	glm::vec2 texCoord;
	vector<glm::vec3> normals;

	// functions
	TerrainVertex();
	TerrainVertex(glm::vec3 pos);
	TerrainVertex(glm::vec3 pos, glm::vec2 texCoord);
	void AddNormal(glm::vec3 normal);
	glm::vec3 CalculateNormal();
	~TerrainVertex();
};