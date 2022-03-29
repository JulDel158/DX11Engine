#pragma once

#include <array>
// #include <DirectXMath.h>
#include <glm/glm.hpp>

#include "collision.hpp"
#include "object_data.hpp"

namespace dxe::debug_lines {

		void clearLines();

		void addLine(const glm::vec3 apos, const glm::vec3 bpos, const glm::vec4 acolor, const glm::vec4 bcolor);

		void addLine(const glm::vec3 apos, const glm::vec3 bpos, const glm::vec4 color);

		const ColoredVertex* getLineVerts();

		size_t getLineVertCount();

		size_t getLineVertCapacity();

		void addGrid();

		void addDebugCube(glm::vec3 pos = glm::vec3{ 0.f, 0.f, 0.f }, float offset = 0.5f, glm::vec4 color = glm::vec4{1.f, 0.f, 0.f, 1.f});

		void addAabb(const aabb_t& box, glm::vec4 color = glm::vec4{1.f, 0.f, 0.f, 1.f});

		void rainbowUpdate(const float dt);
	

} // namespace dxe::debug::lines