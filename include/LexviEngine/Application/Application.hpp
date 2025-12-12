#pragma once

#include "LexviEngine/Application/ApplicationStack.hpp"

namespace Lexvi {
	class Application {
		protected:
			bool m_isRunning = true;

		private:
			ApplicationStack m_AppStack;
	
		public:
			Application() = default;
			virtual ~Application() {};
			
		public:
			virtual void Init() = 0;
			virtual void Shutdown() = 0;

		public:
			void update();

			void render();
		public:
			bool isRunning() const;
	};
}
