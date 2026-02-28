#pragma once

#include "LexviEngine/Application/ApplicationStack.hpp"
#include "LexviEngine/Renderer/RenderGraph/RenderGraph.hpp"

class Renderer;

namespace Lexvi {
	class Application {
		protected:
			bool m_isRunning = true;

		protected:
			ApplicationStack m_AppStack;

            ::Renderer& m_renderer;
	
        public:
            Application(Renderer& renderer) : m_renderer(renderer) {}
			virtual ~Application() {};
			
		public:
			virtual void Init() = 0;

		protected:
			virtual void Shutdown() = 0;
			virtual void AppUpdate() = 0;

		public:
			void Update();

			void FullShutdown();

		public:
			bool isRunning() const;
	};
}
