#include "pch.h"

#include "Utils/SSBO.hpp"

namespace Lexvi {
	void CreateSSBO(SSBO& ssbo, size_t size, uint32_t bindingPoint)
	{
		glCreateBuffers(1, &ssbo.id);
		glNamedBufferData(ssbo.id, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo.id);
		ssbo.size = size;
		ssbo.bindingPoint = bindingPoint;
	}

	void UpdateSSBO(SSBO& ssbo, const void* data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(ssbo.id, offset, size, data);
	}

	void ResizeSSBO(SSBO& ssbo, size_t size) {
		if (ssbo.id) DeleteSSBO(ssbo);
		CreateSSBO(ssbo, size, ssbo.bindingPoint);
	}

	void DeleteSSBO(SSBO& ssbo)
	{
		glDeleteBuffers(1, &ssbo.id);
		ssbo.id = 0;
	}

	void BindSSBO(const SSBO& ssbo) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo.bindingPoint, ssbo.id);
	}

	void MemorySSBOBarrier()
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void MemoryShaderImageBarrier()
	{
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}