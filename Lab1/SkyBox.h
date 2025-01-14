#pragma once

#include <vector>
#include <GL\glew.h>
#include <iostream>
#include "../Lab1/Camera.h"
#include "../Lab1/stb_image.h"
#include "../Lab1/Shader.h"

class Skybox
{
public:
	void init(std::vector<std::string> faces);
	void draw(Camera* camera);

	unsigned int textureID;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;

private:
	Shader shader;
};

