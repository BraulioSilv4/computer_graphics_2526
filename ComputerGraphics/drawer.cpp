#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

typedef struct {
	GLfloat XYZW[4];
	GLfloat RGBA[4];
} Vertex;

class Drawer {
	const Vertex Vertices_triangle[3] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}} };

	const Vertex Vertices_square[4] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}} };

	const Vertex Vertices_parallelogram[4] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
		{{1.0f, 2.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}} };


	glm::mat4 drawTriangle(float scale, float rotation, glm::vec3 translate) {
		glm::mat4 I(1.0f);
		
		glm::mat4 S = glm::scale(I, glm::vec3(scale, scale, 1.0f));

		glm::mat4 R = glm::rotate(I, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 T = glm::translate(I, translate);

		return glm::dot(T, glm::dot(S, R));
	}

	void drawSquare();
	void drawParallelogram();
};

