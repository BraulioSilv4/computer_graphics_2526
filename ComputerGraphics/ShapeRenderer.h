#pragma once

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
	void drawTriangle(float scale, float rotation, glm::vec3 translate, GLint MatrixID);
	void drawSquare();
	void drawParallelogram();

	ShapeRenderer();
	~ShapeRenderer();

private:
	static constexpr Vertex TriangleVertices[3] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}} };

	static constexpr Vertex SquareVertices[4] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}} };

	static constexpr Vertex ParallelogramVertices[4] = {
		{{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
		{{1.0f, 2.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}} };

};

