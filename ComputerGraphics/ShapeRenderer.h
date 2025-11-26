#pragma once

#include "Color.h"
#include <mgl.hpp>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

typedef struct {
	GLfloat XYZW[4];
	GLfloat RGBA[4];
} Vertex;

class ShapeRenderer {
public:
	void drawTriangle(
		glm::vec2 scale,
		float rotation, 
		glm::vec3 translate, 
		glm::vec4 color
	);
	
	void drawSquare(
		glm::vec2 scale,
		float rotation, 
		glm::vec3 translate,
		glm::vec4 color
	);
	
	void drawParallelogram(
		glm::vec2 scale,
		float rotation, 
		glm::vec3 translate,
		glm::vec4 color
	);

	ShapeRenderer(GLint MatrixID, GLint ColorID) {
		this->MatrixID = MatrixID;
		this->ColorID = ColorID;
	}

	~ShapeRenderer();

private:	
	GLint MatrixID;
	GLint ColorID;

	glm::mat4 applyTransform(
		glm::vec2 scale,
		float rotation, 
		glm::vec3 translate
	);
};

