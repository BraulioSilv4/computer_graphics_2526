#pragma once

#include <mgl.hpp>

struct Color {
	static constexpr glm::vec4 Red		= glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Blue		= glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 Green	= glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Yellow	= glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f };
	static constexpr glm::vec4 Magenta	= glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f };
	
	static constexpr glm::vec4 Purple	= glm::vec4{ 0.5f, 0.0f, 0.5f, 1.0f };
	static constexpr glm::vec4 Cyan		= glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 White	= glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	static constexpr glm::vec4 Orange	= glm::vec4{ 1.0f, 0.5f, 0.0f, 1.0f };
};
