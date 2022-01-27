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

		static void addDebugCube(glm::vec3 pos = glm::vec3{ 0.f, 0.f, 0.f }, float offset = 0.5f, glm::vec4 color = glm::vec4{1.f, 0.f, 0.f, 1.f});
	};

} // namespace dxe