#pragma once

#include <chrono>
#include <math.h>
#include <iostream>
#include "TerrainMesh.h"
#include "PerlinNoise.h"

class Terrain : public TerrainMesh // inherit from TerrainMesh
{
public:
	// controls terrain size
	int rows; // rows of the terrain
	int cols; // columns of the terrain

	// controls terrain height and roughness
	float min; // minimum height
	float max; // maximum height
	float modifier; // modifier to control roughness (amount of peaks and dips)

	Terrain();
	void clearData();
	void GenerateTerrain(int rows, int cols);
	void createMesh(int rows, int cols);
	void createNormals();
	void createVertex(glm::vec3 pos, glm::vec2 texCoord);
	void ExportToObj(const std::string& fileName);
	~Terrain();
};


