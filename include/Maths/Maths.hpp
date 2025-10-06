#pragma once

namespace Lexvi {
	namespace Math {
        template<typename T>
        inline T map(T x, T in_min, T in_max, T out_min, T out_max, bool clampOutput = false) {
            if (in_min == in_max) return out_min;
            long double t = (long double)(x - in_min) / (long double)(in_max - in_min);
            long double result = (long double)out_min + t * (long double)(out_max - out_min);
            T r = static_cast<T>(result);
            if (clampOutput) {
                T low = std::min(out_min, out_max);
                T high = std::max(out_min, out_max);
                return std::min(std::max(r, low), high);
            }
            return r;
        }

        template<typename T>
        inline T map_vec(T x, T in_min, T in_max, T out_min, T out_max, bool clampOutput = false) {
            if (in_min == in_max) return out_min;
            T t = (x - in_min) / (in_max - in_min);
            T result = out_min + t * (out_max - out_min);
            if (clampOutput) {
                T low = glm::min(out_min, out_max);
                T high = glm::max(out_min, out_max);
                return glm::min(glm::max(result, low), high);
            }
            return result;
        }
	}
}