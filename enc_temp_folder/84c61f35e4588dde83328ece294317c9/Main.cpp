﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h""
#include "Model.h"
#include "Geometry.h"
#include "Level.h"

#include <iostream>
#include <sstream>
#include <irrklang/irrKlang.h>


// prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(const char *path);
void moveMoveableObject(Geometry& obj);
void setPerFrameUniforms(Shader* shader, Camera& camera/*, DirectionalLight& dirL, PointLight& pointL*/);
void teleportRoom();

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);

// globals
static bool _wireframe = false;
static bool _culling = true;
int score = 0;
Level level("😡", 200.0f, 4.0f);
float movingObjPos = 0.5f;
int temp = 1;
glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
bool pause = true;
int life = 95;
bool room = false;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 768;
float brightness = 1.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.3f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Angels of Tek", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// configure camera settings
	camera.MovementSpeed = 4.0f;
	camera.MouseSensitivity = 1.5f;

	// load shader & set up texture positions
	Shader basicShader("pbr.vert", "pbr.frag");
	Shader oldBasicShader("model.vert", "model.frag");
	Shader planesWalker("simon.fag", "yannic.geil");
	Shader himmerlblau("simon - Kopie.fag", "yannic - Kopie.geil");
	

	basicShader.use();
	basicShader.setInt("albedoMap", 0);
	basicShader.setInt("normalMap", 1);
	basicShader.setInt("metallicMap", 2);
	basicShader.setInt("roughnessMap", 3);
	basicShader.setInt("aoMap", 4);

	GLuint albedo = loadTexture("assets/textures/pbr/Titanium-Scuffed_basecolor.png");
	GLuint normal = loadTexture("assets/textures/pbr/Titanium-Scuffed_normal.png");
	GLuint metallic = loadTexture("assets/textures/pbr/Titanium-Scuffed_metallic.png");
	GLuint roughness = loadTexture("assets/textures/pbr/Titanium-Scuffed_roughness.png");
	GLuint ao = loadTexture("assets/textures/pbr/Titanium-Scuffed_ao.png");

	// lights
	// ------
	glm::vec3 lightPositions[20];
	glm::vec3 lightColors[20];
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	// initialize static shader uniforms before rendering
	// --------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	basicShader.use();
	basicShader.setMat4("viewProjMatrix", projection);

	// load & position model
	Model ourModel("assets/models/nanosuit/nanosuit.obj");
	Model hammer("assets/models/hammer/12221_Cat_v1_l3.obj");

	// generate Materials
	Material cubePhongMaterial(&basicShader, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	Material cubePhongMaterial2(&basicShader, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	Material polaneswalkerMaterial(&planesWalker, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	Material himmerlblauMaterial(&himmerlblau, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	
	// generate lanes
	Geometry lane1 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.4f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 1000.0f), &cubePhongMaterial2);
	Geometry lane2 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -0.4f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 1000.0f), &cubePhongMaterial2);
	Geometry lane3 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.4f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 1000.0f), &cubePhongMaterial2);
	Geometry lane4 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.4f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 1000.0f), &cubePhongMaterial2);
	Geometry lane5 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.4f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 1000.0f), &cubePhongMaterial2);

	// create plane
	int width = 100;
	int height = 8000;
	Geometry plane = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5 * (width - 1), -1, -200)), Geometry::createPlaneGeometry(width, height), &polaneswalkerMaterial);
	Geometry sky = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5 * (width - 1), 5, -200)), Geometry::createPlaneGeometry(width, height), &himmerlblauMaterial);

	// moving cube
	Geometry movableObjectThatIsNotASimpleFirstPersonCamera = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.50f, -40.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f), &cubePhongMaterial2);

	// generate obstacles
	vector<Geometry> testicles;
	for (int i = 0; i < level.level1.size(); i++)
	{
		glm::vec4 pos = level.level1.at(i);
		testicles.push_back(Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, 0.0f, pos.y)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), &cubePhongMaterial));
	}

	Geometry WtfOhneDemCubeGehtDasProgrammNichtKannstDuMirDasErklärenSiomonWesp (Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(0.5f, 0.5f, 0.5f), &cubePhongMaterial));

	// Cheat R00m Kugel
	Geometry showcase (Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)), Geometry::createCubeGeometry(0.5, 0.5, 0.5), &cubePhongMaterial));

	// load cube textures
	GLuint containerTextureID = loadTexture("assets/textures/container.jpg");
	GLuint containerTextureID2 = loadTexture("assets/textures/container2.png");
	GLuint laneTexture= loadTexture("assets/textures/lane.png");

	// start sound engine
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	//engine->play2D("assets/geile mukke ballern/Helblinde - Gateway to Psycho.mp3");
	//engine->play2D("assets/geile mukke ballern/LMFAO - Party Rock Anthem.mp3");

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (!pause) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}

		// Score as window title
		//std::stringstream str;
		//str << level.coutner;
		//glfwSetWindowTitle(window, str.str().c_str());

		// Lifes as window title
		std::stringstream str;
		str << life;
		glfwSetWindowTitle(window, str.str().c_str());

		// Collision / win condition
		if (level.win()) {
			std::cout << "Win" << std::endl;
			glfwSetWindowTitle(window, "Win");
		}

		if (level.collision(camera)){
			life--;
		}

		if (life <= 0) {
			level.coutner = 0;
			camera.ProcessKeyboard(RESET, deltaTime);
			pause = true;
			life = 103;
		}

		// reset
		glfwPollEvents();
		glClearColor(0.0f, 0.4f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// PSYCHO MODE
		/*color.x = (rand() % 100) / 100.0f;
		color.y = (rand() % 100) / 100.0f;
		color.z = (rand() % 100) / 100.0f;
		glClearColor(color.x, color.y, color.z, color.w);*/
		

		oldBasicShader.use();
		setPerFrameUniforms(&oldBasicShader, camera);

		// move & draw model
		ourModel.resetModelMatrix();
		ourModel.transform(glm::rotate(glm::mat4(1.0f), -1.35f, glm::vec3(1.0f, 0.0f, 0.0f)));
		ourModel.transform(glm::scale(glm::mat4(1.0f), glm::vec3(0.05f, 0.05f, 0.05f)));
		ourModel.transform(glm::translate(glm::mat4(1.0f), glm::vec3(camera.Position.x, -0.05f, camera.Position.z)));
		ourModel.Draw(oldBasicShader);
		hammer.resetModelMatrix();
		hammer.transform(glm::rotate(glm::mat4(1.0f), -1.56f, glm::vec3(1.0f, 0.0f, 0.0f)));
		hammer.transform(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)));
		hammer.transform(glm::scale(glm::mat4(1.0f), glm::vec3(0.0015f, 0.0015f, 0.0015f)));
		hammer.transform(glm::translate(glm::mat4(1.0f), glm::vec3(camera.Position.x, 0.11, camera.Position.z - 0.5)));
		hammer.Draw(oldBasicShader);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, containerTextureID);
		moveMoveableObject(movableObjectThatIsNotASimpleFirstPersonCamera);
		movableObjectThatIsNotASimpleFirstPersonCamera.draw();
		glBindTexture(GL_TEXTURE_2D, containerTextureID2);

		basicShader.use();
		setPerFrameUniforms(&basicShader, camera);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);

		for (int i = 0; i < testicles.size(); i++)
		{
			testicles.at(i).draw();
		}

		showcase.resetModelMatrix();
		showcase.transform(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f)));
		showcase.transform(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)));
		showcase.transform(glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f)));
		showcase.transform(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
		if (room) {
			showcase.draw();
		}
		

		glm::vec3 pos = glm::vec3(0, 0.2 , 0);
		basicShader.setVec3("lightPositions[0]", pos);
		basicShader.setVec3("lightColors[0]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, 0.4, 0);
		basicShader.setVec3("lightPositions[1]", pos);
		basicShader.setVec3("lightColors[1]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, 0.6, 0);
		basicShader.setVec3("lightPositions[2]", pos);
		basicShader.setVec3("lightColors[2]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, 0.8, 0);
		basicShader.setVec3("lightPositions[3]", pos);
		basicShader.setVec3("lightColors[3]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, 1, 0);
		basicShader.setVec3("lightPositions[4]", pos);
		basicShader.setVec3("lightColors[4]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, -0.2, 0);
		basicShader.setVec3("lightPositions[5]", pos);
		basicShader.setVec3("lightColors[5]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, -0.4, 0);
		basicShader.setVec3("lightPositions[6]", pos);
		basicShader.setVec3("lightColors[6]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, -0.6, 0);
		basicShader.setVec3("lightPositions[7]", pos);
		basicShader.setVec3("lightColors[7]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, -0.8, 0);
		basicShader.setVec3("lightPositions[8]", pos);
		basicShader.setVec3("lightColors[8]", glm::vec3(150.0f, 150.0f, 150.0f));

		pos = glm::vec3(0, -1, 0);
		basicShader.setVec3("lightPositions[9]", pos);
		basicShader.setVec3("lightColors[9]", glm::vec3(150.0f, 150.0f, 150.0f));


		// draw lanes
		glBindTexture(GL_TEXTURE_2D, laneTexture);
		lane1.draw();
		lane2.draw();
		lane3.draw();
		lane4.draw();
		lane5.draw();

		planesWalker.use();
		setPerFrameUniforms(&planesWalker, camera);
		planesWalker.setFloat("u_time", glfwGetTime());
		plane.draw();

		himmerlblau.use();
		setPerFrameUniforms(&himmerlblau, camera);
		himmerlblau.setFloat("u_time", glfwGetTime());
		sky.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void setPerFrameUniforms(Shader* shader, Camera& camera/*, DirectionalLight& dirL, PointLight& pointL*/)
{
	shader->use();
	shader->setMat4("viewProjMatrix", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f) * camera.GetViewMatrix());
	shader->setVec3("cameraWorldPosition", camera.Position);
	shader->setFloat("prightness", brightness);
	//shader->setVec3("viewPos", camera.Position);
	

	/*shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);*/
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//Lass mas drinnan auch wenn nix drinnan is!
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F8 - Wireframe
	// F2 - Culling
	// Esc - Exit
	// Shift - SPeeeEEeeeD1!
	// Levt - nach links düsen
	// Rechts - nach rechts düsen
	// + - alles wird weißer
	// - - alles wird schwärzer
	// Print - Shwocase room

	if (action != GLFW_PRESS) return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_F8:
		_wireframe = !_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
		break;
	case GLFW_KEY_F2:
		_culling = !_culling;
		if (_culling) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;
	case GLFW_KEY_LEFT_SHIFT:
		camera.MovementSpeed += 1.0f;
		break;
	case GLFW_KEY_W:
		camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case GLFW_KEY_S:
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case GLFW_KEY_A:
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case GLFW_KEY_D:
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	case GLFW_KEY_RIGHT_BRACKET:
		brightness += 0.1;
		break;
	case GLFW_KEY_SLASH:
		brightness -= 0.1;
		break;
	case GLFW_KEY_SPACE:
		pause = !pause;
		break;
	case GLFW_KEY_PRINT_SCREEN:
		camera.ProcessKeyboard(RESET, deltaTime);
		room = !room;
		break;
	case GLFW_KEY_R:
		level.coutner = 0;
		life = 96;
		pause = true;
		camera.ProcessKeyboard(RESET, deltaTime);
		break;
	}

	std::cout << brightness << std::endl;
}

void teleportRoom() {

}

// make windows resizesable
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// utility function for moving the moveable object
void moveMoveableObject(Geometry& obj) {

	if (movingObjPos >= 20.0f)
	{
		temp = -1;
	}
	else if (movingObjPos <= -20.0f)
	{
		temp = 1;
	}

	movingObjPos += temp * 0.1f;
	//std::cout << movingObjPos << std::endl;
	//std::cout << temp << std::endl;
	obj.transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.01f * temp * deltaTime * 60.0f, 0.0f, 0.0f)));
}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const * path)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << ".txt" << std::endl;
		stbi_image_free(data);
	}

	// Aspirin 500mg ohne Mwst.
	return textureID;
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	switch (source) {
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}