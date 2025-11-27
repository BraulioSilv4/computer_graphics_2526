#include "TriangleRenderer.h"

void TriangleRenderer::draw(
	glm::vec2 scale,
	float rotation,
	glm::vec3 translate,
	glm::vec4 color
) {
	this->draw_internal(scale, rotation, translate, color, GL_TRIANGLES, 0);
}
