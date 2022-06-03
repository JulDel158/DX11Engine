#pragma once

// the functions of this file are to generate simple mesh models internally without loading them from disk

#include "object_data.hpp"

namespace dxe {
	
	inline void MakeCube(Objectdata& obj, float offset) {
		// bottom
		ObjVertex p1;
		ObjVertex p2;
		ObjVertex p3;
		ObjVertex p4;
		// top
		ObjVertex p5;
		ObjVertex p6;
		ObjVertex p7;
		ObjVertex p8;

		// TODO: FIX NORMALS (WILL NEED TO ADD VERTICES PER FACE)
		// not correct but will allow me to see the actual color of the obj for now
		p1.nrm = p2.nrm = p3.nrm = p4.nrm = p5.nrm = p6.nrm = p7.nrm = p8.nrm = { 0.f, 1.f, 1.f };

		p1.pos = glm::vec3(-offset, -offset, -offset);
		p2.pos = glm::vec3(-offset, -offset, +offset);
		p3.pos = glm::vec3(+offset, -offset, -offset);
		p4.pos = glm::vec3(+offset, -offset, +offset);

		p5.pos = glm::vec3(-offset, +offset, -offset);
		p6.pos = glm::vec3(-offset, +offset, +offset);
		p7.pos = glm::vec3(+offset, +offset, -offset);
		p8.pos = glm::vec3(+offset, +offset, +offset);

		obj.vertices.reserve(8);
		obj.vertices.push_back(p1);
		obj.vertices.push_back(p2);
		obj.vertices.push_back(p3);
		obj.vertices.push_back(p4);
		obj.vertices.push_back(p5);
		obj.vertices.push_back(p6);
		obj.vertices.push_back(p7);
		obj.vertices.push_back(p8);

		obj.indices.reserve(36);
		// bottom 0 1 2 3
		obj.indices.push_back(0); obj.indices.push_back(1); obj.indices.push_back(2);
		obj.indices.push_back(1); obj.indices.push_back(3); obj.indices.push_back(2);

		// top 4 5 6 7
		obj.indices.push_back(4); obj.indices.push_back(5); obj.indices.push_back(6);
		obj.indices.push_back(5); obj.indices.push_back(7); obj.indices.push_back(6);

		// front 0 2 4 6
		obj.indices.push_back(0); obj.indices.push_back(4); obj.indices.push_back(2);
		obj.indices.push_back(4); obj.indices.push_back(6); obj.indices.push_back(2);

		// back 1 3 5 7
		obj.indices.push_back(1); obj.indices.push_back(5); obj.indices.push_back(3);
		obj.indices.push_back(5); obj.indices.push_back(7); obj.indices.push_back(3);

		// left 0 1 4 5
		obj.indices.push_back(1); obj.indices.push_back(4); obj.indices.push_back(0);
		obj.indices.push_back(1); obj.indices.push_back(5); obj.indices.push_back(4);

		// right 2 3 6 7
		obj.indices.push_back(2); obj.indices.push_back(3); obj.indices.push_back(6);
		obj.indices.push_back(3); obj.indices.push_back(7); obj.indices.push_back(6);
	}

	inline void MakePlane(Objectdata& obj, float width, float lenght) {
		// width determines the size in the x axis, and lenght in the z axis
		ObjVertex p1;
		ObjVertex p2;
		ObjVertex p3;
		ObjVertex p4;

		// dividing by 2
		width /= 2;
		lenght /= 2;

		p1.pos = { -width, 0.f,  lenght }; // - X + Z TOP LEFT
		p2.pos = { width, 0.f,  lenght }; // + X + Z TOP RIGHT
		p3.pos = { -width, 0.f, -lenght }; // - X - Z BOTTOM LEFT
		p4.pos = { width, 0.f, -lenght }; // + X - Z BOTTOM RIGHT

		p1.nrm = p2.nrm = p3.nrm = p4.nrm = { 0.f, 1.f, 0.f };

		p1.uv = { 0.f, 0.f };
		p2.uv = { 1.f, 0.f };
		p3.uv = { 0.f, 1.f };
		p4.uv = { 1.f, 1.f };

		obj.vertices.reserve(4); // 4 indices
		obj.indices.reserve(6); // 2 triangles(3 each)

		obj.vertices.push_back(p1);
		obj.vertices.push_back(p2);
		obj.vertices.push_back(p3);
		obj.vertices.push_back(p4);

		obj.indices.push_back(0);
		obj.indices.push_back(1);
		obj.indices.push_back(2);

		obj.indices.push_back(2);
		obj.indices.push_back(1);
		obj.indices.push_back(3);
	}

} // namespace dxe