#pragma once

#include "LexviEngine/Application/ApplicationStack.hpp"
#include <LexviEngine/Window/Window.hpp>

namespace Lexvi {
	class Application {
		protected:
			bool m_isRunning = true;
            const Window* m_window;

		protected:
			ApplicationStack m_AppStack;
	
        public:
            Application() = default;
			virtual ~Application() {};
			
		public:
			virtual void Init() = 0;

		protected:
			virtual void Shutdown() = 0;
			virtual void AppUpdate() = 0;

		public:
			void Update();

			void Render();

			void FullShutdown();

		public:
			bool isRunning() const;
            void SetWindow(const Window* window) { m_window = window; }
	};
}
