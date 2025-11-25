#include "ShapeRenderer.h"

ShapeRenderer::ShapeRenderer() {}
ShapeRenderer::~ShapeRenderer() {}

void ShapeRenderer::drawTriangle(float scale, float rotation, glm::vec3 translate, GLint MatrixID) {
	glm::mat4 I(1.0f);

	glm::mat4 S = glm::scale(I, glm::vec3(scale, scale, 1.0f));

	glm::mat4 R = glm::rotate(I, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 T = glm::translate(I, translate);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(T * R * S));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE,
		reinterpret_cast<GLvoid*>(0));
}

void ShapeRenderer::drawSquare() {
	// TODO()
}

void ShapeRenderer::drawParallelogram() {
	// TODO()
}
