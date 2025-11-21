#include "pch.h"

#include "Utils/gBuffer.hpp"
#include "Utils/Logging.hpp"
#include "Textures/Textures.hpp"

namespace Lexvi {
	void gBufferManager::Init(unsigned int width, unsigned int height)
	{
		Delete();

		glCreateFramebuffers(1, &buffer.id);

		std::array<uint32_t*, 3> textures = {
			&buffer.positionTex,
			&buffer.normalTex,
			&buffer.color_roughTex
		};
		std::array<GLenum, 3> attachments{};

		for (uint32_t i = 0; i < static_cast<uint32_t>(textures.size()); ++i) {
			uint32_t* texture = textures[i];

			glCreateTextures(GL_TEXTURE_2D, 1, texture);

			uint32_t textureID = *texture;

			GLenum internalFormat = (texture == &buffer.color_roughTex)
				? GL_RGBA8          // albedo + spec
				: GL_RGB32F;       // position / normal

			glTextureStorage2D(textureID, 1, internalFormat, width, height);
			glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
			attachments[i] = attachment;

			glNamedFramebufferTexture(buffer.id, attachment, textureID, 0);
		}

		buffer.depthTex = GenerateDepthTexture(width, height).id;
		glNamedFramebufferTexture(buffer.id, GL_DEPTH_ATTACHMENT, buffer.depthTex, 0);

		glNamedFramebufferDrawBuffers(buffer.id, static_cast<GLsizei>(attachments.size()), attachments.data());

		if (glCheckNamedFramebufferStatus(buffer.id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			LEXVI_LOG_ERROR("GBuffer incomplete!");
		}
	}

	void gBufferManager::Resize(unsigned int width, unsigned int height)
	{
		Init(width, height);
	}

	void gBufferManager::Delete()
	{
		if (buffer.id) {
			glDeleteFramebuffers(1, &buffer.id);
			buffer.id = 0;
		}
		
		std::array<uint32_t*, 3> textures = {
			&buffer.positionTex,
			&buffer.normalTex,
			&buffer.color_roughTex
		};
		for (uint32_t* texture : textures) {
			if (*texture) {
				glDeleteTextures(1, texture);
				*texture = 0;
			}
		}

		if (buffer.depthTex) {
			glDeleteTextures(1, &buffer.depthTex);
			buffer.depthTex = 0;
		}
	}

	void gBufferManager::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.id);
	}

	void gBufferManager::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void gBufferManager::BindTextures() const
	{
		glBindTextureUnit(0, buffer.positionTex);
		glBindTextureUnit(1, buffer.normalTex);
		glBindTextureUnit(2, buffer.color_roughTex);
		glBindTextureUnit(3, buffer.depthTex);
	}

	gBufferManager::SmartBind::SmartBind(const gBufferManager& gBufferMgr) {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
		gBufferMgr.Bind();
	}
	gBufferManager::SmartBind::~SmartBind() {
		glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
	}
}