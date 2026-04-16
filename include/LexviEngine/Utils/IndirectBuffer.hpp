#pragma once

namespace Lexvi {
	struct DrawElementsIndirectCommand {
		GLsizei count = 0;
		GLsizei instanceCount = 0;
		GLsizei firstIndex = 0;
		GLsizei baseVertex = 0;
		GLsizei baseInstance = 0;
	};
}