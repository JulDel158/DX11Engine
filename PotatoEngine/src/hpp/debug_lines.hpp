#pragma once

#include <array>
// #include <DirectXMath.h>
#include <glm/glm.hpp>

#include "object_data.hpp"

namespace dxe {

	struct debug_lines {
		debug_lines() = delete;
		~debug_lines() = delete;
		debug_lines(const debug_lines&) = delete;
		debug_lines& operator=(const debug_lines&) = delete;

		static void clearLines();

		static void addLine(glm::vec3 apos, glm::vec3 bpos, glm::vec4 acolor, glm::vec4 bcolor);

		inline static void addLine(glm::vec3 apos, glm::vec3 bpos, glm::vec4 color) { addLine(apos, bpos, color, color); }

		static const ColoredVertex* getLineVerts();

		static size_t getLineVertCount();

		static size_t getLineVertCapacity();

		static void addGrid();
	};

} // namespace dxe