#pragma once

#include "ShapeRenderer.h"

class SquareRenderer : public ShapeRenderer {
public:
	void draw(
		glm::vec2 scale,
		float rotation,
		glm::vec3 translate,
		glm::vec4 color
	) override;

	SquareRenderer(GLint MatrixID, GLint ColorID) : ShapeRenderer(MatrixID, ColorID) {}

	~SquareRenderer() {};
};
