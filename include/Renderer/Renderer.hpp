#pragma once

#include "Shader/Shader.hpp"
#include "Camera/Camera.hpp"
#include "Renderable/IRenderable/IRenderable.hpp"

namespace Lexvi {
	class Renderer
	{
	private:
		Shader* defaultShader = nullptr;
		using Renderable_Shader = std::pair< std::shared_ptr<IRenderable>, std::shared_ptr<Shader>>;

	public:
		Renderer() = default;

		void setDefaultShader(Shader* shader);

		void Draw(IRenderable& obj, const Camera& camera, const Shader* shader = nullptr) const;
		void Draw(std::vector<Renderable_Shader>& objects, const Camera& camera) const;
		void Draw(std::vector<IRenderable>& objects, const Camera& camera, const Shader* shader = nullptr) const;

		void DisableBackFaceCulling();
		void EnableBackFaceCulling();
		void SetBackFace(bool CW);

		void ClearBuffers();

		static void ViewportSize(uint32_t offsetX, uint32_t offsety, uint32_t width, uint32_t height);

	private:
		const Shader* setCurrentShader(const Shader* shader) const;
	};
}