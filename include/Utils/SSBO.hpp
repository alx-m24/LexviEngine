#pragma once

#include <cstdint>

namespace Lexvi {
	struct SSBO {
		uint32_t id;
		uint32_t bindingPoint;
		size_t size;
	};

	void CreateSSBO(SSBO& ssbo, size_t size, uint32_t bindingPoint);
	void ResizeSSBO(SSBO& ssbo, size_t size);
	void UpdateSSBO(SSBO& ssbo, const void* data, size_t size, uint32_t offset);
	void DeleteSSBO(SSBO& ssbo);

	void BindSSBO(const SSBO& ssbo);

	void MemorySSBOBarrier();
	void MemoryShaderImageBarrier();
}