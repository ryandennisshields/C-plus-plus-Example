#pragma once

#include <SDL\SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Display.h" 
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "transform.h"
#include "SkyBox.h"
#include "Terrain.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_sdl2.h"
#include "../ImGui/imgui_impl_opengl3.h"

enum class GameState{PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	void initSystems();
	void initRandomness();

	void createNewTerrain();
	void createScreenQuad();
	void processInput();
	void gameLoop();

	void linkEmapping();
	void linkNoise();
	void linkExploding();

	void drawModels();
	void drawSkyBox();
	void drawGame();
	void drawTerrain();

	void unbindFBO();
	void bindFBO();
	void generateFBO(float w, float h);
	void renderFBO();
	void applyBlur();
	void combineBloom();
	
	void renderGUI();

	int scene;

	Display _gameDisplay;
	GameState _gameState;
	Mesh mesh1;
	Mesh mesh2;
	Mesh mesh3;
	Mesh terrainMesh;
    Terrain terrain;
	Camera myCamera;

	Shader shaderSkybox;
	Shader eMapping;
	Shader noiseShader;
	Shader geoShader;
	Shader FBOShader;
	Shader blurShader;
	Shader bloomShader;
	Shader terrainShader;
	Texture texture;
	Texture texture1;
	Texture texture2;
	Texture texture3;
	Texture texture4;
	Texture texture5;
	Texture terrainTexture;

	Skybox skybox;

	vector<std::string> faces;

	float counter;

	int randTime;
	int randFreqX;
	int randFreqY;
	float randTexCoords;
	float randOffset;
	float randColor1R;
	float randColor1G;
	float randColor1B;
	float randColor2R;
	float randColor2G;
	float randColor2B;

	GLuint FBO;
	GLuint RBO;
	GLuint CBO;
	GLuint PPFBO[2];
	GLuint PPB[2];

	unsigned int quadVAO, quadVBO;

	glm::vec3 cameraTarget;
	float orbitSpeed;

	float animationSpeed;
	float bloomIntensity;

	glm::vec3 lightPosition;
};
