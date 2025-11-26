////////////////////////////////////////////////////////////////////////////////
//
// Basic Triangle rendering
//
// This program demonstrates fundamental Modern OpenGL concepts by rendering
// two triangle instances directly in Clip Space. It serves as an introductory
// example for understanding the OpenGL graphics pipeline and basic shader
// programming.
//
// Key Concepts Demonstrated:
// - Vertex Array Objects (VAOs) and Vertex Buffer Objects (VBOs)
// - Shader program creation and management
// - Attribute and uniform handling
// - Basic transformation matrices
// - Clip space rendering without model/view/projection matrices
//
// Copyright (c) 2013-25 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include <memory>

#include "../mgl/mgl.hpp"

#include "ShapeRenderer.h"

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
public:
	MyApp() = default;
	~MyApp() override = default;

	void initCallback(GLFWwindow* win) override;
	void displayCallback(GLFWwindow* win, double elapsed) override;
	void windowCloseCallback(GLFWwindow* win) override;
	void windowSizeCallback(GLFWwindow* win, int width, int height) override;

private:
	const GLuint POSITION = 0, COLOR = 1;
	GLuint VaoId, VboId[2];
	std::unique_ptr<mgl::ShaderProgram> Shaders = nullptr;
	GLint MatrixId;
	GLint UniformColorId;

	void createShaderProgram();
	void createBufferObjects(/*Vertex* vertices, GLubyte* indices*/);
	void destroyBufferObjects();
	void drawScene();
};


//////////////////////////////////////////////////////////////////////// SHADERs

void MyApp::createShaderProgram() {
	Shaders = std::make_unique<mgl::ShaderProgram>();
	Shaders->addShader(GL_VERTEX_SHADER, "clip-vs.glsl");
	Shaders->addShader(GL_FRAGMENT_SHADER, "clip-fs.glsl");

	Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, POSITION);
	Shaders->addAttribute(mgl::COLOR_ATTRIBUTE, COLOR);
	Shaders->addUniform("Matrix");
	Shaders->addUniform("dynamicColor");

	Shaders->create();

	MatrixId = Shaders->Uniforms["Matrix"].index;
	UniformColorId = Shaders->Uniforms["dynamicColor"].index;
}

//////////////////////////////////////////////////////////////////// VAOs & VBOs


const Vertex Vertices[] = {
	{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},	// 0
	{{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},	// 1
	{{0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},	// 2
	{{1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},	// 3
	{{1.0f, 2.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}} }; // 4

const GLubyte Indices[] = { 
	0, 3, 2,		// Triangle
	0, 1, 2, 3,		// Square
	0, 3, 2, 4		// Parallelogram
};	

void MyApp::createBufferObjects(/*Vertex *vertices, GLubyte *indices*/) {
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	
	glGenBuffers(2, VboId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
		
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<GLvoid*>(0));

	glEnableVertexAttribArray(COLOR);
	glVertexAttribPointer(
		COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		reinterpret_cast<GLvoid*>(sizeof(Vertices[0].XYZW)));
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices,
		GL_STATIC_DRAW);
		
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, VboId);
}

void MyApp::destroyBufferObjects() {
	glBindVertexArray(VaoId);
	glDisableVertexAttribArray(POSITION);
	glDisableVertexAttribArray(COLOR);
	glDeleteVertexArrays(1, &VaoId);
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 I(1.0f);
const glm::mat4 M =
glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 0.0f));

void MyApp::drawScene() {
	ShapeRenderer shapeRenderer(MatrixId, UniformColorId);

	// Drawing directly in clip space

	glBindVertexArray(VaoId);
	Shaders->bind();

	shapeRenderer.drawSquare(glm::vec2(0.25f, 0.25f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 0.0f), Color::Green);
	
	shapeRenderer.drawParallelogram(glm::vec2(0.25f, 0.25f), glm::radians(0.0f), glm::vec3(0.25f, 0.f, 0.0f), Color::Yellow);

	shapeRenderer.drawTriangle(glm::vec2(0.25f, 0.25f), glm::radians(90.0f), glm::vec3(0.75f, 0.4f, 0.0f), Color::Magenta);

	shapeRenderer.drawTriangle(glm::vec2(0.5f, 0.5f), glm::radians(270.0f), glm::vec3(-0.5f, 0.25f, 0.0f), Color::Magenta);

	shapeRenderer.drawTriangle(glm::vec2(0.25f, 0.25f), glm::radians(180.0f), glm::vec3(0.0f, 0.5f, 0.0f), Color::Cyan);

	shapeRenderer.drawTriangle(glm::vec2(0.5f, 0.5f), glm::radians(315.0f), glm::vec3((-sqrt(0.5) -0.25), 0.0f, 0.0f), Color::Blue);

	shapeRenderer.drawTriangle(glm::vec2(0.25f, 0.25f), glm::radians(135.0f), glm::vec3(-0.25, 0.0f, 0.0f), Color::Orange);

	Shaders->unbind();
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
	createBufferObjects(/*(Vertex *)Vertices, (GLubyte*)Indices*/);
	createShaderProgram();
}

void MyApp::windowCloseCallback(GLFWwindow* win) { destroyBufferObjects(); }

void MyApp::windowSizeCallback(GLFWwindow* win, int winx, int winy) {
	glViewport(0, 0, winx, winy);
}

void MyApp::displayCallback(GLFWwindow* win, double elapsed) { drawScene(); }

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char* argv[]) {
	mgl::Engine& engine = mgl::Engine::getInstance();
	engine.setApp(new MyApp());
	engine.setOpenGL(4, 6);
	engine.setWindow(600, 600, "Hello Modern 2D World", 0, 1);
	engine.init();
	engine.run();
	exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////// END
