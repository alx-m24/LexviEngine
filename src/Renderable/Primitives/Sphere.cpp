#include "pch.h"

#include "Renderable/Primitives/Sphere.hpp"

namespace Lexvi {
	void generateUnitSphere(SphereMesh& sphere, int stacks, int slices) {
		sphere.vertices.clear();
		sphere.indices.clear();

		// --- VERTICES --- //
		for (int i = 0; i <= stacks; ++i) {
			float V = (float)i / (float)stacks;
			float theta = V * static_cast<float>(std::numbers::pi); // latitude

			for (int j = 0; j <= slices; ++j) {
				float U = (float)j / (float)slices;
				float phi = U * static_cast<float>(2.0 * std::numbers::pi); // longitude

				SphereVertex vertex;
				vertex.position.x = cosf(phi) * sinf(theta);
				vertex.position.y = cosf(theta);
				vertex.position.z = sinf(phi) * sinf(theta);

				vertex.normal = glm::normalize(vertex.position); // normal is just the unit position

				sphere.vertices.push_back(vertex);
			}
		}
	}

	Sphere::Sphere(int stacks, int slices) {
		generateUnitSphere(sphereMesh, stacks, slices);
	}

	void Sphere::Draw(const Shader* shader)
	{
		shader->use();
		glBindVertexArray(sphereMesh.VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereMesh.indices.size()), GL_UNSIGNED_INT, nullptr);
	}
}