#include "pch.h"

#include "Renderer/Renderer.hpp"

using namespace Lexvi;

void Lexvi::Renderer::setDefaultShader(Shader* shader)
{
	defaultShader = shader;
}

void Lexvi::Renderer::Draw(IRenderable& obj, const Camera& camera, const Shader* shader) const
{
	if (!obj.isVisible(camera)) return;

	const Shader* currentShader = setCurrentShader(shader);

	if (!currentShader) {
		throw std::runtime_error("No Shader availabe to draw.");
		return;
	}

	SmartBind shaderBind(*currentShader);

	obj.Draw(currentShader);
}

void Lexvi::Renderer::Draw(std::vector<Renderable_Shader>& objects, const Camera& camera) const
{
	for (auto& obj_shader : objects) {
		IRenderable& obj = *obj_shader.first;
		Draw(obj, camera, obj_shader.second.get());
	}
}

void Lexvi::Renderer::Draw(std::vector<IRenderable>& objects, const Camera& camera, const Shader* shader) const
{
	const Shader* currentShader = setCurrentShader(shader);

	if (!currentShader) {
		throw std::runtime_error("No Shader availabe to draw.");
		return;
	}

	SmartBind shaderBind(*currentShader);

	for (auto& obj : objects) {
		if (!obj.isVisible(camera)) continue;
		currentShader->setMat4("model", obj.getTransforms());
		obj.Draw(currentShader);
	}
}

void Lexvi::Renderer::DisableBackFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void Lexvi::Renderer::EnableBackFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void Lexvi::Renderer::SetBackFace(bool CW)
{
	glFrontFace(CW ? GL_CCW : GL_CW);
}

void Lexvi::Renderer::SetFaceToCull(bool back)
{
	glCullFace(back ? GL_BACK : GL_FRONT);
}

void Lexvi::Renderer::EnableDepthTest() {
	glEnable(GL_DEPTH_TEST);
}

void Lexvi::Renderer::DisableDepthTest() {
	glDisable(GL_DEPTH_TEST);
}

void Lexvi::Renderer::ViewportSize(uint32_t offsetX, uint32_t offsetY, uint32_t width, uint32_t height)
{
	glViewport(offsetX, offsetY, width, height);
}

const Lexvi::Shader* Lexvi::Renderer::setCurrentShader(const Shader* shader) const
{
	return (shader) ? shader : defaultShader;
}

void Lexvi::Renderer::ClearBuffers() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Lexvi::Renderer::DisableBlend()
{
	glDisable(GL_BLEND);
}

void Lexvi::Renderer::EnableBlend()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
