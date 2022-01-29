#include "../hpp/object_data.hpp"

// std
#include <cassert>
#include <limits>
#include <iostream>
#include <unordered_map>


// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "../hpp/file_reader.hpp"

namespace std {
	template<>
	struct hash<dxe::ObjVertex> {
		size_t operator()(dxe::ObjVertex const& vertex) const {
			size_t seed = 0;
			tools::hashCombine(seed, vertex.pos, vertex.nrm, vertex.uv);
			return seed;
		}
	};
} // namespace std

namespace dxe {
	void View_t::getView(const glm::vec4& translation)
	{
		glm::mat4 rotZ{ 1.f }, rotY{ 1.f };
		rotationMat = glm::mat4{ 1.f };

		rotY = glm::rotate(rotY, glm::radians(rotation.y), { 0.f, 1.f, 0.f });

		rotZ = glm::rotate(rotZ, glm::radians(rotation.z), { 0.f, 0.f, 1.f });

		rotationMat = glm::rotate(rotationMat, glm::radians(rotation.x), { 1.f, 0.f, 0.f });

		rotationMat = rotY * rotationMat;
		rotationMat = rotZ * rotationMat;
		
		const glm::vec4 moveDir = rotationMat * translation;

		position.x += moveDir.x;
		position.y += moveDir.y;
		position.z += moveDir.z;

		view = rotationMat;

		view[3][0] = position.x;
		view[3][1] = position.y;
		view[3][2] = position.z;
	}

	void View_t::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
		glm::vec3 z = glm::normalize(direction);
		glm::vec3 x = glm::normalize(glm::cross(up, z));
		glm::vec3 y = glm::normalize(glm::cross(z, x));

		view = {
			glm::vec4(x.x, x.y, x.z, 0.f),
			glm::vec4(y.x, y.y, y.z, 0.f),
			glm::vec4(z.x, z.y, z.z, 0.f),
			glm::vec4(position.x, position.y, position.z, 1.f)
		};

		//// x
		//view[0][0] = x.x;
		//view[0][1] = x.y;
		//view[0][2] = x.z;

		//// y
		//view[1][0] = y.x;
		//view[1][1] = y.y;
		//view[1][2] = y.z;

		//// z
		//view[2][0] = z.x;
		//view[2][1] = z.y;
		//view[2][2] = z.z;

		//// position
		//view[3][0] = position.x;
		//view[3][1] = position.y;
		//view[3][2] = position.z;
	}

	void View_t::LookAtTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		setViewDirection(position, target - position, up);
	}

	void View_t::FPSViewRH(glm::vec3 eye, float pitch, float yaw, bool inverView)
	{
		// I assume the values are already converted to radians.
		float cosPitch = glm::cos(pitch);
		float sinPitch = glm::sin(pitch);
		float cosYaw = glm::cos(yaw);
		float sinYaw = glm::sin(yaw);
		glm::vec3 xaxis = { cosYaw, 0, -sinYaw };
		glm::vec3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
		glm::vec3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

		// Create a 4x4 view matrix from the right, up, forward and eye position vectors
		view = glm::mat4{
			glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0.f),
			glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0.f),
			glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0.f),
			glm::vec4(-glm::dot(xaxis, eye), -glm::dot(yaxis, eye), -glm::dot(zaxis, eye), 1.f)
		};

		if (inverView) { view = glm::inverse(view); }
	}

	void View_t::UpdateView() {
		FPSViewRH(position, -rotation.x, rotation.y, invertView);
	}

	void View_t::dPrintViewMat() {
	#ifndef NDEBUG
		std::cout << "--------------VIEW MATRIX--------------\n";
		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x) {
				std::cout << view[y][x] << "        ";
			}
			std::cout << std::endl;
		}
		std::cout << "--------------VIEW MATRIX--------------\n\n\n";
	#endif // NDEBUG
	}

	void Window_cb::setPerspectiveProjection(float fovy, float aspect, float near, float far)
	{
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fovy / 2.f);
		projection = glm::mat4{ 0.0f };
		projection[0][0] = 1.f / (aspect * tanHalfFovy);
		projection[1][1] = 1.f / (tanHalfFovy);
		projection[2][2] = far / (far - near);
		projection[2][3] = 1.f;
		projection[3][2] = -(far * near) / (far - near);
	}

	void Objectdata::loadObject(const char* filepath) {
		tinyobj::attrib_t attrib; // stores pos, color, normal, and uv coordinates data
		std::vector<tinyobj::shape_t> shapes; // index values for each face element
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath)) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<ObjVertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				ObjVertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						-attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					/*vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};*/
				}

				if (index.normal_index >= 0) {
					vertex.nrm = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		} // end for loops
	}

	void Objectdata::dMakeCube(float offset) {
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

		p1.pos = glm::vec3(-offset, -offset, -offset);
		p2.pos = glm::vec3(-offset, -offset, +offset);
		p3.pos = glm::vec3(+offset, -offset, -offset);
		p4.pos = glm::vec3(+offset, -offset, +offset);

		p5.pos = glm::vec3(-offset, +offset, -offset);
		p6.pos = glm::vec3(-offset, +offset, +offset);
		p7.pos = glm::vec3(+offset, +offset, -offset);
		p8.pos = glm::vec3(+offset, +offset, +offset);

		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p3);
		vertices.push_back(p4);
		vertices.push_back(p5);
		vertices.push_back(p6);
		vertices.push_back(p7);
		vertices.push_back(p8);

		// bottom 0 1 2 3
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(1); indices.push_back(3); indices.push_back(2);

		// top 4 5 6 7
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(5); indices.push_back(7); indices.push_back(6);

		// front 0 2 4 6
		indices.push_back(0); indices.push_back(4); indices.push_back(2);
		indices.push_back(4); indices.push_back(6); indices.push_back(2);

		// back 1 3 5 7
		indices.push_back(1); indices.push_back(5); indices.push_back(3);
		indices.push_back(5); indices.push_back(7); indices.push_back(3);

		// left 0 1 4 5
		indices.push_back(1); indices.push_back(4); indices.push_back(0);
		indices.push_back(1); indices.push_back(5); indices.push_back(4);

		// right 2 3 6 7
		indices.push_back(2); indices.push_back(3); indices.push_back(6);
		indices.push_back(3); indices.push_back(7); indices.push_back(6);
	}

} // namespace dxe