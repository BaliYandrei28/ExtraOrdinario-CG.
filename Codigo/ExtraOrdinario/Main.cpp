#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//C++ standard libraries
#include <iostream>
#include <fstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "SOIL2/SOIL2.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Animation.h"
#include "Animator.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void AnimationKeys();
float ObtenerAlturaTerreno(float x, float z);


// Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1000;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(1.5f, 0.5f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 270.0f);
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

//Luces
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;
bool luzNaranja = false;
bool zorroCaminando = false;
float tiempoZorro = 0.0f;
float zorroPosZ = 0.0f;
float velocidadZorro = 0.15f;
// salto del zorro
bool zorroSaltando = false;
float tiempoSaltoZorro = 0.0f;
float duracionSaltoZorro = 1.0f;
float alturaMaximaSalto = 0.7f;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,2.0f, 0.0f)
};

#define MAX_FRAMES 20
typedef struct _frame {
	float rotGato;
	float rotGatoInc;
	float gatoPosX;
	float gatoPosY;
	float gatoPosZ;
	float incX;
	float incY;
	float incZ;
	float headGato;
	float headIncGato;
	float FLegsGato;
	float FLegsIncGato;
	float BLegsGato;
	float BLegsIncGato;
	float tailGato;
	float tailIncGato;
	float inclinacionGato;
	float inclinacionIncGato;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// === Animacion ciclo dia-noche ===
float sunAngle = 0.0f;
float sunSpeed = 0.2f;
float sunRadius = 10.0f;
bool dayNightCycle = true;
glm::vec3 sunPosition(0.0f);

// Intensidad luz puntual, se cambia con SPACE
glm::vec3 Light1 = glm::vec3(0);


// === Animaciones FBX (por esqueleto) ===
bool playFBX = false;
Animation* g_SteveAnim = nullptr;
Animator* g_SteveAnimator = nullptr;
// Animacion del caballo
Animation* g_CaballoAnim = nullptr;
Animator* g_CaballoAnimator = nullptr;
// Movimiento del caballo
float caballoPosX = 0.0f;
float velocidadCaballo = 0.35f;
bool caballoRegresando = false;
float steveSpeed = 1.0f;
int steveEstado = 0;
float steveDir = 1.0f;
float stevePosX = 0.0f, stevePosY = 0.0f, stevePosZ = 0.0f;


#define MAX_FRAMES 16
int i_max_steps = 140;
int i_curr_steps = 0;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Inicializacion de GLFW y creacion de ventana
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Extraordinario CG", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// === Carga de shaders ===
	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader fuegoShader("Shader/fuego.vs", "Shader/fuego.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
	Shader skyboxShader("Shader/SkyBox.vs", "Shader/SkyBox.frag");
	// Shader para dar color de bronce a la campana
	Shader campanaShader("Shader/campana.vs", "Shader/campana.frag");

	// Shader para los colores originales del canon
	Shader canonShader("Shader/canon.vs", "Shader/canon.frag");


	// === Carga de modelos ===

	// Piso con cuatro texturas
	Model Aldea((char*)"ModelsExtra/Floor_4Texturas.obj");


	Model tree((char*)"ModelsExtra/Tree/Tree.obj");
	// Zorro dividido para animar sus patas
	Model zorroCompleto((char*)"ModelsExtra/Zorro/zorroCompleto.obj");

	// Piezas del zorro
	Model zorroCabeza((char*)"ModelsExtra/Zorro/cabeza.obj");
	Model zorroCuerpo((char*)"ModelsExtra/Zorro/cuerpo.obj");
	Model zorroCola((char*)"ModelsExtra/Zorro/cola.obj");

	Model pataDerDelante((char*)"ModelsExtra/Zorro/pataDerDelante.obj");
	Model pataDerDetras((char*)"ModelsExtra/Zorro/pataDerDetras.obj");

	Model pataIzqDelante((char*)"ModelsExtra/Zorro/pataIzqDelante.obj");
	Model pataIzqDetras((char*)"ModelsExtra/Zorro/pataIzqDetras.obj");

	// Caballo animado
	Model caballo((char*)"ModelsExtra/Horse/Horse.obj");
	// Animacion por huesos del caballo
	Model caballoAnimado((char*)"ModelsExtra/Horse/Horse_walk.fbx");

	g_CaballoAnim = new Animation(
		"ModelsExtra/Horse/Horse_walk.fbx",
		&caballoAnimado
	);

	// Carreta y sus cuatro ruedas
	Model carretaCuerpo((char*)"ModelsExtra/Carreta/carreta_cuerpo.obj");

	Model ruedaFL((char*)"ModelsExtra/Carreta/rueda_fl.obj");
	Model ruedaFR((char*)"ModelsExtra/Carreta/rueda_fr.obj");
	Model ruedaRL((char*)"ModelsExtra/Carreta/rueda_rl.obj");
	Model ruedaRR((char*)"ModelsExtra/Carreta/rueda_rr.obj");

	Model barril((char*)"ModelsExtra/Barril/old-barrel.obj");

	Model caja((char*)"ModelsExtra/Cajas/wooden-crate.obj");

	Model tienda((char*)"ModelsExtra/Tienda/tentClosed.obj");


	// === POZO DE AGUA ===
	Model pozoBase((char*)"ModelsExtra/Pozo/pozo_base.obj");
	Model pozoCubeta((char*)"ModelsExtra/Pozo/pozo_cubeta.obj");

	Model campana((char*)"ModelsExtra/Campana/Bell.fbx");

	Model sacos((char*)"ModelsExtra/Sacos/SackTrench.fbx");

	Model farol((char*)"ModelsExtra/Farol/post_lantern.fbx");

	Model canon((char*)"ModelsExtra/Canon/Prop_Cannon.fbx");

	Model casa((char*)"ModelsExtra/Casa/Houses_FirstAge_1_Level2.fbx");

	Model tren((char*)"ModelsExtra/Tren/Locomotive_Front.fbx");

	Model nopal((char*)"ModelsExtra/Nopal/CactusFlowers_5.fbx");

	Model cerca((char*)"ModelsExtra/Cerca/Fence.fbx");

	GLuint texturaCanon = TextureFromFile(
		"Atlas_Pirate.png", "ModelsExtra/Canon");

	Model soporteCampana((char*)"ModelsExtra/Campana/soporte_campana.obj");


	// Fogata con cuadros de animacion
	Model fogataTroncos((char*)"ModelsExtra/Fuego/Llamas/troncos.obj");

	Model fuego0((char*)"ModelsExtra/Fuego/Llamas/llama_00.obj");
	Model fuego1((char*)"ModelsExtra/Fuego/Llamas/llama_01.obj");
	Model fuego2((char*)"ModelsExtra/Fuego/Llamas/llama_02.obj");
	Model fuego3((char*)"ModelsExtra/Fuego/Llamas/llama_03.obj");
	Model fuego4((char*)"ModelsExtra/Fuego/Llamas/llama_04.obj");
	Model fuego5((char*)"ModelsExtra/Fuego/Llamas/llama_05.obj");
	Model fuego6((char*)"ModelsExtra/Fuego/Llamas/llama_06.obj");
	Model fuego7((char*)"ModelsExtra/Fuego/Llamas/llama_07.obj");

	Model* cuadrosFuego[8] = {
		&fuego0, &fuego1, &fuego2, &fuego3,
		&fuego4, &fuego5, &fuego6, &fuego7
	};

	g_CaballoAnimator = new Animator(g_CaballoAnim);

	g_CaballoAnimator->PlayAnimation(g_CaballoAnim);


	GLuint texturaCaballo = SOIL_load_OGL_texture(
		"ModelsExtra/Horse/textures/Horse Brown.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	Model Steve((char*)"ModelsExtra/Steve/steveAnimated.fbx");
	g_SteveAnim = new Animation("ModelsExtra/Steve/steveAnimated.fbx", &Steve);
	g_SteveAnimator = new Animator(g_SteveAnim);
	g_SteveAnimator->Reset();


	// Vertices skybox
	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

	//Skybox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// Load textures
	vector < const GLchar*> faces;
	faces.push_back("SkyBox/right.jpg");
	faces.push_back("SkyBox/left.jpg");
	faces.push_back("SkyBox/top.jpg");
	faces.push_back("SkyBox/bottom.jpg");
	faces.push_back("SkyBox/back.jpg");
	faces.push_back("SkyBox/front.jpg");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);


	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// === Loop para renderizar la escena ===
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		sunPosition = glm::vec3(0.0f, 10.0f, 0.0f);
		float dayFactor = 0.5f;

		// Dirección de la luz hacia abajo
		glm::vec3 sunDir = glm::vec3(0.0f, -1.0f, 0.0f);

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		AnimationKeys();


		// Clear the colorbuffer

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);


		glm::mat4 modelTemp = glm::mat4(1.0f); //Temp


		// Use corresponding shader when setting uniforms/drawing objects
		lightingShader.Use();


		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Luz direccional blanca o naranja (L)

		glm::vec3 colorLuz;

		if (luzNaranja)
			colorLuz = glm::vec3(1.0f, 0.5f, 0.0f);
		else
			colorLuz = glm::vec3(1.0f, 1.0f, 1.0f);

		float intensidadLuz = 0.5f;

		glm::vec3 ambient = colorLuz * 0.15f;
		glm::vec3 diffuse = colorLuz * intensidadLuz;
		glm::vec3 specular = colorLuz * intensidadLuz;


		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), sunDir.x, sunDir.y, sunDir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), ambient.x, ambient.y, ambient.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), specular.x, specular.y, specular.z);


		// Tres luces RGB que se controlan con R, G y B

		static bool lucesRGB[3] = { true, true, true };
		static bool teclasRGBAnteriores[3] = { false, false, false };

		int teclasRGB[3] = {
			GLFW_KEY_R,
			GLFW_KEY_G,
			GLFW_KEY_B
		};

		glm::vec3 coloresRGB[3] = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
		};

		glm::vec3 posicionesRGB[3] = {
			glm::vec3(0.0f, 4.0f, -2.0f),
			glm::vec3(1.5f, 4.0f, -4.0f),
			glm::vec3(3.0f, 4.0f, -2.0f)
		};

		// Luces dirigidas hacia la zona del caballo
		glm::vec3 objetivoRGB(1.5f, 0.8f, -2.0f);

		for (int i = 0; i < 3; i++)
		{
			bool presionada =
				glfwGetKey(window, teclasRGB[i]) == GLFW_PRESS;

			if (presionada && !teclasRGBAnteriores[i])
			{
				lucesRGB[i] = !lucesRGB[i];
			}

			teclasRGBAnteriores[i] = presionada;

			std::string nombre =
				"spotLights[" + std::to_string(i) + "].";

			glm::vec3 direccion = glm::normalize(
				objetivoRGB - posicionesRGB[i]);

			glm::vec3 intensidad = lucesRGB[i]
				? coloresRGB[i] * 1.5f
				: glm::vec3(0.0f);

			auto enviarVector = [&](const char* propiedad,
				const glm::vec3& valor)
				{
					std::string uniforme = nombre + propiedad;

					glUniform3fv(
						glGetUniformLocation(
							lightingShader.Program,
							uniforme.c_str()),
						1, glm::value_ptr(valor));
				};

			enviarVector("position", posicionesRGB[i]);
			enviarVector("direction", direccion);

			enviarVector("ambient", glm::vec3(0.0f));
			enviarVector("diffuse", intensidad);
			enviarVector("specular", intensidad * 0.35f);

			glUniform1f(
				glGetUniformLocation(
					lightingShader.Program,
					(nombre + "cutOff").c_str()),
				glm::cos(glm::radians(22.5f)));

			glUniform1f(
				glGetUniformLocation(
					lightingShader.Program,
					(nombre + "outerCutOff").c_str()),
				glm::cos(glm::radians(30.0f)));

			glUniform1f(
				glGetUniformLocation(
					lightingShader.Program,
					(nombre + "constant").c_str()), 1.0f);

			glUniform1f(
				glGetUniformLocation(
					lightingShader.Program,
					(nombre + "linear").c_str()), 0.09f);

			glUniform1f(
				glGetUniformLocation(
					lightingShader.Program,
					(nombre + "quadratic").c_str()), 0.032f);
		}


		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);


		glm::mat4 view;
		view = camera.GetViewMatrix();

		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "useBones"), 0);

		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Aldea.Draw(lightingShader);


		// Caballo: animacion por huesos y recorrido de ida y vuelta

		g_CaballoAnimator->UpdateAnimation(deltaTime);

		// Movimiento de ida y regreso
		if (!caballoRegresando)
		{
			caballoPosX += velocidadCaballo * deltaTime;

			if (caballoPosX >= 3.0f)
			{
				caballoPosX = 3.0f;
				caballoRegresando = true;
			}
		}
		else
		{
			caballoPosX -= velocidadCaballo * deltaTime;

			if (caballoPosX <= 0.0f)
			{
				caballoPosX = 0.0f;
				caballoRegresando = false;
			}
		}


		// Aplicar matrices de huesos al shader
		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 1);

		const auto& huesosCaballo =
			g_CaballoAnimator->GetFinalBoneMatrices();

		for (int i = 0; i < (int)huesosCaballo.size(); i++)
		{
			std::string nombreHueso =
				"finalBonesMatrices[" + std::to_string(i) + "]";

			glUniformMatrix4fv(
				glGetUniformLocation(
					lightingShader.Program,
					nombreHueso.c_str()),
				1,
				GL_FALSE,
				glm::value_ptr(huesosCaballo[i])
			);
		}

		model = glm::mat4(1.0f);
		float alturaCaballo =
			ObtenerAlturaTerreno(caballoPosX, -2.0f);

		model = glm::translate(model,
			glm::vec3(caballoPosX, alturaCaballo + 0.1f, -2.0f));

		float rotacionCaballo = caballoRegresando ? -90.0f : 90.0f;

		model = glm::rotate(model,
			glm::radians(rotacionCaballo),
			glm::vec3(0.0f, 1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.003f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(model));


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturaCaballo);

		glUniform1i(
			glGetUniformLocation(lightingShader.Program, "material.diffuse"),
			0
		);

		glUniform1i(
			glGetUniformLocation(lightingShader.Program, "material.specular"),
			0
		);

		caballoAnimado.Draw(lightingShader);

		glBindTexture(GL_TEXTURE_2D, 0);


		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);


		// Carreta con ruedas animadas

		static float giroRuedas = 0.0f;
		static float carretaPosZ = -3.0f;
		static bool carretaRegresando = false;

		// C pausa y V reanuda la carreta
		static bool carretaPausada = false;

		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		{
			carretaPausada = true;
		}

		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		{
			carretaPausada = false;
		}

		float velocidadCarreta = 0.35f;
		float posicionAnterior = carretaPosZ;

		if (!carretaPausada)
		{
			if (!carretaRegresando)
			{
				carretaPosZ -= velocidadCarreta * deltaTime;

				if (carretaPosZ <= -4.5f)
				{
					carretaPosZ = -4.5f;
					carretaRegresando = true;
				}
			}
			else
			{
				carretaPosZ += velocidadCarreta * deltaTime;

				if (carretaPosZ >= -3.0f)
				{
					carretaPosZ = -3.0f;
					carretaRegresando = false;
				}
			}
		}

		// Giro de las ruedas segun el desplazamiento
		float desplazamiento = carretaPosZ - posicionAnterior;

		giroRuedas -= glm::degrees(desplazamiento / 0.18f);


		model = glm::mat4(1.0f);

		float alturaCarreta =
			ObtenerAlturaTerreno(-4.0f, carretaPosZ);

		model = glm::translate(model,
			glm::vec3(-4.0f, alturaCarreta, carretaPosZ));
		model = glm::scale(model, glm::vec3(0.4f));


		glm::mat4 matrizCarreta = model;

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizCarreta));

		carretaCuerpo.Draw(lightingShader);

		// Rotacion de las cuatro ruedas
		auto dibujarRueda = [&](Model& rueda,
			glm::vec3 pivote)
			{
				glm::mat4 modeloRueda = matrizCarreta;

				modeloRueda = glm::translate(modeloRueda, pivote);

				modeloRueda = glm::rotate(modeloRueda,
					glm::radians(giroRuedas),
					glm::vec3(1.0f, 0.0f, 0.0f));

				modeloRueda = glm::translate(modeloRueda, -pivote);

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
					glm::value_ptr(modeloRueda));

				rueda.Draw(lightingShader);
			};

		dibujarRueda(ruedaFL,
			glm::vec3(0.931250f, 0.500000f, 0.647284f));

		dibujarRueda(ruedaFR,
			glm::vec3(-0.813750f, 0.500000f, 0.647284f));

		dibujarRueda(ruedaRL,
			glm::vec3(0.961250f, 0.680000f, -1.802716f));

		dibujarRueda(ruedaRR,
			glm::vec3(-0.843750f, 0.680000f, -1.802716f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizCarreta));


		// Barril
		model = glm::mat4(1.0f);

		float alturaBarril =
			ObtenerAlturaTerreno(-5.2f, -1.5f);

		model = glm::translate(model,
			glm::vec3(-5.2f, alturaBarril, -1.5f));

		model = glm::scale(model, glm::vec3(0.55f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(model));

		barril.Draw(lightingShader);


		// Caja de madera
		model = glm::mat4(1.0f);

		float alturaCaja =
			ObtenerAlturaTerreno(-4.45f, -1.5f);

		model = glm::translate(model,
			glm::vec3(-4.45f, alturaCaja, -1.5f));

		model = glm::scale(model, glm::vec3(0.7f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(model));

		caja.Draw(lightingShader);


		// Fogata animada

		float fuegoPosX = -3.0f;
		float fuegoPosZ = -1.1f;

		float alturaFogata =
			ObtenerAlturaTerreno(fuegoPosX, fuegoPosZ);

		glm::mat4 matrizFogata = glm::mat4(1.0f);

		matrizFogata = glm::translate(matrizFogata,
			glm::vec3(fuegoPosX, alturaFogata + 0.03f, fuegoPosZ));

		matrizFogata = glm::scale(matrizFogata,
			glm::vec3(0.65f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);


		glm::mat4 matrizTroncos = glm::scale(
			matrizFogata, glm::vec3(0.60f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizTroncos));

		fogataTroncos.Draw(lightingShader);


		// Dibujar las llamas con transparencia
		int cuadroActual = (int)(glfwGetTime() * 10.0f) % 8;

		GLboolean cullActivo = glIsEnabled(GL_CULL_FACE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);

		fuegoShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(fuegoShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(fuegoShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniformMatrix4fv(
			glGetUniformLocation(fuegoShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizFogata));

		glUniform1i(
			glGetUniformLocation(fuegoShader.Program, "texture_diffuse1"), 0);

		cuadrosFuego[cuadroActual]->Draw(fuegoShader);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		if (cullActivo)
			glEnable(GL_CULL_FACE);

		lightingShader.Use();


		// Tienda de campana

		float tiendaPosX = -1.4f;
		float tiendaPosZ = -4.2f;

		float alturaTienda =
			ObtenerAlturaTerreno(tiendaPosX, tiendaPosZ);

		glm::mat4 matrizTienda = glm::mat4(1.0f);

		matrizTienda = glm::translate(matrizTienda,
			glm::vec3(tiendaPosX, alturaTienda + 0.03f, tiendaPosZ));

		matrizTienda = glm::scale(matrizTienda,
			glm::vec3(3.0f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizTienda));

		tienda.Draw(lightingShader);


		// Pozo con cubeta animada

		float pozoPosX = 3.2f;
		float pozoPosZ = -4.5f;

		float alturaPozo = ObtenerAlturaTerreno(pozoPosX, pozoPosZ);

		glm::mat4 matrizPozo = glm::mat4(1.0f);

		matrizPozo = glm::translate(matrizPozo,
			glm::vec3(pozoPosX, alturaPozo + 0.02f, pozoPosZ));

		matrizPozo = glm::scale(matrizPozo,
			glm::vec3(0.75f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizPozo));

		pozoBase.Draw(lightingShader);


		// Subida y bajada de la cubeta
		float movimientoCubeta =
			0.10f * sin((float)glfwGetTime() * 2.3f);

		glm::mat4 matrizCubeta = glm::translate(
			matrizPozo,
			glm::vec3(0.0f, -0.08f + movimientoCubeta, 0.0f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizCubeta));

		pozoCubeta.Draw(lightingShader);


		// Campana con movimiento de balanceo


		float campanaPosX = 5.0f;
		float campanaPosZ = -4.5f;

		float alturaCampana =
			ObtenerAlturaTerreno(campanaPosX, campanaPosZ);

		float anguloCampana =
			18.0f * sin((float)glfwGetTime() * 2.5f);

		glm::mat4 matrizCampana = glm::mat4(1.0f);

		matrizCampana = glm::translate(
			matrizCampana,
			glm::vec3(campanaPosX, alturaCampana + 0.75f,
				campanaPosZ));

		matrizCampana = glm::rotate(
			matrizCampana,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizCampana = glm::rotate(
			matrizCampana,
			glm::radians(anguloCampana),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizCampana = glm::scale(
			matrizCampana, glm::vec3(0.5f));

		matrizCampana = glm::translate(
			matrizCampana,
			glm::vec3(0.0f, 0.0f, -0.73f));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizCampana));


		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizCampana));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(
			glGetUniformLocation(campanaShader.Program, "colorBronce"),
			0.78f, 0.49f, 0.18f);

		campana.Draw(campanaShader);

		lightingShader.Use();


		// Soporte fijo de la campana

		glm::mat4 matrizSoporte = glm::mat4(1.0f);

		matrizSoporte = glm::translate(
			matrizSoporte,
			glm::vec3(campanaPosX, alturaCampana,
				campanaPosZ));

		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizSoporte));

		soporteCampana.Draw(lightingShader);


		// Sacos de arena

		float sacosPosX = -2.5f;
		float sacosPosZ = -6.0f;

		float alturaSacos =
			ObtenerAlturaTerreno(sacosPosX, sacosPosZ);

		glm::mat4 matrizSacos = glm::mat4(1.0f);

		matrizSacos = glm::translate(
			matrizSacos,
			glm::vec3(sacosPosX, alturaSacos + 0.02f, sacosPosZ));

		matrizSacos = glm::rotate(
			matrizSacos,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizSacos = glm::scale(
			matrizSacos, glm::vec3(0.5f));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizSacos));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(
			glGetUniformLocation(campanaShader.Program, "colorBronce"),
			0.58f, 0.49f, 0.33f);

		sacos.Draw(campanaShader);


		lightingShader.Use();


		// Farol antiguo

		float farolPosX = 0.0f;
		float farolPosZ = -5.3f;

		float alturaFarol =
			ObtenerAlturaTerreno(farolPosX, farolPosZ);

		glm::mat4 matrizFarol = glm::mat4(1.0f);

		matrizFarol = glm::translate(
			matrizFarol,
			glm::vec3(farolPosX, alturaFarol + 0.02f, farolPosZ));


		matrizFarol = glm::scale(
			matrizFarol, glm::vec3(0.25f));


		glUniform1i(glGetUniformLocation(
			lightingShader.Program, "useBones"), 0);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(matrizFarol));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizFarol));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(
			glGetUniformLocation(campanaShader.Program, "colorBronce"),
			0.60f, 0.40f, 0.20f);


		// Dibujar por separado el poste y la lampara

		if (farol.GetMeshCount() >= 2)
		{
			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.60f, 0.40f, 0.20f);

			glUniformMatrix4fv(
				glGetUniformLocation(campanaShader.Program, "model"),
				1, GL_FALSE, glm::value_ptr(matrizFarol));

			farol.DrawSingleMesh(campanaShader, 0);

			glm::mat4 matrizLampara = glm::translate(
				matrizFarol,
				glm::vec3(0.0f, 3.0f, 1.0f));

			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.85f, 0.65f, 0.25f);

			glUniformMatrix4fv(
				glGetUniformLocation(campanaShader.Program, "model"),
				1, GL_FALSE, glm::value_ptr(matrizLampara));

			farol.DrawSingleMesh(campanaShader, 1);
		}
		else
		{
			farol.Draw(campanaShader);
		}


		lightingShader.Use();


		// Canon antiguo


		float canonPosX = 1.8f;
		float canonPosZ = -7.0f;

		float alturaCanon =
			ObtenerAlturaTerreno(canonPosX, canonPosZ);

		glm::mat4 matrizCanon = glm::mat4(1.0f);

		matrizCanon = glm::translate(
			matrizCanon,
			glm::vec3(canonPosX, alturaCanon + 0.02f, canonPosZ));

		matrizCanon = glm::rotate(
			matrizCanon,
			glm::radians(30.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		matrizCanon = glm::scale(
			matrizCanon, glm::vec3(0.85f));


		// Dibujar canon con su textura

		canonShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(canonShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizCanon));

		glUniformMatrix4fv(
			glGetUniformLocation(canonShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(canonShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, texturaCanon);

		glUniform1i(
			glGetUniformLocation(canonShader.Program, "canonAtlas"),
			7);

		canon.Draw(canonShader);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);

		lightingShader.Use();


		// Casa rustica

		float casaPosX = 5.8f;
		float casaPosZ = -7.5f;

		float alturaCasa =
			ObtenerAlturaTerreno(casaPosX, casaPosZ);

		glm::mat4 matrizCasa = glm::mat4(1.0f);

		matrizCasa = glm::translate(
			matrizCasa,
			glm::vec3(casaPosX, alturaCasa + 0.02f, casaPosZ));

		matrizCasa = glm::rotate(
			matrizCasa,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizCasa = glm::scale(
			matrizCasa, glm::vec3(1.5f));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizCasa));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(
			glGetUniformLocation(campanaShader.Program, "colorBronce"),
			0.58f, 0.40f, 0.23f);

		casa.Draw(campanaShader);

		lightingShader.Use();


		// Locomotora de vapor

		float trenPosX = -8.0f;
		float trenPosZ = -8.0f;

		float alturaTren =
			ObtenerAlturaTerreno(trenPosX, trenPosZ);

		glm::mat4 matrizTren = glm::mat4(1.0f);

		matrizTren = glm::translate(
			matrizTren,
			glm::vec3(trenPosX, alturaTren + 0.05f, trenPosZ));

		matrizTren = glm::rotate(
			matrizTren,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizTren = glm::scale(
			matrizTren, glm::vec3(0.40f));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		// Colocar y colorear las distintas piezas de la locomotora
		auto dibujarParteTren = [&](unsigned int indice,
			const glm::mat4& matrizParte,
			const glm::vec3& color)
			{
				glUniformMatrix4fv(
					glGetUniformLocation(campanaShader.Program, "model"),
					1, GL_FALSE, glm::value_ptr(matrizParte));

				glUniform3f(
					glGetUniformLocation(campanaShader.Program, "colorBronce"),
					color.r, color.g, color.b);

				tren.DrawSingleMesh(campanaShader, indice);
			};

		glm::vec3 grisMetal(0.48f, 0.48f, 0.52f);
		glm::vec3 negroMetal(0.12f, 0.12f, 0.14f);

		// El modelo se carga en diez mallas
		if (tren.GetMeshCount() == 10)
		{
			// Ruedas grandes
			float posicionesRuedas[3] = {
				-0.31955f, 0.94138f, 2.20231f
			};

			for (int i = 0; i < 3; i++)
			{
				glm::mat4 matrizRueda = glm::translate(
					matrizTren,
					glm::vec3(posicionesRuedas[i], 0.0f, 0.50008f));

				dibujarParteTren(i * 2, matrizRueda, grisMetal);
				dibujarParteTren(i * 2 + 1, matrizRueda, negroMetal);
			}

			// Cuerpo del tren
			dibujarParteTren(6, matrizTren, grisMetal);
			dibujarParteTren(7, matrizTren, negroMetal);

			// Ruedas pequenas
			glm::mat4 ruedaPequena1 = glm::translate(
				matrizTren,
				glm::vec3(-2.63900f, 0.0f, 0.35408f));

			ruedaPequena1 = glm::rotate(
				ruedaPequena1,
				glm::radians(90.0f),
				glm::vec3(1.0f, 0.0f, 0.0f));

			ruedaPequena1 = glm::scale(
				ruedaPequena1,
				glm::vec3(1.0f, 1.0f, 0.20987f));

			dibujarParteTren(8, ruedaPequena1, negroMetal);

			// Segundo conjunto de ruedas pequenas
			glm::mat4 ruedaPequena2 = glm::translate(
				matrizTren,
				glm::vec3(-1.35919f, 0.0f, 0.35408f));

			dibujarParteTren(9, ruedaPequena2, negroMetal);
		}
		else
		{
			glUniformMatrix4fv(
				glGetUniformLocation(campanaShader.Program, "model"),
				1, GL_FALSE, glm::value_ptr(matrizTren));

			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.48f, 0.48f, 0.52f);

			tren.Draw(campanaShader);
		}

		lightingShader.Use();


		// Nopal con flores

		float nopalPosX = -5.5f;
		float nopalPosZ = -1.0f;

		float alturaNopal =
			ObtenerAlturaTerreno(nopalPosX, nopalPosZ);

		glm::mat4 matrizNopal = glm::mat4(1.0f);

		matrizNopal = glm::translate(
			matrizNopal,
			glm::vec3(nopalPosX, alturaNopal + 0.02f, nopalPosZ));

		matrizNopal = glm::rotate(
			matrizNopal,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizNopal = glm::scale(
			matrizNopal, glm::vec3(1.2f));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizNopal));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));


		// Dibujar flores y pencas con colores diferentes
		if (nopal.GetMeshCount() >= 2)
		{
			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.90f, 0.30f, 0.55f);

			nopal.DrawSingleMesh(campanaShader, 0);

			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.15f, 0.45f, 0.12f);

			nopal.DrawSingleMesh(campanaShader, 1);
		}
		else
		{
			glUniform3f(
				glGetUniformLocation(campanaShader.Program, "colorBronce"),
				0.15f, 0.45f, 0.12f);

			nopal.Draw(campanaShader);
		}


		lightingShader.Use();

		// Cerca de madera


		float cercaPosX = 3.5f;
		float cercaPosZ = -1.8f;

		float alturaCerca =
			ObtenerAlturaTerreno(cercaPosX, cercaPosZ);

		glm::mat4 matrizCerca = glm::mat4(1.0f);

		matrizCerca = glm::translate(
			matrizCerca,
			glm::vec3(cercaPosX, alturaCerca + 0.02f, cercaPosZ));

		matrizCerca = glm::rotate(
			matrizCerca,
			glm::radians(-90.0f),
			glm::vec3(1.0f, 0.0f, 0.0f));

		matrizCerca = glm::scale(
			matrizCerca, glm::vec3(1.2f));

		campanaShader.Use();

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "model"),
			1, GL_FALSE, glm::value_ptr(matrizCerca));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "view"),
			1, GL_FALSE, glm::value_ptr(view));

		glUniformMatrix4fv(
			glGetUniformLocation(campanaShader.Program, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(
			glGetUniformLocation(campanaShader.Program, "colorBronce"),
			0.55f, 0.35f, 0.18f);

		cerca.Draw(campanaShader);

		lightingShader.Use();


		// Nueve arboles acomodados a la altura del terreno

		glm::vec2 posicionesArboles[9] = {
			glm::vec2(0.0f, 8.0f),      // Arbol 1
			glm::vec2(8.0f, 0.0f),      // Arbol 2
			glm::vec2(-8.0f, 0.0f),     // Arbol 3
			glm::vec2(0.0f, -10.0f),    // Arbol 4
			glm::vec2(3.0f, -15.0f),    // Arbol 5

			glm::vec2(-12.0f, -8.0f),   // Arbol 6
			glm::vec2(12.0f, -8.0f),    // Arbol 7
			glm::vec2(-12.0f, 8.0f),    // Arbol 8
			glm::vec2(12.0f, 8.0f)      // Arbol 9
		};

		for (int i = 0; i < 9; i++)
		{
			float arbolX = posicionesArboles[i].x;
			float arbolZ = posicionesArboles[i].y;

			float alturaArbol =
				ObtenerAlturaTerreno(arbolX, arbolZ);

			model = glm::mat4(1.0f);

			model = glm::translate(
				model,
				glm::vec3(arbolX, alturaArbol, arbolZ));

			model = glm::scale(
				model, glm::vec3(2.0f));

			glUniform1i(
				glGetUniformLocation(
					lightingShader.Program, "useBones"), 0);

			glUniformMatrix4fv(
				modelLoc, 1, GL_FALSE,
				glm::value_ptr(model));

			tree.Draw(lightingShader);
		}


		// Zorro: caminata y salto

		if (zorroCaminando)
		{
			tiempoZorro += deltaTime;
			zorroPosZ += velocidadZorro * deltaTime;
			if (zorroPosZ >= 45.0f)
			{
				zorroPosZ = 45.0f;
				zorroCaminando = false;
			}
		}
		float pasoZorro = 0.0f;
		if (zorroCaminando)
		{
			pasoZorro = sin(tiempoZorro * 5.0f) * 25.0f;
		}
		// Movimiento de salto
		float elevacionSalto = 0.0f;

		if (zorroSaltando)
		{
			tiempoSaltoZorro += deltaTime;

			float progreso = tiempoSaltoZorro / duracionSaltoZorro;

			if (progreso >= 1.0f)
			{
				progreso = 1.0f;
				zorroSaltando = false;
				tiempoSaltoZorro = 0.0f;
			}
			elevacionSalto = sin(progreso * 3.14159265f)
				* alturaMaximaSalto;
		}


		model = glm::mat4(1.0f);
		float alturaSuelo = ObtenerAlturaTerreno(-2.0f, zorroPosZ);
		float alturaZorro = alturaSuelo + 0.243f + elevacionSalto;
		model = glm::translate(model,
			glm::vec3(-2.0f, alturaZorro, zorroPosZ));

		model = glm::scale(model, glm::vec3(0.2f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		zorroCuerpo.Draw(lightingShader);
		zorroCabeza.Draw(lightingShader);
		zorroCola.Draw(lightingShader);
		auto dibujarPata = [&](Model& pata, glm::vec3 pivote, float angulo)
			{
				glm::mat4 modeloPata = model;

				modeloPata = glm::translate(modeloPata, pivote);

				modeloPata = glm::rotate(modeloPata,
					glm::radians(angulo),
					glm::vec3(1.0f, 0.0f, 0.0f));

				modeloPata = glm::translate(modeloPata, -pivote);

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
					glm::value_ptr(modeloPata));

				pata.Draw(lightingShader);
			};
		dibujarPata(pataDerDelante,
			glm::vec3(-0.254f, -0.493f, -0.918f), pasoZorro);
		dibujarPata(pataIzqDelante,
			glm::vec3(0.267f, -0.493f, -0.918f), -pasoZorro);
		dibujarPata(pataDerDetras,
			glm::vec3(-0.275f, -0.493f, -1.518f), -pasoZorro);
		dibujarPata(pataIzqDetras,
			glm::vec3(0.267f, -0.493f, -1.518f), pasoZorro);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
			glm::value_ptr(model));


		// Steve: caminata por estados (modelo de referencia)
		model = modelTemp;
		if (playFBX)
		{
			// Recorrido de ida y vuelta
			if (steveEstado == 0) // Estado 0: Caminando hacia adelante
			{
				g_SteveAnimator->UpdateAnimation(deltaTime);
				stevePosZ += steveDir * steveSpeed * deltaTime;

				if (stevePosZ >= 2.8f)
				{
					stevePosZ = 2.8f;
					steveDir = -1.0f;
					steveEstado = 1;
				}
			}
			else if (steveEstado == 1) // Estado 1: Regresando al origen
			{
				g_SteveAnimator->UpdateAnimation(deltaTime);
				stevePosZ += steveDir * steveSpeed * deltaTime;

				if (stevePosZ <= 0.0f)
				{
					stevePosZ = 0.0f;
					steveEstado = 2;
					playFBX = false;
				}
			}

			glUniform1i(glGetUniformLocation(lightingShader.Program, "useBones"), 1);
			model = glm::translate(model, glm::vec3(2.0f + stevePosX, 0.1f + stevePosY, 0.0f + stevePosZ));
			if (steveEstado == 1)
				model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.05f));
			const auto& transforms = g_SteveAnimator->GetFinalBoneMatrices();
			for (int i = 0; i < (int)transforms.size(); i++)
			{
				string uniformName = "finalBonesMatrices[" + to_string(i) + "]";
				glUniformMatrix4fv(
					glGetUniformLocation(lightingShader.Program, uniformName.c_str()),
					1, GL_FALSE, glm::value_ptr(transforms[i])
				);
			}
		}
		else
		{
			glUniform1i(glGetUniformLocation(lightingShader.Program, "useBones"), 0);
			model = glm::translate(model, glm::vec3(2.0f + stevePosX, 0.1f + stevePosY, 0.0f + stevePosZ));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.05f));
		}
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Steve.Draw(lightingShader);

		glUniform1i(glGetUniformLocation(lightingShader.Program, "useBones"), 0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(0);

		// === Dibujo del skybox ===
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	delete g_SteveAnim;
	delete g_SteveAnimator;
	glfwTerminate();
	return 0;
}


void DoMovement()
{
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT, deltaTime);

}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		luzNaranja = !luzNaranja;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		zorroCaminando = !zorroCaminando;
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		if (!zorroSaltando)
		{
			zorroSaltando = true;
			tiempoSaltoZorro = 0.0f;
		}
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		stevePosZ = 0.0f;
		steveEstado = 0;
		steveDir = 1.0f;

		g_SteveAnimator->PlayAnimation(g_SteveAnim);
		playFBX = true;

		printf("Animaci�n de Steve iniciada\n");

	}
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

// Function to handle keyframe animation logic
void AnimationKeys() {

}

// Is called whenever the mouse moves
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}


void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].gatoPosX - KeyFrame[playIndex].gatoPosX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].gatoPosY - KeyFrame[playIndex].gatoPosY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].gatoPosZ - KeyFrame[playIndex].gatoPosZ) / i_max_steps;
	KeyFrame[playIndex].headIncGato = (KeyFrame[playIndex + 1].headGato - KeyFrame[playIndex].headGato) / i_max_steps;
	KeyFrame[playIndex].FLegsIncGato = (KeyFrame[playIndex + 1].FLegsGato - KeyFrame[playIndex].FLegsGato) / i_max_steps;
	KeyFrame[playIndex].BLegsIncGato = (KeyFrame[playIndex + 1].BLegsGato - KeyFrame[playIndex].BLegsGato) / i_max_steps;
	KeyFrame[playIndex].tailIncGato = (KeyFrame[playIndex + 1].tailGato - KeyFrame[playIndex].tailGato) / i_max_steps;

	KeyFrame[playIndex].rotGatoInc = (KeyFrame[playIndex + 1].rotGato - KeyFrame[playIndex].rotGato) / i_max_steps;
	KeyFrame[playIndex].inclinacionIncGato = (KeyFrame[playIndex + 1].inclinacionGato - KeyFrame[playIndex].inclinacionGato) / i_max_steps;
}

float ObtenerAlturaTerreno(float x, float z)
{
	static Assimp::Importer importer;

	static const aiScene* terreno = importer.ReadFile(
		"ModelsExtra/Floor.obj",
		aiProcess_Triangulate
	);

	if (!terreno || !terreno->HasMeshes())
		return 0.0f;

	for (unsigned int i = 0; i < terreno->mNumMeshes; i++)
	{
		const aiMesh* malla = terreno->mMeshes[i];

		for (unsigned int j = 0; j < malla->mNumFaces; j++)
		{
			const aiFace& cara = malla->mFaces[j];

			if (cara.mNumIndices != 3)
				continue;

			const aiVector3D& a = malla->mVertices[cara.mIndices[0]];
			const aiVector3D& b = malla->mVertices[cara.mIndices[1]];
			const aiVector3D& c = malla->mVertices[cara.mIndices[2]];

			float d = (b.z - c.z) * (a.x - c.x)
				+ (c.x - b.x) * (a.z - c.z);

			if (fabsf(d) < 0.000001f)
				continue;

			float u = ((b.z - c.z) * (x - c.x)
				+ (c.x - b.x) * (z - c.z)) / d;

			float v = ((c.z - a.z) * (x - c.x)
				+ (a.x - c.x) * (z - c.z)) / d;

			float w = 1.0f - u - v;

			if (u >= -0.0001f &&
				v >= -0.0001f &&
				w >= -0.0001f)
			{
				return u * a.y + v * b.y + w * c.y;
			}
		}
	}

	return 0.0f;
}