#include "ParellelogramRenderer.h"

void ParellelogramRenderer::draw(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color
) {
	this->draw_internal(scale, rotation, translate, color, GL_TRIANGLE_STRIP, 7);
}


