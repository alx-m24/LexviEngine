#pragma once

#include "Textures/Textures.hpp"

namespace Lexvi {
	enum FrameBufferAttachments {
		NONE = 0,
		DEPTH = 1 << 0,
		STENCIL = 1 << 1,
		COLOR = 1 << 2
	};
	
	inline FrameBufferAttachments operator|(FrameBufferAttachments a, FrameBufferAttachments b) {
		return static_cast<FrameBufferAttachments>(static_cast<int>(a) | static_cast<int>(b));
	}

	std::string AttachmentToString(FrameBufferAttachments attachment);

	class Input;

	class FrameBuffer {
	private:
		FrameBufferAttachments attachments = NONE;

	private:
		unsigned int fbo = 0;
		unsigned int width = 0, height = 0;
		unsigned int colorAttachmentNum = 0;

		std::unordered_map<std::string, Texture> attachedTextures;

	public:
		FrameBuffer() = default;
		~FrameBuffer();

		FrameBuffer(FrameBufferAttachments attachments, unsigned int width, unsigned int height) : attachments(attachments), width(width), height(height) { 
			CreateFrameBuffer();
		};

		FrameBuffer(FrameBufferAttachments attachments, unsigned int colorAttachmentNum, unsigned int width, unsigned int height) : attachments(attachments), colorAttachmentNum(colorAttachmentNum), width(width), height(height) { CreateFrameBuffer(); };

	public:
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;

		FrameBuffer(FrameBuffer&& other) noexcept {
			fbo = other.fbo;
			width = other.width;
			height = other.height;
			attachments = other.attachments;
			colorAttachmentNum = other.colorAttachmentNum;
			attachedTextures = std::move(other.attachedTextures);

			other.fbo = 0;
			other.width = 0;
			other.height = 0;
			other.attachments = NONE;
		}

		FrameBuffer& operator=(FrameBuffer&& other) noexcept {
			if (this != &other) {
				DeleteFBO(); // delete old resources

				fbo = other.fbo;
				width = other.width;
				height = other.height;
				attachments = other.attachments;
				colorAttachmentNum = other.colorAttachmentNum;
				attachedTextures = std::move(other.attachedTextures);

				other.fbo = 0;
				other.width = 0;
				other.height = 0;
				other.attachments = NONE;
			}
			return *this;
		}


	public:
		void AddAttachment(FrameBufferAttachments newAttachment);
		void ResizeFrameBuffer(unsigned int width, unsigned int height);

		void getFrameBufferSize(unsigned int& width, unsigned int& height) const;

		void BindFrameBuffer() const;
		void UnBindFrameBuffer() const;

		const Texture* getAttachment(FrameBufferAttachments attachment, unsigned int number = 0) const;

	private:
		void CreateFrameBuffer();
		void DeleteFBO();
	};
}