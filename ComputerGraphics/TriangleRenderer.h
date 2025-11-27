#pragma once

#include "ShapeRenderer.h"

class TriangleRenderer : public ShapeRenderer {
public:
	void draw(
		glm::vec2 scale,
		float rotation,
		glm::vec3 translate,
		glm::vec4 color
	) override;

    TriangleRenderer(GLint MatrixID, GLint ColorID) : ShapeRenderer(MatrixID, ColorID) {}

	~TriangleRenderer() {};
};