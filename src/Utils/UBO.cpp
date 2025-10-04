#include "pch.h"

#include "Utils/UBO.hpp"

namespace Lexvi {
	void CreateUBO(UBO& ubo, size_t size, uint32_t bindingPoint)
	{
		glCreateBuffers(1, &ubo.id);
		glNamedBufferData(ubo.id, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo.id);
		ubo.size = size;
		ubo.bindingPoint = bindingPoint;
	}

	void UpdateUBO(UBO& ubo, const void* data, size_t size, uint32_t offset)
	{
		glNamedBufferSubData(ubo.id, offset, size, data);
	}

	void DeleteUBO(UBO& ubo)
	{
		glDeleteBuffers(1, &ubo.id);
		ubo.id = 0;
	}
}