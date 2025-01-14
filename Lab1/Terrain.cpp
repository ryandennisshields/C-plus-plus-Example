#include "Terrain.h"
#include <fstream>

Terrain::Terrain()
{
	// initialize variables
	cols = 0;
	rows = 0;
	min = 0.0f;
	max = 0.0f;
	modifier = 0.0f;
}

void Terrain::clearData()
{
	// clear the current terrain's data, including vertices, indices, and normals
	vertices.clear();

	indices.clear();
	normals.clear();
	normalsPos.clear();
}

void Terrain::GenerateTerrain(int rows, int cols)
{
	std::chrono::time_point<std::chrono::system_clock> start, end; // intialize variables for timing
	start = std::chrono::system_clock::now(); // start timing

	numVertices = 0; // reset the number of vertices

	createMesh(rows, cols); // create the mesh for the terrain using the rows and columns
	createNormals(); // create the normals for the terrain

	std::vector<glm::vec3> positions; // initialize vector for positions
	std::vector<glm::vec2> texCoords; // initialize vector for texture coordinates
	std::vector<glm::vec3> normals; // initialize vector for normals

	for (int i = 0; i < numVertices; i++) // for the number of vertices
	{
		positions.push_back(vertices[i].pos); // add the position of the vertex to the positions vector
		texCoords.push_back(vertices[i].texCoord); // add the texture coordinates of the vertex to the texture coordinates vector
		normals.push_back(vertices[i].CalculateNormal()); // add the normal of the vertex to the normals vector after calculating the normal positions
		// add the normal positions to the normals position vector after they have been calculated
		normalsPos.push_back(
			glm::vec3(
				vertices[i].pos.x + normals[i].x,
				vertices[i].pos.y + normals[i].y,
				vertices[i].pos.z + normals[i].z
			)
		);
	}

	// set up OpenGL buffers
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(NUM_BUFFERS, vertexBufferObject);

	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[POSITION_INDEX]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_INDEX);
	glVertexAttribPointer(POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[TEXCOORD_INDEX]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEXCOORD_INDEX);
	glVertexAttribPointer(TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject[NORMAL_INDEX]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_INDEX);
	glVertexAttribPointer(NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObject[ELEMENT_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	// normal vertices setup
	glGenVertexArrays(1, &normalsVAO);
	glGenBuffers(1, normalsVBO);

	glBindVertexArray(normalsVAO);

	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, normalsPos.size() * sizeof(normalsPos[0]), &normalsPos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	end = std::chrono::system_clock::now(); // end timer
	std::chrono::duration<double> elapsedTime = end - start; // calculate elapsed time by taking start time away from ending time
	std::cout << "Generated map in: " << elapsedTime.count() << " seconds, " << numVertices << " vertices" << std::endl; // output the time taken to generate the map and the number of vertices
}

void Terrain::createMesh(int rows, int cols)
{
	PerlinNoise pn(time(NULL)); // create a PerlinNoise object using current time as a seed for randomization
	double xOff = 0.0; // initialize x offset (modifies terrain roughness on the x axis by the modifier value)
	double zOff = 0.0; // initialize z offset (modifies terrain roughness on the z axis by the modifier value)
	double noiseValue = 0.0; // initialize noise value (used to calculate the height of the terrain at a given point using PerlinNoise)

	for (int i = 0; i < rows; i++) // for the amount of rows
	{
		xOff = 0.0; // reset x offset before processing a new row

		for (int j = 0; j < cols; j++) // for the amount of columns
		{
			glm::vec3 vertPos = glm::vec3((-1.0f * cols / 4) + (j * 0.5f), 0, (1.0f * rows / 4) - (i * 0.5f)); // calculate the position of vertexes based on the amount of columns and rows
			noiseValue = pn.noise(xOff, 0, zOff); // calculate the noise value at the current point using PerlinNoise with the x and z offsets
			vertPos.y = MapValue(min, max, noiseValue); // set the y position of the vertex depending on a random noise value modifier to the minimum and maximum
			createVertex(vertPos, glm::vec2(j, i)); // create the vertex based of the position and texture coordinates of the current columns and rows
			xOff += modifier; // increment the x offset by the modifier to make the next x axis position different from the last
		}
		zOff += modifier; // increment the z offset by the modifier to make the next z axis position different from the last
	}

	for (int i = 0; i < numVertices; i++) // for the number of vertices
	{
		// initialize quad positions
		int a = i;
		int b = i + cols;
		int c = i + cols + 1;
		int d = i + 1;

		// make sure that the current vertex is not on the edge of the terrain and the indices are not generated for the previous column or row
		if ((i + 1) % cols == 0) continue;
		if (i >= (rows * cols) - cols) continue;

		// create the first triangle of the quad
		indices.push_back(a);
		indices.push_back(d);
		indices.push_back(b);

		// create the second triangle of the quad
		indices.push_back(b);
		indices.push_back(d);
		indices.push_back(c);
	}
}

void Terrain::createNormals()
{
	for (int i = 0; i != indices.size(); i += 3) // for the amount of indices
	{
		// get the vertices of the triangle
		TerrainVertex& p1 = vertices.at(indices.at(i));
		TerrainVertex& p2 = vertices.at(indices.at(i + 1));
		TerrainVertex& p3 = vertices.at(indices.at(i + 2));

		glm::vec3 U(p2.pos - p1.pos); // calculate the U vector of the triangle
		glm::vec3 V(p3.pos - p1.pos); // calculate the V vector of the triangle

		glm::vec3 normal = glm::cross(U, V); // using the U vector and V vector, calculate the normal of the triangle using cross

		// add normals to the vertices
		vertices.at(indices.at(i)).AddNormal(normal);
		vertices.at(indices.at(i + 1)).AddNormal(normal);
		vertices.at(indices.at(i + 2)).AddNormal(normal);
	}
}

void Terrain::createVertex(glm::vec3 pos, glm::vec2 texCoord)
{
	vertices.push_back(TerrainVertex(pos, texCoord)); // add a new vertex to the vertices vector
	numVertices++; // increment the number of vertices
}

void Terrain::ExportToObj(const std::string& filename)
{
	std::ofstream file; // create a file object
	file.open(filename); // open the file with the given filename

	if (!file.is_open()) // if the file fails to open
	{
		std::cerr << "Failed to open file: " << filename << std::endl; // print an error message
		return;
	}

	// write vertices to the obj file
	for (const auto& vertex : getVertices())
	{
        file << "v " << vertex.pos.x << " " << vertex.pos.y << " " << vertex.pos.z << "\n";
	}

	// write normals to the obj file
	for (const auto& normal : getNormalsPos())
	{
		file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
	}

	// write texture coordinates to the obj file
	for (const auto& texCoord : getTexCoords())
	{
		file << "vt " << texCoord.x << " " << texCoord.y << "\n";
	}

	// write indices to the obj file
	const auto& indices = getIndices();
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		file << "f "
			<< indices[i] + 1 << "/" << indices[i] + 1 << "/" << indices[i] + 1 << " "
			<< indices[i + 1] + 1 << "/" << indices[i + 1] + 1 << "/" << indices[i + 1] + 1 << " "
			<< indices[i + 2] + 1 << "/" << indices[i + 2] + 1 << "/" << indices[i + 2] + 1 << "\n";
	}

	file.close(); // close the file now that writing is done
	std::cout << "Terrain exported to " << filename << std::endl; // print a message to the console to show file path
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &vertexArrayObject); // delete the vertex array object when terrain is destroyed
}