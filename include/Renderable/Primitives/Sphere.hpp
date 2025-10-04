#pragma once

#include <Renderable/IRenderable/IRenderable.hpp>

namespace Lexvi {
	struct SphereVertex {
		glm::vec3 position;
		glm::vec3 normal;
	};

	struct SphereMesh {
		std::vector<SphereVertex> vertices;
		std::vector<uint32_t> indices;
		unsigned int VAO = 0, VBO = 0, EBO = 0;
	};

	// sets up the buffers for a unit sphere with position and normal for each vertex
	void generateUnitSphere(SphereMesh& sphere, int stacks, int slices);

	class Sphere : public IRenderable {
	private:
		SphereMesh sphereMesh;

	public:
		Sphere(int stacks, int slices);

		void Draw(const Shader* shader) override;
	};
}