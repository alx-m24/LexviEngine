#pragma once

#include "Bindable/Bindable.hpp"

namespace Lexvi {
	struct gBuffer {
		unsigned int id = 0;
		unsigned int positionTex = 0;
		unsigned int normalTex = 0;
		unsigned int color_roughTex = 0;
		unsigned int depthTex = 0;
	};

	class gBufferManager : public Bindable {
	private:
		gBuffer buffer{};

	public:
		gBufferManager() = default;
		gBufferManager(unsigned int width, unsigned int height) {
			Init(width, height);
		}

		~gBufferManager() {
			Delete();
		}

	public:
		void Init(unsigned int width, unsigned int height);
		void Resize(unsigned int width, unsigned int height);
		void Delete();

	public:
		void Bind() const override;
		void Unbind() const override;

		void BindTextures() const;

	public:
		struct SmartBind {
		private:
			int prevFBO = 0;

		public:
			SmartBind(const gBufferManager& gBufferMgr);
			~SmartBind();
		};
	};
}