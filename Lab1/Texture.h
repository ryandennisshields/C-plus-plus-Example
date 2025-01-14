#pragma once

#include <string>
#include <GL\glew.h>

class Texture
{
public:
	Texture();
	void init(const std::string& fileName);
	void Bind(unsigned int unit); // bind upto 32 textures
	GLuint getID() { return textureHandler; }
	~Texture();

private:
	GLuint textureHandler;
};

