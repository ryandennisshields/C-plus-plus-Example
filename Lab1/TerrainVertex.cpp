#include "TerrainVertex.h"

TerrainVertex::TerrainVertex()
{
}

TerrainVertex::TerrainVertex(glm::vec3 pos)
{
	this->pos = pos; // set the position of the vertex
}

TerrainVertex::TerrainVertex(glm::vec3 pos, glm::vec2 texCoord)
{
	this->pos = pos; // set the position of the vertex
	this->texCoord = texCoord; // set the texture coordinates of the vertex
	this->normals.clear(); // clear the normals vector to remove old data for the vertex
}

void TerrainVertex::AddNormal(glm::vec3 normal)
{
	this->normals.push_back(normal); // add a normal to the normals vector
}

glm::vec3 TerrainVertex::CalculateNormal() // calculate the averaged normal of the normals to create smoother and more accurate lighting
{
	glm::vec3 averagedNormal;

	for (int i = 0; i < normals.size(); i++)
	{
		averagedNormal += normals[i]; // add the current normals to the averaged normal
	}

	// get the averaged normal by dividing the normals by the normals vector size
	averagedNormal = glm::vec3(
		averagedNormal.x / normals.size(),
		averagedNormal.y / normals.size(),
		averagedNormal.z / normals.size()
	);

	return averagedNormal;
}

TerrainVertex::~TerrainVertex()
{
}