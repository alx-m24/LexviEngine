#include "pch.h"

#include "Utils/snoise.hpp"

namespace Lexvi {
	// GLSL-compatible Perlin/Simplex noise function
	glm::vec3 mod289(glm::vec3 x) {
		return x - floor(x * (1.0f / 289.0f)) * 289.0f;
	}

	glm::vec2 mod289(glm::vec2 x) {
		return x - floor(x * (1.0f / 289.0f)) * 289.0f;
	}

	glm::vec3 permute(glm::vec3 x) {
		return mod289(((x * 34.0f) + 10.0f) * x);
	}

	float snoise(glm::vec2 v)
	{
		const glm::vec4 C = glm::vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
			0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
			-0.577350269189626,  // -1.0 + 2.0 * C.x
			0.024390243902439); // 1.0 / 41.0
		// First corner
		glm::vec2 i = floor(v + glm::dot(v, glm::vec2(C.y)));
		glm::vec2 x0 = v - i + glm::dot(i, glm::vec2(C.x));

		// Other corners
		glm::vec2 i1;
		//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
		//i1.y = 1.0 - i1.x;
		i1 = (x0.x > x0.y) ? glm::vec2(1.0, 0.0) : glm::vec2(0.0, 1.0);
		// x0 = x0 - 0.0 + 0.0 * C.xx ;
		// x1 = x0 - i1 + 1.0 * C.xx ;
		// x2 = x0 - 1.0 + 2.0 * C.xx ;
		glm::vec4 x12 = glm::vec4(x0.x, x0.y, x0.x, x0.y) + glm::vec4(C.x, C.x, C.z, C.z);
		x12.x -= i1.x;
		x12.y -= i1.y;

		// Permutations
		i = mod289(i); // Avoid truncation effects in permutation
		glm::vec3 p = permute(permute(i.y + glm::vec3(0.0, i1.y, 1.0))
			+ i.x + glm::vec3(0.0, i1.x, 1.0));

		glm::vec3 m = glm::max(0.5f - glm::vec3(dot(x0, x0), glm::dot(glm::vec2(x12.x, x12.y), glm::vec2(x12.x, x12.y)), glm::dot(glm::vec2(x12.z, x12.w), glm::vec2(x12.z, x12.w))), 0.0f);
		m = m * m;
		m = m * m;

		// Gradients: 41 points uniformly over a line, mapped onto a diamond.
		// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

		glm::vec3 x = 2.0f * glm::fract(p * glm::vec3(C.w)) - 1.0f;
		glm::vec3 h = abs(x) - 0.5f;
		glm::vec3 ox = floor(x + 0.5f);
		glm::vec3 a0 = x - ox;

		// Normalise gradients implicitly by scaling m
		// Approximation of: m *= inversesqrt( a0*a0 + h*h );
		m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);

		// Compute final noise value at P
		glm::vec3 g;
		g.x = a0.x * x0.x + h.x * x0.y;
		g = glm::vec3(g.x, glm::vec2(a0.y, a0.z) * glm::vec2(x12.x, x12.z) + glm::vec2(h.y, h.z) * glm::vec2(x12.y, x12.w));
		return 130.0f * glm::dot(m, g);
	}
}