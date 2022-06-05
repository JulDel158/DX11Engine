#include "../hpp/debug_lines.hpp"

#include<array>
#include <glm/gtx/compatibility.hpp>

namespace {
	constexpr size_t MAX_LINE_VERTS = 100000;

	size_t lineVertCount = 0;
	std::vector<dxe::ColoredVertex> lineVerts(MAX_LINE_VERTS);
    
    uint8_t colorIndex = 0;
   
    glm::vec4 colors[6] = {
        {1.f, 0.f, 0.f, 1.f}, // red
        {1.f, 1.f, 0.f, 1.f},
        {0.f, 1.f, 0.f, 1.f}, // green
        {0.f, 1.f, 1.f, 1.f},
        {0.f, 0.f, 1.f, 1.f}, // blue
        {1.f, 0.f, 1.f, 1.f}
    };

    glm::vec4 currentColor = { 1.f, 0.f, 0.f, 1.f };

    constexpr float totalTime = 2.f;
    float currTime = 0.f;
}

namespace dxe::debug_lines {

	void clearLines() { lineVertCount = 0; }

	void addLine(const glm::vec3 apos, const glm::vec3 bpos, const glm::vec4 acolor, const glm::vec4 bcolor) {
        if (lineVertCount + 2 >= MAX_LINE_VERTS) { return; } // prevent going out of bounds
		lineVerts[lineVertCount].pos = apos;
		lineVerts[lineVertCount++].color = acolor;
		lineVerts[lineVertCount].pos = bpos;
		lineVerts[lineVertCount++].color = bcolor;
	}

    void addLine(const glm::vec3 apos, const glm::vec3 bpos, const glm::vec4 color) {
        //addLine(apos, bpos, color, color);
        addLine(apos, bpos, color, color);
    }

	const ColoredVertex* getLineVerts() { return lineVerts.data(); }

	size_t getLineVertCount() { return lineVertCount; }

	size_t getLineVertCapacity() { return MAX_LINE_VERTS; }

	void addGrid() {
        const float size = 50;
        const float spacing = 1;
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
                glm::vec4(0.0, 0.2f, 1.0f, 1.0f));   // color for both
            //linez
            addLine(
                glm::vec3(dxz, 0.0f, cz), // first point
                glm::vec3(dxz, 0.0f, cz + size),  // second point
                glm::vec4(0.0, 0.2f, 1.0f, 1.0f));  // color for both

            dxz += spacing;
        }
        //drawing last line of the grid
        addLine(
            glm::vec3(cx, 0.0f, dxz), // first point
            glm::vec3(cx + size, 0.0f, dxz),  // second point
            glm::vec4(0.0, 0.2f, 1.0f, 1.0f));   // color for both

        addLine(
            glm::vec3(dxz, 0.0f, cz), // first point
            glm::vec3(dxz, 0.0f, cz + size),  // second point
            glm::vec4(0.0, 0.2f, 1.0f, 1.0f));  // color for both
	}

    void addDebugCube(glm::vec3 pos, float offset, glm::vec4 color) {

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

    void addAabb(aabb_t box, glm::vec4 color) {
        const glm::vec3 min = (box.isMinMax) ? box.min : box.center - box.extent;
        const glm::vec3 max = (box.isMinMax) ? box.max : box.center + box.extent;

        glm::vec3 p[6];
        //p1 = min
        p[0] = {max.x, min.y, min.z};
        p[1] = {min.x, min.y, max.z};
        p[2] = {max.x, min.y, max.z};
        p[3] = {min.x, max.y, min.z};
        p[4] = {max.x, max.y, min.z};
        p[5] = {min.x, max.y, max.z};
        //p8 = max

        // bottom h lines
        addLine(min, p[0], color);
        addLine(min, p[1], color);
        addLine(p[0], p[2], color);
        addLine(p[1], p[2], color);

        // bottom to top lines
        addLine(min, p[3], color);
        addLine(p[0], p[4], color);
        addLine(p[1], p[5], color);
        addLine(p[2], max, color);

        // top lines
        addLine(p[3], p[4], color);
        addLine(p[3], p[5], color);
        addLine(p[4], max, color);
        addLine(p[5], max, color);
    }

    void rainbowUpdate(const float dt) {
        currTime = (currTime + dt > totalTime) ? totalTime : currTime + dt;
        const float a = glm::clamp(currTime / totalTime, 0.f, 1.f);

        if (colorIndex > 5) { colorIndex = 0; }
        const uint8_t nextInx = (colorIndex == 5) ? 0 : colorIndex + 1;

        currentColor = glm::lerp(colors[colorIndex], colors[nextInx], a);

        for (size_t i = 0; i < lineVertCount; ++i) {
            lineVerts[i].color = currentColor;
        }

        if (currTime == totalTime){ 
            ++colorIndex;
            currTime = 0;
        }
    }

    


} // namespace dxe