#include "ShapeRenderer.h"

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


void ShapeRenderer::draw_internal(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color,
	GLenum mode,
	GLbyte offset
) {
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(applyTransform(scale, rotation,translate)));
	glUniform4fv(ColorID, 1, glm::value_ptr(color));
	glDrawElements(mode, mode == GL_TRIANGLE_STRIP ? 4 : 3, GL_UNSIGNED_BYTE,
		reinterpret_cast<GLvoid*>(offset));
}