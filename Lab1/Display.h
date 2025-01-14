#pragma once

#include <SDL/SDL.h>
#include <GL\glew.h>
#include <iostream>
#include <string>
#include "Shader.h"
#include "Mesh.h"
using namespace std;


class Display
{
public:
	Display();
	~Display();
	void initDisplay();

    SDL_Window* getWindow() { return sdlWindow; }
    SDL_GLContext getContext() { return glContext; }

	void swapBuffer();
	void clearDisplay(float r, float g, float b, float a);

	float getWidth();
	float getHeight();

private:
	void returnError(std::string errorString);
	
	SDL_GLContext glContext; // global variable to hold the context
	SDL_Window* sdlWindow; // holds pointer to out window
	float screenWidth;
	float screenHeight;
};

