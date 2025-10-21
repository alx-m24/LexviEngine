#include "pch.h"

#include "Utils/FrameBuffer.hpp"

namespace Lexvi {
	FrameBuffer::~FrameBuffer()
	{
		DeleteFBO();
	}

	void FrameBuffer::AddAttachment(FrameBufferAttachments newAttachment) {
		attachments = attachments | newAttachment;
		CreateFrameBuffer();
	}

	void FrameBuffer::ResizeFrameBuffer(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;

		CreateFrameBuffer();
	}

    void FrameBuffer::getFrameBufferSize(unsigned int& width, unsigned int& height) const
    {
        width = this->width;
        height = this->height;
    }

    void FrameBuffer::BindFrameBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    }

    void FrameBuffer::UnBindFrameBuffer() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    const Texture* FrameBuffer::getAttachment(FrameBufferAttachments attachment, unsigned int number) const
    {
        // Assert that only one bit is set
        assert(attachment != FrameBufferAttachments::NONE &&
            (static_cast<int>(attachment) & (static_cast<int>(attachment) - 1)) == 0);

        std::string attachmentString = AttachmentToString(attachment);
        if (attachment & COLOR) attachmentString += std::to_string(number);

        auto it = attachedTextures.find(attachmentString);
        assert(it != attachedTextures.end()); // make sure it's actually attached

        return &it->second;
    }

	void FrameBuffer::CreateFrameBuffer()
	{
		DeleteFBO();

        // Create FBO
        glCreateFramebuffers(1, &fbo);

        // Track draw buffers for color attachments
        std::vector<GLenum> drawBuffers;

        // COLOR ATTACHMENT
        if (attachments & COLOR) {
            colorAttachmentNum = std::max(1u, colorAttachmentNum);
            for (uint32_t i = 0; i < colorAttachmentNum; ++i) {
                Texture colorTex;
                colorTex.type = "FBO_COLOR";
                glCreateTextures(GL_TEXTURE_2D, 1, &colorTex.id);
                glTextureStorage2D(colorTex.id, 1, GL_RGBA8, width, height);

                // Filtering
                glTextureParameteri(colorTex.id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(colorTex.id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTextureParameteri(colorTex.id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(colorTex.id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
                glNamedFramebufferTexture(fbo, attachment, colorTex.id, 0);

                attachedTextures[std::string("COLOR" + std::to_string(i))] = std::move(colorTex);
                drawBuffers.push_back(attachment);
            }
        }

        // DEPTH ATTACHMENT
        if (attachments & DEPTH) {
            Texture depthTex;

            depthTex.id = GenerateDepthTexture(width, height);

            glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depthTex.id, 0);
            attachedTextures["DEPTH"] = std::move(depthTex);
        }

        // STENCIL ATTACHMENT
        if (attachments & STENCIL) {
            // Usually depth+stencil are combined in one texture, but here separate
            Texture stencilTex;
            stencilTex.type = "FBO_STENCIL";
            glCreateTextures(GL_TEXTURE_2D, 1, &stencilTex.id);
            glTextureStorage2D(stencilTex.id, 1, GL_STENCIL_INDEX8, width, height);

            glNamedFramebufferTexture(fbo, GL_STENCIL_ATTACHMENT, stencilTex.id, 0);
            attachedTextures["STENCIL"] = std::move(stencilTex);
        }

        // Configure draw buffers
        if (drawBuffers.empty()) {
            glNamedFramebufferDrawBuffer(fbo, GL_NONE); // no color output
        }
        else {
            glNamedFramebufferDrawBuffers(fbo, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
        }

        // Check FBO completeness
        GLenum status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer incomplete! Status: " << status << std::endl;
        }
	}

	void FrameBuffer::DeleteFBO()
	{
        if (fbo) {
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }
		for (auto& attachedtex : attachedTextures) {
			if (attachedtex.second.id) glDeleteTextures(1, &attachedtex.second.id);
		}
		attachedTextures.clear();
	}
    std::string AttachmentToString(FrameBufferAttachments attachment)
    {
        // Assert that only one bit is set
        assert(attachment != FrameBufferAttachments::NONE &&
            (static_cast<int>(attachment) & (static_cast<int>(attachment) - 1)) == 0);

        switch (attachment) {
        case COLOR:
            return "COLOR";
        case DEPTH:
            return "DEPTH";
        case STENCIL:
            return "STENCIL";
        default:
            return "NONE";
        }

        return std::string();
    }
}