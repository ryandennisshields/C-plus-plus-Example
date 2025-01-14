#include "MainGame.h"
#include "Camera.h"
#include <iostream>
#include <string>

Transform transform;

MainGame::MainGame()
	// initialise all variables
	: CBO(0), FBO(0), RBO(0), PPB(), PPFBO(), quadVAO(0), quadVBO(0), animationSpeed(0.0f), bloomIntensity(0.0f), counter(0.0f), orbitSpeed(0.0f),
	randColor1R(0.0f), randColor1G(0.0f), randColor1B(0.0f), randColor2R(0.0f), randColor2G(0.0f), randColor2B(0.0f),
	randFreqX(0), randFreqY(0), randOffset(0.0f), randTexCoords(0.0f), randTime(0), scene(0)
{
	_gameState = GameState::PLAY;
	Display* _gameDisplay = new Display();
}

MainGame::~MainGame()
{
}

void MainGame::run()
{
	// run init systems and game loop
	initSystems();
	gameLoop();
}

void MainGame::initSystems()
{
	// initialize display
	_gameDisplay.initDisplay(); 
	
	// load models
	mesh1.loadModel("..\\res\\cube.obj");
	mesh2.loadModel("..\\res\\sphere.obj");
	mesh3.loadModel("..\\res\\cone.obj");
	terrainMesh.loadModel("..\\res\\terrain.obj");

	// load shaders
	eMapping.init("..\\res\\shaderReflection.vert", "..\\res\\shaderReflection.frag");
	noiseShader.init("..\\res\\noise.vert", "..\\res\\noise.frag");
	geoShader.initGeo();
	FBOShader.init("..\\res\\FBOShader.vert", "..\\res\\FBOShader.frag");
	blurShader.init("..\\res\\blur.vert", "..\\res\\blur.frag");
	bloomShader.init("..\\res\\bloom.vert", "..\\res\\bloom.frag");
	terrainShader.init("..\\res\\terrainshader.vert", "..\\res\\terrainshader.frag");

	// load textures
	texture.init("..\\res\\noise.png");
	texture1.init("..\\res\\lava.jpg");
	texture2.init("..\\res\\bricks.jpg");
	texture3.init("..\\res\\grass.jpg");
	texture4.init("..\\res\\stone.jpg");
	texture5.init("..\\res\\water.jpg");
	terrainTexture = texture3; // set terrain texture to grass as default

	// initialise camera
	myCamera.initCamera(glm::vec3(0, 0, -5), 70.0f, (float)_gameDisplay.getWidth()/_gameDisplay.getHeight(), 0.01f, 1000.0f);

	// generate the framebuffer object
	generateFBO(_gameDisplay.getWidth(), _gameDisplay.getHeight());

	// create a screen quad
	createScreenQuad();

	// initialise skybox faces
	vector<std::string> faces
	{
		"..\\res\\skybox\\right.png",
		"..\\res\\skybox\\left.png",
		"..\\res\\skybox\\top.png",
		"..\\res\\skybox\\bot.png",
		"..\\res\\skybox\\front.png",
		"..\\res\\skybox\\back.png"
	};

	// initialise skybox with faces
	skybox.init(faces);

	// initialise random variables for noise shader
	initRandomness();

	// initialize ImGui
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(_gameDisplay.getWindow(), _gameDisplay.getContext());
    ImGui_ImplOpenGL3_Init("#version 330");
}

void MainGame::initRandomness() 
{
	srand(static_cast <unsigned> (time(0))); // create a new seed for the random values
	randTime = rand() % 19 + 1; // random time value between 1 and 20
	randFreqX = rand() % 6; // random frequency value between 0 and 5
	randFreqY = rand() % 4; // random frequency value between 0 and 3
	// random values between 0 and 1
	randTexCoords = (float)rand() / (RAND_MAX);
	randOffset = (float)rand() / (RAND_MAX);
	randColor1R = (float)rand() / (RAND_MAX); 
	randColor1G = (float)rand() / (RAND_MAX);
	randColor1B = (float)rand() / (RAND_MAX);
	randColor2R = (float)rand() / (RAND_MAX);
	randColor2G = (float)rand() / (RAND_MAX);
	randColor2B = (float)rand() / (RAND_MAX);
}

void MainGame::createNewTerrain()
{
	terrain.clearData(); // clear the current terrain's data
	terrain.GenerateTerrain(100, 100); // generate a new terrain with 100 rows and 100 columns

	terrain.ExportToObj("..\\res\\terrain.obj"); // export the terrain to an obj file

	terrainMesh.loadModel("..\\res\\terrain.obj"); // load the new terrain model
}

void MainGame::createScreenQuad()
{
	float quadVertices[] = {
		// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f

		// vertex attributes for a quad that fills the top-left quarter of the screen
		//-1.0f,  1.0f,  0.0f, 1.0f,
		//-1.0f,  0.0f,  0.0f, 0.0f,
		// 0.0f,  0.0f,  1.0f, 0.0f,

		//-1.0f,  1.0f,  0.0f, 1.0f,
		// 0.0f,  0.0f,  1.0f, 0.0f,
		// 0.0f,  1.0f,  1.0f, 1.0f
	};
	// cube VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

}

void MainGame::gameLoop() // run input and draw functions while game is running
{
	while (_gameState != GameState::EXIT)
	{
		processInput();
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event evnt; // create an event variable to process events

	while(SDL_PollEvent(&evnt)) // get and process events
	{
		ImGui_ImplSDL2_ProcessEvent(&evnt); // process events for ImGui
		switch (evnt.type)
		{
		case SDL_QUIT: // if quit event is triggered, exit game
			_gameState = GameState::EXIT;
			break;
		}
	}
	
}

void MainGame::linkEmapping()
{
	glEnable(GL_BLEND); // enable blending for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // set blending function
	eMapping.Bind(); // bind the shader
	// set shader variables
	eMapping.setMat4("projection", myCamera.getProjection());
	eMapping.setMat4("view", myCamera.getView());
	eMapping.setMat4("model", transform.GetModel());
	eMapping.setVec3("cameraPos", myCamera.getPos());

	eMapping.Update(transform, myCamera); // update the shader
}

void MainGame::linkNoise()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	noiseShader.Bind();
	noiseShader.setFloat("time", counter / randTime);
	noiseShader.setFloat("fogDensity", 0.5);
	noiseShader.setVec3("fogColor", 0.0f, 0.0f, 0.5f);
	noiseShader.setFloat("maxDist", 10.0f);
	noiseShader.setFloat("minDist", 0.0f);
	noiseShader.setInt("randFreqX", randFreqX);
	noiseShader.setInt("randFreqY", randFreqY);
	noiseShader.setFloat("randTexCoords", randTexCoords);
	noiseShader.setFloat("randOffset", randOffset);
	noiseShader.setFloat("randColor1R", randColor1R);
	noiseShader.setFloat("randColor1G", randColor1G);
	noiseShader.setFloat("randColor1B", randColor1B);
	noiseShader.setFloat("randColor2R", randColor2R);
	noiseShader.setFloat("randColor2G", randColor2G);
	noiseShader.setFloat("randColor2B", randColor2B);
	GLuint t1L = glGetUniformLocation(noiseShader.getID(), "texture1"); // texture 1 location
	GLuint t2L = glGetUniformLocation(noiseShader.getID(), "texture2"); // texture 2 location

	// set textures
	glActiveTexture(GL_TEXTURE0); // set active texture unit
	glBindTexture(GL_TEXTURE_2D, texture.getID()); // bind the texture using the texture ID
	glUniform1i(t1L, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1.getID());
	glUniform1i(t2L, 1);

	noiseShader.Update(transform, myCamera);
}

void MainGame::linkExploding() 
{
	geoShader.Bind();
	geoShader.setFloat("time", counter);

	GLuint t1L = glGetUniformLocation(geoShader.getID(), "diffuse");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2.getID());
	glUniform1i(t1L, 0);

	geoShader.Update(transform, myCamera);
}

void MainGame::drawModels()
{
	linkEmapping(); // link the environment mapping shader for the mesh
	// set dimensions and position of the mesh
	transform.SetPos(glm::vec3(0.0, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, 10, 0.0));
	transform.SetScale(glm::vec3(1.2, 1.2, 1.2));
	mesh1.draw(); // draw the mesh
	mesh1.updateSphereData(glm::vec3(0.0, 0.0, 0.0)); // update the sphere data for the mesh for setting camera targets

	linkNoise(); // link the noise shader for the mesh
	transform.SetPos(glm::vec3(5.0, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, 10, 0.0));
	transform.SetScale(glm::vec3(1.2, 1.2, 1.2));
	mesh2.draw();
	mesh2.updateSphereData(glm::vec3(5.0, 0.0, 0.0));

	linkExploding(); // link the exploding shader for the mesh
	transform.SetPos(glm::vec3(-5.0, 0.0, 0.0));
	transform.SetRot(glm::vec3(0.0, 10, 0.0));
	transform.SetScale(glm::vec3(1.2, 1.2, 1.2));
	mesh3.draw();
	mesh3.updateSphereData(glm::vec3(-5.0, 0.0, 0.0));
}

void MainGame::drawSkyBox()
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.textureID); // bind the texture as a cube map to the skybox using the texture ID

	skybox.draw(&myCamera); // draw the skybox

	glEnableClientState(GL_COLOR_ARRAY);
	glEnd();
}

void MainGame::bindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO); // bind the framebuffer
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the color and depth buffer
}

void MainGame::unbindFBO()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // unbind the framebuffer
}

void MainGame::generateFBO(float w, float h)
{
	glGenFramebuffers(1, &FBO); // generate a framebuffer object
	glBindFramebuffer(GL_FRAMEBUFFER, FBO); // bind the framebuffer object

	// create a colorbuffer for attachment texture
	glGenTextures(1, &CBO);
	glBindTexture(GL_TEXTURE_2D, CBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBO, 0);

	// create a renderbuffer object for depth and stencil attachment 
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h); // use a single renderbuffer object for both a depth and stencil buffer
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO); // attach the renderbuffer object to the framebuffer

	// create ping-pong framebuffers for blurring
	glGenFramebuffers(2, PPFBO); // generate 2 framebuffers
	glGenTextures(2, PPB); // generate 2 textures
	for (unsigned int i = 0; i < 2; i++) // for each framebuffer
	{
		glBindFramebuffer(GL_FRAMEBUFFER, PPFBO[i]); // bind the framebuffer
		glBindTexture(GL_TEXTURE_2D, PPB[i]); // bind the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL); // create a texture for the framebuffer
		// set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PPB[i], 0); // attach the texture to the framebuffer
	}

	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer is complete!" << endl;
}

void MainGame::renderFBO()
{
	glClear(GL_COLOR_BUFFER_BIT); // clear the color buffer

	FBOShader.Bind(); // bind the framebuffer shader
	glBindVertexArray(quadVAO); // bind the quad VAO
	glBindTexture(GL_TEXTURE_2D, CBO);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6); // draw the quad
}

void MainGame::applyBlur()
{
	bool horizontal = true; // set horizontal to true so then we can switch between horizontal and vertical blurring
	bool first_iteration = true; // set first iteration to true to only bind the color buffer once and then the ping pong buffer subsequent times
	int amount = 10; // set the amount of blurring iterations (5 horizontal and 5 vertical)
	blurShader.Bind(); // bind the blur shader
	for (unsigned int i = 0; i < amount; i++) // for the amount of blurring iterations
	{
		glBindFramebuffer(GL_FRAMEBUFFER, PPFBO[horizontal]); // bind the framebuffer using the ping pong frame buffer object
		blurShader.setInt("horizontal", horizontal); // set the horizontal variable to determine the direction of the blur
		glBindTexture(GL_TEXTURE_2D, first_iteration ? CBO : PPB[!horizontal]); // bind the color buffer if it is the first iteration, otherwise bind the ping pong buffer
		glBindVertexArray(quadVAO); // bind the quad VAO
		glDrawArrays(GL_TRIANGLES, 0, 6); // draw the quad
		horizontal = !horizontal; // switch the horizontal variable to switch direction of blur
		// if first iteration is done, then set first iteration to false
		if (first_iteration)
			first_iteration = false;
	}
}

void MainGame::combineBloom()
{
	bloomShader.Bind(); // bind the bloom shader
	glActiveTexture(GL_TEXTURE0); // set active texture unit
	glBindTexture(GL_TEXTURE_2D, CBO); // bind the color buffer texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, PPB[1]); // bind the ping pong buffer texture
	bloomShader.setInt("scene", 0); // set the scene variable to 0
	bloomShader.setInt("bloomBlur", 1); // set the bloom blur variable to 1
	bloomShader.setFloat("bloomIntensity", bloomIntensity); // set the bloom intensity variable by GUI
	glBindVertexArray(quadVAO); // bind the quad VAO
	glDrawArrays(GL_TRIANGLES, 0, 6); // draw the quad
}

void MainGame::drawTerrain()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	terrainShader.Bind();
	terrainShader.setMat4("projection", myCamera.getProjection());
	terrainShader.setMat4("view", myCamera.getView());
	terrainShader.setMat4("model", transform.GetModel());
	terrainShader.setVec3("lightPos", lightPosition); // set light position by GUI
	GLuint t1L = glGetUniformLocation(terrainShader.getID(), "texture");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTexture.getID()); // set texture ID by GUI
	glUniform1i(t1L, 0);

	terrainShader.Update(transform, myCamera);

	transform.SetPos(glm::vec3(0.0, -20.0, 0.0));
	transform.SetRot(glm::vec3(0.0, 0.0, 0.0));
	transform.SetScale(glm::vec3(1.0, 1.0, 1.0));
	terrainMesh.draw();
	terrainMesh.updateSphereData(glm::vec3(0.0, 0.0, 0.0));
}

void MainGame::renderGUI() 
{
	// creates a frame for the GUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// creates a window for controlling settings
	ImGui::Begin("Settings");
	ImGui::SetWindowSize(ImVec2(400, 300));
	ImGui::SetWindowPos(ImVec2(300, 100));
	// scene control
	const char* scenes[] = { "Three Meshes Scene", "Noise Terrain Scene" };
	static int scene_current = 0;
	if (ImGui::Combo("Scene", &scene_current, scenes, IM_ARRAYSIZE(scenes)))
	{
		switch (scene_current)
		{
		case 0:
			scene = 0;
			break;
		case 1:
			scene = 1;
			break;
		}
	}
	// specific settings depending on the scene
	ImGui::Text("Scene Settings");
	if (scene == 0) 
	{
		// change the camera's target to the selected mesh
		const char* items[] = { "Noise Mesh", "Exploding Mesh", "Reflective Mesh" };
		static int item_current = 0;
		if (ImGui::Combo("Camera Target", &item_current, items, IM_ARRAYSIZE(items)))
		{
			switch (item_current)
			{
			case 0:
				cameraTarget = glm::vec3(mesh1.getSpherePos().x, mesh1.getSpherePos().y, mesh1.getSpherePos().z);
				break;
			case 1:
				cameraTarget = glm::vec3(mesh2.getSpherePos().x, mesh2.getSpherePos().y, mesh2.getSpherePos().z);
				break;
			case 2:
				cameraTarget = glm::vec3(mesh3.getSpherePos().x, mesh3.getSpherePos().y, mesh3.getSpherePos().z);
				break;
			}
		}
		ImGui::SliderFloat("Animation Speed", &animationSpeed, 0.0f, 5.0f); // control the animation speed (exploding mesh's explosion rate and noise mesh's animated effect)
		ImGui::SliderFloat("Bloom Intensity", &bloomIntensity, 0.0f, 5.0f); // control the bloom intensity
	}
	else if (scene == 1)
	{
		// change the texture of the terrain
		const char* textures[] = { "Grass", "Stone", "Water" };
		static int texture_current = 0;
		if (ImGui::Combo("Texture", &texture_current, textures, IM_ARRAYSIZE(textures)))
		{
			switch (texture_current)
			{
			case 0:
				terrainTexture = texture3;
				break;
			case 1:
				terrainTexture = texture4;
				break;
			case 2:
				terrainTexture = texture5;
				break;
			}
		}
		// control light position
		ImGui::SliderFloat("Light Position X", &lightPosition.x, -50.0f, 50.0f);
		ImGui::SliderFloat("Light Position Y", &lightPosition.y, 0.0f, 100.0f);
		ImGui::SliderFloat("Light Position Z", &lightPosition.z, -50.0f, 50.0f);
		ImGui::SliderFloat("Minimum Height", &terrain.min, 0.0f, 40.0f); // control the minimum height of the terrain
		ImGui::SliderFloat("Maximum Height", &terrain.max, 0.0f, 40.0f); // control the maximum height of the terrain
		ImGui::SliderFloat("Roughness", &terrain.modifier, 0.0f, 0.1f); // control the roughness of the terrain (the amount of peaks and valleys)
		// button to generate a new terrain
		ImGui::Button("Generate Terrain (Performance Cost!)");
		if (ImGui::IsItemClicked())
		{
			createNewTerrain();
		}
	}
	// camera settings
	ImGui::Text("Camera Settings");
	ImGui::SliderFloat("Orbit Speed", &orbitSpeed, 0.0f, 1.0f); // control how fast the camera orbits around it's target object

	ImGui::End(); // end the settings window

	// render the GUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainGame::drawGame()
{
	// scale for frame rate
    static Uint32 lastTime = SDL_GetTicks();
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

	counter += animationSpeed * deltaTime; // increment the counter for the animation speed by the delta time

    _gameDisplay.clearDisplay(0.8f, 0.8f, 0.8f, 1.0f); // sets background colour

	// scene 0 is the three meshes scene
	if (scene == 0) 
	{
		bindFBO();

		drawModels();
		drawSkyBox();

		// default framebuffer
		//renderFBO();

		// blur effect
		applyBlur();

		unbindFBO();

		// bloom effect
		combineBloom();

		// can be used to create a seperate window without the framebuffer effects (requires resizing the quad, only works with default framebuffer)
		//drawModels();
		//glEnable(GL_DEPTH_TEST);
		//drawSkyBox();

		// object camera will look towards
		myCamera.target = glm::vec3(cameraTarget);

		// set camera position
		//myCamera.setPos(glm::vec3(mesh1.getSpherePos().x, mesh1.getSpherePos().y, mesh1.getSpherePos().z - 20));

		// orbit camera around target
		myCamera.Orbit(orbitSpeed * deltaTime, 20.0f);
	}
	// scene 1 is the noise terrain scene
	else if (scene == 1)
	{
		drawTerrain();

		myCamera.target = glm::vec3(terrainMesh.getSpherePos());

		//myCamera.setPos(glm::vec3(0, 70, -10));

		myCamera.Orbit(orbitSpeed * deltaTime, 70.0f);
	}

    renderGUI();

    _gameDisplay.swapBuffer();    
}