#include "../hpp/debug_lines.hpp"

#include<array>


namespace {
	constexpr size_t MAX_LINE_VERTS = 2000;

	size_t lineVertCount = 0;
	std::array<dxe::ColoredVertex, MAX_LINE_VERTS> lineVerts;
}

namespace dxe {

	void debug_lines::clearLines() { lineVertCount = 0; }

	void debug_lines::addLine(glm::vec3 apos, glm::vec3 bpos, glm::vec4 acolor, glm::vec4 bcolor) {
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
                glm::vec3(cx, 0.0f, dxz), // first point
                glm::vec3(cx + size, 0.0f, dxz),  // second point
                glm::vec4(1.0, 0.0f, 1.0f, 1.0f));   // color for both
            //linez
            addLine(
                glm::vec3(dxz, 0.0f, cz), // first point
                glm::vec3(dxz, 0.0f, cz + size),  // second point
                glm::vec4(1.0, 0.0f, 1.0f, 1.0f));  // color for both

            dxz += spacing;
        }
        //drawing last line of the grid
        addLine(
            glm::vec3(cx, 0.0f, dxz), // first point
            glm::vec3(cx + size, 0.0f, dxz),  // second point
            glm::vec4(1.0, 0.0f, 1.0f, 1.0f));   // color for both

        addLine(
            glm::vec3(dxz, 0.0f, cz), // first point
            glm::vec3(dxz, 0.0f, cz + size),  // second point
            glm::vec4(1.0, 0.0f, 1.0f, 1.0f));  // color for both
	}

    void debug_lines::addDebugCube(glm::vec3 pos, float offset, glm::vec4 color) {

        // bottom
        glm::vec3 p1;
        glm::vec3 p2;
        glm::vec3 p3;
        glm::vec3 p4;
        // top
        glm::vec3 p5;
        glm::vec3 p6;
        glm::vec3 p7;
        glm::vec3 p8;

        p1 = glm::vec3(pos.x - offset, pos.y - offset, pos.z - offset);
        p2 = glm::vec3(pos.x - offset, pos.y - offset, pos.z + offset);
        p3 = glm::vec3(pos.x + offset, pos.y - offset, pos.z - offset);
        p4 = glm::vec3(pos.x + offset, pos.y - offset, pos.z + offset);

        p5 = glm::vec3(pos.x - offset, pos.y + offset, pos.z - offset);
        p6 = glm::vec3(pos.x - offset, pos.y + offset, pos.z + offset);
        p7 = glm::vec3(pos.x + offset, pos.y + offset, pos.z - offset);
        p8 = glm::vec3(pos.x + offset, pos.y + offset, pos.z + offset);

        addLine(p1, p2, color);
        addLine(p1, p3, color);
        addLine(p2, p4, color);
        addLine(p4, p3, color);
                         
        addLine(p1, p5, color);
        addLine(p2, p6, color);
        addLine(p3, p7, color);
        addLine(p4, p8, color);
                        
        addLine(p5, p6, color);
        addLine(p5, p7, color);
        addLine(p6, p8, color);
        addLine(p8, p7, color);
    }

} // namespace dxe