/*
Práctica 6: Texturizado
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
#include "Shader_m.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture dadoTexture;
Texture logofiTexture;

Model AudiR8;
Model RinR8;
Model LlantaR8;
Model Dado_M;

Skybox skybox;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_texture.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_texture.frag";




//cálculo del promedio de las normales para sombreado de Phong
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
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);


	
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

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
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

	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
// average normals
	GLfloat dado_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		0.0f,  1.0f,  0.0f,	    0.45f,  0.3f,		-1.0f,	0.0f,	0.0f, //0
		1.0f,  0.0f,  -0.24f,	0.62f,	0.19f,		-1.0f,	0.0f,	0.0f, //1 //cara 9
		0.65f, 0.0f,  0.97f,	0.44f,	0.13f,		-1.0f,	0.0f,	0.0f, //2

		0.0f,  1.0f,  0.0f,	    0.415f, 0.295f,		-1.0f,	0.0f,	0.0f, //0
		0.65f, 0.0f,  0.97f,	0.415f,	0.13f,		-1.0f,	0.0f,	0.0f, //2 // cara 3
	   -0.65f, 0.0f,  0.97f,	0.25f,	0.2f,		-1.0f,	0.0f,	0.0f, //3

		0.0f,  1.0f,  0.0f,	    0.39f,  0.31f,		-1.0f,	0.0f,	0.0f, //0
	   -0.65f, 0.0f,  0.97f,	0.201f,	0.22f,		-1.0f,	0.0f,	0.0f, //3 //cara 1
	   -1.0f,  0.0f, -0.24f,	0.17f,	0.33f,		-1.0f,	0.0f,	0.0f, //4
	   
		0.0f,  1.0f,  0.0f,	    0.41f,  0.34f,		-1.0f,	0.0f,	0.0f, //0
	   -1.0f,  0.0f, -0.24f,	0.2f,	0.4f,		-1.0f,	0.0f,	0.0f, //4 //cara 7
		0.0f,  0.0f,  -1.0f,	0.3f,	0.48f,		-1.0f,	0.0f,	0.0f, //5

		0.0f,  1.0f,  0.0f,	    0.43f,  0.36f,		-1.0f,	0.0f,	0.0f, //0
		0.0f,  0.0f,  -1.0f,	0.4f,	0.51f,		-1.0f,	0.0f,	0.0f, //5 //cara 5
		1.0f,  0.0f,  -0.24f,	0.52f,	0.5f,		-1.0f,	0.0f,	0.0f, //1

		0.0f, -1.0f,  0.0f,	    0.61f,  0.69f,		-1.0f,	0.0f,	0.0f, //6
		1.0f,  0.0f,  -0.24f,	0.48f,	0.53f,		-1.0f,	0.0f,	0.0f, //1 //cara 10
		0.65f, 0.0f,  0.97f,	0.68f,	0.52f,		-1.0f,	0.0f,	0.0f, //2

		0.0f, -1.0f,  0.0f,	    0.59f,  0.69f,		-1.0f,	0.0f,	0.0f, //6
		0.65f, 0.0f,  0.97f,	0.45f,	0.56f,		-1.0f,	0.0f,	0.0f, //2 //cara 8
	   -0.65f, 0.0f,  0.97f,	0.36f,	0.67f,		-1.0f,	0.0f,	0.0f, // 3

	   0.0f, -1.0f,  0.0f,	    0.58f,  0.72f,		-1.0f,	0.0f,	0.0f, //6
	   -0.65f, 0.0f,  0.97f,	0.44f,	0.84f,		-1.0f,	0.0f,	0.0f, //3 //cara 2
	   -1.0f,  0.0f, -0.24f,	0.36f,	0.71f,		-1.0f,	0.0f,	0.0f, //4

	   0.0f, -1.0f,  0.0f,	    0.6f,  0.723,		-1.0f,	0.0f,	0.0f, //6
	   -1.0f,  0.0f, -0.24f,	0.63f,	0.89f,		-1.0f,	0.0f,	0.0f, //4 //cara 6
		0.0f,  0.0f,  -1.0f,	0.47f,	0.86f,		-1.0f,	0.0f,	0.0f, //5

		0.0f, -1.0f,  0.0f,	    0.62f,  0.72f,		-1.0f,	0.0f,	0.0f, //6 //cara 4
		1.0f,  0.0f,  -0.24f,	0.69f,	0.89f,		-1.0f,	0.0f,	0.0f, //1
		0.0f,  0.0f,  -1.0f,	0.83f,	0.8f,		-1.0f,	0.0f,	0.0f, //5
	};

	Mesh* dado = new Mesh();
	dado->CreateMesh(dado_vertices, dado_indices, sizeof(dado_vertices) / sizeof(dado_vertices[0]), sizeof(dado_indices) / sizeof(dado_indices[0]));

	meshList.push_back(dado);

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
	dadoTexture = Texture("Textures/dado10caras.png");
	dadoTexture.LoadTextureA();
	logofiTexture = Texture("Textures/escudo_fi_color.tga");
	logofiTexture.LoadTextureA();
	
	
	AudiR8 = Model();
	AudiR8.LoadModel("Models/carrosinllantas.obj");
	RinR8 = Model();
	RinR8.LoadModel("Models/rintexturizado.obj");
	LlantaR8 = Model();
	LlantaR8.LoadModel("Models/llantatexturizada.obj");

	
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
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
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		color = glm::vec3(1.0f, 1.0f, 1.0f);//color blanco, multiplica a la información de color de la textura

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		meshList[2]->RenderMesh();

		//++++++++++++++++++++++++++++++++++++++++++++++++++++DADO+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		meshList[4]->RenderMesh();
		

		//++++++++++++++++++++++++++++++++++++++++++++++++Audi r8++++++++++++++++++++++++++++++++++++++++++++++++++++

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, -1.5f + mainWindow.getmuevex()));

		//+++++++++++++++++++++++++++++++++++++++++++++++++Carcasa Audi+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		AudiR8.RenderModel();


		//+++++++++++++++++++++++++++++++++++++++++LLanta izquierda delantera+++++++++++++++++++++++++++++++++++++++++++++++++
		modelaux = model;

		model = glm::translate(model, glm::vec3(2.34f, 1.05f, 3.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();


		model = modelaux;


		//+++++++++++++++++++++++++++++++++++++++++LLanta derecha delantera+++++++++++++++++++++++++++++++++++++++++++++++++
		modelaux = model;

		model = glm::translate(model, glm::vec3(-2.38f, 1.05f, 3.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;

		//+++++++++++++++++++++++++++++++++++++++++LLanta derecha trasera+++++++++++++++++++++++++++++++++++++++++++++++++
		modelaux = model;

		model = glm::translate(model, glm::vec3(-2.38f, 1.05f, -4.16f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;

		//+++++++++++++++++++++++++++++++++++++++++LLanta izquierda trasera+++++++++++++++++++++++++++++++++++++++++++++++++
		modelaux = model;

		model = glm::translate(model, glm::vec3(2.34f, 1.05f, -4.16f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		RinR8.RenderModel();
		LlantaR8.RenderModel();

		model = modelaux;


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
/*
//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		logofiTexture.UseTexture(); //textura con transparencia o traslucidez
		FIGURA A RENDERIZAR de OpenGL, si es modelo importado no se declara UseTexture
		glDisable(GL_BLEND);
*/