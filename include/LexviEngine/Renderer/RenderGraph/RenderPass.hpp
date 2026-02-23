#pragma once

#include "ImageResource.hpp"

#include <unordered_map>

// Forward declaration
class Renderer;
class RenderContext;

namespace RenderGraph {
    class RenderPass {
        public:
            friend class RenderGraph;
            friend class ::Renderer;
        protected:
            std::string name = "";

            std::vector<std::string> reads {};
            std::vector<std::string> writes {};

            // Execution-time only
            std::unordered_map<std::string, ImageResource*> readImages;
            std::unordered_map<std::string, ImageResource*> writeImages;

            Renderer* renderer;

        public:
            RenderPass(std::string&& name, const std::vector<std::string>& reads, const std::vector<std::string>& writes)
                : name(std::move(name)), reads(std::move(reads)), writes(std::move(writes)) {}
            virtual ~RenderPass() = default;

        public:
            virtual void BeginPass(RenderContext& renderContext) = 0;
            virtual void RunPass(RenderContext& renderContext) = 0;
            virtual void EndPass(RenderContext& renderContext) = 0;

            bool operator==(const RenderPass& pass) const {
                return name == pass.name;
            }
    };

    template<typename T>
    concept isRenderPass = std::is_base_of_v<RenderPass, T>;

    template<typename T>
    concept RenderPass_T = isRenderPass<T>;
}
