#pragma once

#include "Camera/Camera.hpp"
#include "Shader/Shader.hpp"

namespace Lexvi {
	class IRenderable {
	protected:
		glm::mat4 transforms = (1.0f);
		CameraAABB cameraAABB = {};

	public:
		virtual ~IRenderable() = default;

		virtual void Draw(const Shader* shader) = 0;

		virtual void updateBoundingBox() {};

		virtual void setTransforms(const glm::mat4& mat) { transforms = mat; }
		virtual glm::mat4 getTransforms() const { return transforms; };
		virtual CameraAABB getBoundBox() const { return cameraAABB; };
		virtual bool isVisible(const Camera& camera) const {
			// default: always visible
			return true;
		}
	};
}