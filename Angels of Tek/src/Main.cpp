#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Utils/Shader.h>
#include <Utils/Camera.h>
#include <Utils/Model.h>
#include <Utils/Geometry.h>
#include <Utils/Level.h>

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
void setPerFrameUniforms(Shader* shader, Camera& camera/*, DirectionalLight& dirL, PointLight& pointL*/);

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);

// globals
static bool _wireframe = false;
static bool _culling = true;
int score = 0;
Level level("😡");

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	//engine->play2D("assets/geile mukke ballern/LMFAO - Party Rock Anthem.mp3");


	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
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
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// configure camera settings
	// -----------------------------
	camera.MovementSpeed = 3.0f;
	camera.MouseSensitivity = 1.5f;

	// SIMON SHADER
	Shader basicShader("basic.vert", "basic.frag");
	Shader basicShader2("basic.vert", "basic.frag");


	// load models
	// -----------
	//Model ourModel("assets/models/nanosuit/nanosuit.obj");
	//Model ourModel("assets/models/IronMan/IronMan.obj");
	//Model ourModel("assets/models/bugatti/bugatti.obj");
	//Model ourModel("assets/models/dennis/rp_dennis_posed_004_30k_native.obj");

	// EXPERIMENTAL SIMON CUBEZ
	Material cubePhongMaterial(&basicShader, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	Geometry cubePhong = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f), &cubePhongMaterial);
	Material cubePhongMaterial2(&basicShader, glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.7f, 0.1f), 2.0f);
	Geometry cubePhong2 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(1.0f, 1.5f, 0.5f), &cubePhongMaterial2);

	vector<Geometry> testicles;

	for (int i = 0; i < level.level1.size(); i++)
	{
		glm::vec4 pos = level.level1.at(i);
		testicles.push_back(Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, 0.0f, pos.y)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f), &cubePhongMaterial));
	}

	Geometry WtfOhneDemCubeGehtDasProgrammNichtKannstDuMirDasErklärenSiomonWesp (Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(0.5f, 0.5f, 0.5f), &cubePhongMaterial));


	//// load textures (we now use a utility function to keep the code more organized)
	//// -----------------------------------------------------------------------------
	//unsigned int specularMap = loadTexture("assets/textures/container2_specular.png");

	GLuint containerTextureID = loadTexture("assets/textures/container.jpg");
	GLuint containerTextureID2 = loadTexture("assets/textures/container2.png");

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		camera.ProcessKeyboard(FORWARD, deltaTime);

		//std::cout << camera.Position.z << std::endl;

		// Collision / win condition
		//if (camera.Position.z < -10) {
		//	std::cout << "Win" << std::endl;
		//	glfwSetWindowTitle(window, "Win");
		//	//break;
		//}

		if (level.collision(camera)){
			std::cout << "Lose" << std::endl;
			glfwSetWindowTitle(window, "Lose");
			break;
		}
			

		// input
		// -----
		glfwPollEvents();

		// render
		// ------
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// BIND ME
		glBindTexture(GL_TEXTURE_2D, containerTextureID);

		// RENDER ME
		setPerFrameUniforms(&basicShader, camera);
		//cubePhong.draw();

		glBindTexture(GL_TEXTURE_2D, containerTextureID2);
		//cubePhong2.draw();
		//testicles.at(0).draw();

		for (int i = 0; i < testicles.size(); i++)
		{
			testicles.at(i).draw();
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void setPerFrameUniforms(Shader* shader, Camera& camera/*, DirectionalLight& dirL, PointLight& pointL*/)
{
	shader->use();
	shader->setMat4("viewProjMatrix", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f) * camera.GetViewMatrix());
	shader->setVec3("cameraWorldPosition", camera.Position);
	//shader->setVec3("viewPos", camera.Position);
	

	/*shader->setUniform("dirL.color", dirL.color);
	shader->setUniform("dirL.direction", dirL.direction);
	shader->setUniform("pointL.color", pointL.color);
	shader->setUniform("pointL.position", pointL.position);
	shader->setUniform("pointL.attenuation", pointL.attenuation);*/
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

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
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit

	if (action != GLFW_PRESS) return;

	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_F1:
		_wireframe = !_wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
		break;
	case GLFW_KEY_F2:
		_culling = !_culling;
		if (_culling) glEnable(GL_CULL_FACE);
		else glDisable(GL_CULL_FACE);
		break;
	case GLFW_KEY_W:
		//camera.ProcessKeyboard(FORWARD, deltaTime);
		break;
	case GLFW_KEY_S:
		//camera.ProcessKeyboard(BACKWARD, deltaTime);
		break;
	case GLFW_KEY_A:
		camera.ProcessKeyboard(LEFT, deltaTime);
		break;
	case GLFW_KEY_D:
		camera.ProcessKeyboard(RIGHT, deltaTime);
		break;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
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
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

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

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
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
