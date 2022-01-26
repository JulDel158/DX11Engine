#pragma once

#include <array>
#include <DirectXMath.h>

#include "object_data.hpp"

namespace dxe {

	struct debug_lines {
		debug_lines() = default;
		~debug_lines() = default;

		static void clearLines() { lineVertCount = 0; }

		static void addLine(DirectX::XMFLOAT3 apos, DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT4 acolor, DirectX::XMFLOAT4 bcolor) {
			lineVerts[lineVertCount].pos = apos;
			lineVerts[lineVertCount++].color = acolor;
			lineVerts[lineVertCount].pos = bpos;
			lineVerts[lineVertCount++].color = bcolor;
		}

		static const ColoredVertex* getLineVerts() { return lineVerts.data(); }

		static size_t getLineVertCount() { return lineVertCount; }

		static size_t getLineVertCapacity() { return MAX_LINE_VERTS; }

	private:
		static constexpr size_t MAX_LINE_VERTS = 2000;

		static size_t lineVertCount;
		static std::array<ColoredVertex, MAX_LINE_VERTS> lineVerts;
	};

} // namespace dxe