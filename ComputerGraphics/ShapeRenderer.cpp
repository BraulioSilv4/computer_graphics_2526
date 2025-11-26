#include "ShapeRenderer.h"

//ShapeRenderer::ShapeRenderer(GLint MatrixID, GLint ColorID) {
//	this->MatrixID = MatrixID;
//	this->ColorID = ColorID;
//}
ShapeRenderer::~ShapeRenderer() {}

glm::mat4 ShapeRenderer::applyTransform(
	glm::vec2 scale,
	float rotation, 
	glm::vec3 translate
) {
	glm::mat4 I(1.0f);

	glm::mat4 S = glm::scale(I, glm::vec3(scale[0], scale[1], 1.0f));

	glm::mat4 R = glm::rotate(I, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 R2 = glm::rotate(I, glm::radians(15.0f), glm::vec3(0.0f,0.0f,1.0f)); //shape is slightly rotated

	glm::mat4 T = glm::translate(I, translate);

	return R2 * T * R * S;
}

void ShapeRenderer::drawTriangle(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color
) {
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(applyTransform(scale, rotation,translate)));
	glUniform4fv(ColorID, 1, glm::value_ptr(color));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE,
		reinterpret_cast<GLvoid*>(0));
	
}

void ShapeRenderer::drawSquare(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color
) {
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(applyTransform(scale ,rotation, translate)));
	glUniform4fv(ColorID, 1, glm::value_ptr(color));
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(3));
}

void ShapeRenderer::drawParallelogram(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color
) {
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(applyTransform(scale, rotation,translate)));
	glUniform4fv(ColorID, 1, glm::value_ptr(color));
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(7));
}
