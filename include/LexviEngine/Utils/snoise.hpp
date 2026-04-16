#pragma once

namespace Lexvi {
	// GLSL-compatible Perlin/Simplex noise function
	glm::vec3 mod289(glm::vec3 x);

	glm::vec2 mod289(glm::vec2 x);

	glm::vec3 permute(glm::vec3 x);

	float snoise(glm::vec2 v);
}