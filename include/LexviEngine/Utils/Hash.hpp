#pragma once

namespace Lexvi {
	namespace Hash {
		struct IVec2Hash {
			size_t operator()(const glm::ivec2& v) const noexcept {
				// 64-bit style mix, works fine for grid coords
				std::size_t h1 = std::hash<int>()(v.x);
				std::size_t h2 = std::hash<int>()(v.y);
				return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
			}
		};

		struct IVec2Equal {
			bool operator()(const glm::ivec2& a, const glm::ivec2& b) const noexcept {
				return a.x == b.x && a.y == b.y;
			}
		};
	}
}