/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture dadoTexture;

Model AudiR8;
Model RinR8;
Model LlantaR8;
Model CofreR8;
Model GalaxyS22;
Model Lampara;
Model Blackhawk_M;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CrearDado()
{
	unsigned int dado_indices[] = {
		0, 1, 2,  // Triángulo 1
		3, 4, 5,  // Triángulo 2
		6, 7, 8,  // Triángulo 3
		9, 10, 11, // Triángulo 4
		12, 13, 14, // Triángulo 5
		15, 16, 17, // Triángulo 6
		18, 19, 20, // Triángulo 7
		21, 22, 23, // Triángulo 8
		24, 25, 26, // Triángulo 9
		27, 28, 29, // Triángulo 10
	};


	GLfloat dado_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		0.0f,  1.0f,  0.0f,	    0.45f,  0.3f,		-1.0f,	-1.0f,	-1.0f, //0
		1.0f,  0.0f,  -0.24f,	0.62f,	0.19f,		-1.0f,	-1.0f,	-1.0f, //1 //cara 9
		0.65f, 0.0f,  0.97f,	0.44f,	0.13f,		-1.0f,	-1.0f,	-1.0f, //2

		0.0f,  1.0f,  0.0f,	    0.415f, 0.295f,		0.0f,	-1.0f,	-1.0f, //0
		0.65f, 0.0f,  0.97f,	0.415f,	0.13f,		0.0f,	-1.0f,	-1.0f, //2 // cara 3
	   -0.65f, 0.0f,  0.97f,	0.25f,	0.2f,		0.0f,	-1.0f,	-1.0f, //3

		0.0f,  1.0f,  0.0f,	    0.39f,  0.31f,		1.0f,	-1.0f,	-1.0f, //0
	   -0.65f, 0.0f,  0.97f,	0.201f,	0.22f,		1.0f,	-1.0f,	-1.0f, //3 //cara 1
	   -1.0f,  0.0f, -0.24f,	0.17f,	0.33f,		1.0f,	-1.0f,	-1.0f, //4

		0.0f,  1.0f,  0.0f,	    0.41f,  0.34f,		1.0f,	-1.0f,	1.0f, //0
	   -1.0f,  0.0f, -0.24f,	0.2f,	0.4f,		1.0f,	-1.0f,	1.0f, //4 //cara 7
		0.0f,  0.0f,  -1.0f,	0.3f,	0.48f,		1.0f,	-1.0f,	1.0f, //5

		0.0f,  1.0f,  0.0f,	    0.43f,  0.36f,		-1.0f,	-1.0f,	1.0f, //0
		0.0f,  0.0f,  -1.0f,	0.4f,	0.51f,		-1.0f,	-1.0f,	1.0f, //5 //cara 5
		1.0f,  0.0f,  -0.24f,	0.52f,	0.5f,		-1.0f,	-1.0f,	1.0f, //1

		0.0f, -1.0f,  0.0f,	    0.61f,  0.69f,		-1.0f,	1.0f,	-1.0f, //6
		1.0f,  0.0f,  -0.24f,	0.48f,	0.53f,		-1.0f,	1.0f,	-1.0f, //1 //cara 10
		0.65f, 0.0f,  0.97f,	0.68f,	0.52f,		-1.0f,	1.0f,	-1.0f, //2

		0.0f, -1.0f,  0.0f,	    0.59f,  0.69f,		0.0f,	1.0f,	-1.0f, //6
		0.65f, 0.0f,  0.97f,	0.45f,	0.56f,		0.0f,	1.0f,	-1.0f, //2 //cara 8
	   -0.65f, 0.0f,  0.97f,	0.36f,	0.67f,		0.0f,	1.0f,	-1.0f, // 3

	   0.0f, -1.0f,  0.0f,	    0.58f,  0.72f,		1.0f,	1.0f,	-1.0f, //6
	   -0.65f, 0.0f,  0.97f,	0.44f,	0.84f,		1.0f,	1.0f,	-1.0f, //3 //cara 2
	   -1.0f,  0.0f, -0.24f,	0.36f,	0.71f,		1.0f,	1.0f,	-1.0f, //4

	   0.0f, -1.0f,  0.0f,	    0.6f,  0.723,		1.0f,	1.0f,	1.0f, //6
	   -1.0f,  0.0f, -0.24f,	0.63f,	0.89f,		1.0f,	1.0f,	1.0f, //4 //cara 6
		0.0f,  0.0f,  -1.0f,	0.47f,	0.86f,		1.0f,	1.0f,	1.0f, //5

		0.0f, -1.0f,  0.0f,	    0.62f,  0.72f,		-1.0f,	1.0f,	1.0f, //6 //cara 4
		1.0f,  0.0f,  -0.24f,	0.69f,	0.89f,		-1.0f,	1.0f,	1.0f, //1
		0.0f,  0.0f,  -1.0f,	0.83f,	0.8f,		-1.0f,	1.0f,	1.0f, //5
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(dado_vertices, dado_indices, sizeof(dado_vertices) / sizeof(dado_vertices[0]), sizeof(dado_indices) / sizeof(dado_indices[0]));

	meshList.push_back(dado);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void manageLights(PointLight* real_pointLights,unsigned int* num_point, SpotLight* real_spotLights, unsigned int* num_spot) {
	

	//funcion a mejorar cuando tenga tiempo
	//por el momento solo es un prototipo por falta de tiempo
	//point
	int i = 0;
	if (mainWindow.get_turn_on_faro()) {
		real_pointLights[i] = pointLights[0];
		i++;
	}

	*num_point = i;

	//spot
	int j = 0;

	real_spotLights[j] = spotLights[0]; //lampara
	j++;

	if (mainWindow.get_turn_on_galaxy_flash()) {
		real_spotLights[j] = spotLights[1];
		j++;
	}

	if (mainWindow.get_turn_on_r8_delantera()) {
		real_spotLights[j] = spotLights[2];
		j++;
	}

	if (mainWindow.get_turn_on_r8_trasera()) {
		real_spotLights[j] = spotLights[3];
		j++;
	}

	real_spotLights[j] = spotLights[4]; //luz cofre
	j++;

	*num_spot = j;
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado10caras.png");
	dadoTexture.LoadTextureA();

	AudiR8 = Model();
	AudiR8.LoadModel("Models/carro-sin-llantas-cofre.obj");
	RinR8 = Model();
	RinR8.LoadModel("Models/rintexturizado.obj");
	LlantaR8 = Model();
	LlantaR8.LoadModel("Models/llantatexturizada.obj");
	CofreR8 = Model();
	CofreR8.LoadModel("Models/cofree-texturizado.obj");

	GalaxyS22 = Model();
	GalaxyS22.LoadModel("Models/S22 Ultra.obj");

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");

	Lampara = Model();
	Lampara.LoadModel("Models/LamparaSinPiso.obj");
	

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);


	//Declaración de primer luz puntual

	//Luz del faro
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.8f, //aqui poner el primero .2 o .3 en el primero y en el segundo es la distancia donde se intensificara del foco
		28.0f, 6.5f, -20.0,
		1.1f, 0.02f, 0.005f);
	pointLightCount++;

	
	//linterna personal
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//flash galaxy s22
	spotLights[1] = SpotLight(1.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.001f, 0.002f,
		20.0f);
	spotLightCount++;

	//Luz delantera del faro R8
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		6.0f, 0.0f, 4.0f,
		0.0f, 0.0f, 0.1f,
		1.0f, 0.0f, 0.005f,
		15.0f);
	spotLightCount++;

	//Luz trasera del faro R8
	spotLights[3] = SpotLight(1.0f, 0.0f, 0.0f,
		1.0f, 2.0f,
		6.0f, 0.0f, 4.0f,
		0.0f, 0.0f, -0.1f,
		1.0f, 0.0f, 0.005f,
		15.0f);
	spotLightCount++;

	//Luz cofre R8
	spotLights[4] = SpotLight(1.0f, 1.0f, 0.0f,
		1.0f, 2.0f,
		6.0f, 0.0f, 4.0f,
		0.0f, 0.0f, 0.1f,
		1.0f, 0.0f, 0.002f,
		15.0f);
	spotLightCount++;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);

		PointLight real_pointLights[MAX_POINT_LIGHTS];
		SpotLight real_spotLights[MAX_SPOT_LIGHTS];

		unsigned int real_spotLightCount = 0;
		unsigned int real_pointLightCount = 0;

		manageLights(real_pointLights, &real_pointLightCount, real_spotLights, &real_spotLightCount);

		shaderList[0].SetPointLights(real_pointLights, real_pointLightCount);
		shaderList[0].SetSpotLights(real_spotLights, real_spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//++++++++++++++++++++++++++++++++++++++++++AUDI R8+++++++++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(6.0f, -1.0f, -4.0f + mainWindow.getmuevex()));
		glm::vec3 position = glm::vec3(model[3][0] + 1.45f, model[3][1] + 1.68f, model[3][2] + 5.9f);
		spotLights[2].SetPos(position);

		position = glm::vec3(model[3][0] + 1.45f, model[3][1] + 1.7f, model[3][2] - 5.9f);
		spotLights[3].SetPos(position);

		//Carcasa Audi
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AudiR8.RenderModel();

		//LLanta izquierda delantera
		modelaux = model;

		model = glm::translate(model, glm::vec3(2.34f, 1.05f, 3.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();


		model = modelaux;

		//LLanta derecha delantera
		modelaux = model;

		model = glm::translate(model, glm::vec3(-2.38f, 1.05f, 3.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;

		//LLanta derecha trasera
		modelaux = model;

		model = glm::translate(model, glm::vec3(-2.38f, 1.05f, -4.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;

		//LLanta izquierda trasera
		modelaux = model;

		model = glm::translate(model, glm::vec3(2.34f, 1.05f, -4.16f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;

		//Cofre R8
		modelaux = model;

		model = glm::translate(model, glm::vec3(0.01f, 2.2f, 2.65f));
		model = glm::rotate(model, -glm::radians(mainWindow.getarticulacionCofre()), glm::vec3(1.0f, 0.0f, 0.0f));

		position = glm::vec3(model[3][0], model[3][1] + 1.68f, model[3][2] + 5.9f);
		spotLights[4].SetPos(position);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		CofreR8.RenderModel();

		model = modelaux;

		//++++++++++++++++++++++++++++++++++++++++++++GALAXY S22 ULTRA++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, 0.0f, -5.0f));
		position = glm::vec3(model[3][0] + 0.27f, model[3][1] + 4.3f, model[3][2]);
		spotLights[1].SetPos(position);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		GalaxyS22.RenderModel();

		//++++++++++++++++++++++++++++++++++++++++++++DADO++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();

		//+++++++++++++++++++++++++++++++++++++++++++LAMPARA++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.0f, -0.9f, -20.0));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.RenderModel();

		//++++++++++++++++++++++++++++++++++++++++++++++++++++AGAVE++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
