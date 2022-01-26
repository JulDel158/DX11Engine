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

	void debug_lines::addGrid() {
        const float size = 50;
        const float spacing = 2;
        const float lines = size / spacing;
        float dxz = -size / 2;
        //float z = dxz;
        const float cz = dxz;
        const float cx = dxz;
        int i = 0;

        for (int p = 0; p < lines; ++p)
        {
            //linex
            addLine(
                DirectX::XMFLOAT3(cx, 0.0f, dxz), // first point
                DirectX::XMFLOAT3(cx + size, 0.0f, dxz),  // second point
                DirectX::XMFLOAT4(1.0, 0.0f, 1.0f, 1.0f));   // color for both
            //linez
            addLine(
                DirectX::XMFLOAT3(dxz, 0.0f, cz), // first point
                DirectX::XMFLOAT3(dxz, 0.0f, cz + size),  // second point
                DirectX::XMFLOAT4(1.0, 0.0f, 1.0f, 1.0f));  // color for both

            dxz += spacing;
        }
        //drawing last line of the grid
        addLine(
            DirectX::XMFLOAT3(cx, 0.0f, dxz), // first point
            DirectX::XMFLOAT3(cx + size, 0.0f, dxz),  // second point
            DirectX::XMFLOAT4(1.0, 0.0f, 1.0f, 1.0f));   // color for both

        addLine(
            DirectX::XMFLOAT3(dxz, 0.0f, cz), // first point
            DirectX::XMFLOAT3(dxz, 0.0f, cz + size),  // second point
            DirectX::XMFLOAT4(1.0, 0.0f, 1.0f, 1.0f));  // color for both
	}

} // namespace dxe