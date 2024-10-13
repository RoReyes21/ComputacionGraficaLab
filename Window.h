#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getmuevex2() { return muevex2; }
	GLfloat getarticulacionCofre() { return angulocofre; };
	GLboolean get_turn_on_faro() { return turn_on_faro; }
	GLboolean get_turn_on_r8_delantera() { return turn_on_r8_trasera; }
	GLboolean get_turn_on_r8_trasera() { return turn_on_r8_delantera; }
	GLboolean get_turn_on_galaxy_flash() { return turn_on_galaxy_flash; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat muevex2;
	GLfloat angulocofre = 0.0f;
	GLboolean turn_on_faro;
	GLboolean turn_on_r8_trasera;
	GLboolean turn_on_r8_delantera;
	GLboolean turn_on_galaxy_flash;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

