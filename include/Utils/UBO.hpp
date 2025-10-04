#pragma once

#include <cstdint>

namespace Lexvi {
	struct UBO {
		uint32_t id;
		uint32_t bindingPoint;
		size_t size;
	};

	void CreateUBO(UBO& ubo, size_t size, uint32_t bindingPoint);
	void UpdateUBO(UBO& ubo, const void* data, size_t size, uint32_t offset);
	void DeleteUBO(UBO& ubo);
}