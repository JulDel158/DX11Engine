#include "../hpp/debug_lines.hpp"

#include<array>


namespace {
	constexpr size_t MAX_LINE_VERTS = 2000;

	size_t lineVertCount = 0;
	std::array<dxe::ColoredVertex, MAX_LINE_VERTS> lineVerts;
}

namespace dxe {

	void debug_lines::clearLines() { lineVertCount = 0; }

	void debug_lines::addLine(DirectX::XMFLOAT3 apos, DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT4 acolor, DirectX::XMFLOAT4 bcolor) {
		lineVerts[lineVertCount].pos = apos;
		lineVerts[lineVertCount++].color = acolor;
		lineVerts[lineVertCount].pos = bpos;
		lineVerts[lineVertCount++].color = bcolor;
	}

	const ColoredVertex* debug_lines::getLineVerts() { return lineVerts.data(); }

	size_t debug_lines::getLineVertCount() { return lineVertCount; }

	size_t debug_lines::getLineVertCapacity() { return MAX_LINE_VERTS; }

} // namespace dxe