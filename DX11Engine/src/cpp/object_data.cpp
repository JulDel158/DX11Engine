#include "../hpp/object_data.hpp"

// std
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
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
	void View_t::updateView(const glm::vec4& translation)
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

		invertView = true;
	}

	glm::vec3 View_t::getFoward() const {
		return glm::normalize(glm::vec3{view[2].x, view[2].y, view[2].z});
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
		projection[3][2] = -((far) / (far - near)) * near;
	}

	void Objectdata::loadObject(const char* filepath, bool invertY) {
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
				const int sign = (invertY) ? -1 : 1;
				if (index.vertex_index >= 0) {
					const std::size_t vertInx = static_cast<std::size_t>(index.vertex_index);
					vertex.pos = {
						attrib.vertices[3 * vertInx + 0],
						attrib.vertices[3 * vertInx + 1] * sign,
						attrib.vertices[3 * vertInx + 2]
					};

					/*vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};*/
				}

				if (index.normal_index >= 0) {
					const std::size_t nrmInx = static_cast<std::size_t>(index.normal_index);
					vertex.nrm = {
						attrib.normals[3 * nrmInx + 0],
						attrib.normals[3 * nrmInx + 1] * sign,
						attrib.normals[3 * nrmInx + 2]
					};
				}

				if (index.texcoord_index >= 0) {
					const std::size_t texCoordInx = static_cast<std::size_t>(index.texcoord_index);
					vertex.uv = {
						attrib.texcoords[2 * texCoordInx + 0],
						attrib.texcoords[2 * texCoordInx + 1]
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

	void Objectdata::dMakePlane() {
		ObjVertex p1;
		ObjVertex p2;
		ObjVertex p3;
		ObjVertex p4;

		p1.pos = { 0.f , 0.f, 0.f };
		p2.pos = { 0.f, 0.f, 1.f };
		p3.pos = { 1.f, 0.f, 0.f };
		p4.pos = { 1.f, 0.f, 1.f };

		p1.nrm = p2.nrm = p3.nrm = p4.nrm = { 0.f, 1.f, 0.f };

		p1.uv = { 0.f, 1.f };
		p2.uv = { 1.f, 1.f };
		p3.uv = {0.f, 0.f};
		p4.uv = {1.f, 0.f};

		vertices.push_back(p1);
		vertices.push_back(p2);
		vertices.push_back(p3);
		vertices.push_back(p4);

		// the 2 triangles that form the plane
		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(1);

		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(1);
	}

	void Terrain::loadTerrain(const char* filepath, const bool invertY, const bool minMaxFormat) {
		// 1. Load Mesh data
		// 2. Generate Triangle data
		// 3. Generate BVH tree


		// getting the object model for terrain
		object.model.loadObject(filepath, invertY);

		std::vector<int> tInds; // this will be shuffled when generating the tree to have some balance
		const int vertCount = static_cast<int>(object.model.indices.size()) / 3;

		//===================== generating triangle data =====================
		// preparing containers
		triangles.resize(vertCount);
		tInds.resize(vertCount);

		for (int i = 0, j = 0; i < triangles.size(); ++i) {
			tInds[i] = i;

			//glm::vec3 avg{ 0.f };

			// we must get the next triangle from our data
			for (int k = 0; k < 3; ++k) {
				triangles[i].indx[k] = object.model.indices[j++];
				triangles[i].centroid += object.model.vertices[ triangles[i].indx[k] ].pos; // average position of current triangle
			}
			triangles[i].centroid /= 3.f; // the average is the center of the triangle
			
			// initialize min & max for our aabb
			triangles[i].box.min = triangles[i].box.max = object.model.vertices[triangles[i].indx[0]].pos;

			// calculating the min and max for this triangle
			for (int k = 1; k < 3; ++k) {
				triangles[i].box.min = glm::min(triangles[i].box.min, object.model.vertices[triangles[i].indx[k]].pos);
				triangles[i].box.max = glm::max(triangles[i].box.max, object.model.vertices[triangles[i].indx[k]].pos);
			}

			triangles[i].box.isMinMax = minMaxFormat;
			if (!minMaxFormat) { // adjusting format to center and extent if enabled
				triangles[i].box.center = (triangles[i].box.min + triangles[i].box.max) / 2.f;
				triangles[i].box.extent -= triangles[i].box.center;
			}

		} // for each triangle

		// shuffling all triangles to prepare for tree generation
		std::random_device randDevice; // used to seed engine
		std::mt19937 randEngine(randDevice()); // pseudo random number generator engine
		std::shuffle(tInds.begin(), tInds.end(), randEngine);

		// ===================== generating bvh tree =====================
		bvh_node root = bvh_node(triangles[tInds[0]].box, tInds[0]);
		tree.push_back(root);

		// manhattan distance function
		auto manhDistance = [](const glm::vec3& lhs, const glm::vec3& rhs)->float {
			return glm::abs(lhs.x - rhs.x) + glm::abs(lhs.y - rhs.y) + glm::abs(lhs.z - rhs.z);
		};

		int curr = 0;
		for (int i = 1; i < tInds.size(); ++i) {

			bvh_node node = bvh_node(triangles[tInds[i]].box, tInds[i]);
			tree.push_back(node);

			curr = 0;

			// we need to reach a leaf so we can insert the next entry into the tree
			while (tree[curr].isBranch()) { 
				 ComputeBounds(tree[curr].aabb(), root.aabb(), tree[curr].aabb());
				 // we will used distance to balance the tree
				 const float leftDist = manhDistance(root.aabb().center, tree[tree[curr].left()].aabb().center);
				 const float rightDist = manhDistance(root.aabb().center, tree[tree[curr].right()].aabb().center);
				 
				 curr = (leftDist < rightDist) ? tree[curr].left() : tree[curr].right();
			}

			bvh_node child = bvh_node(tree[curr].aabb(), tree[curr].elementId());
			tree.push_back(child);

			ComputeBounds(tree[curr].aabb(), node.aabb(), tree[curr].aabb());
			tree[curr].left() =  static_cast<uint32_t>(tree.size()) - 2;
			tree[curr].right() = static_cast<uint32_t>(tree.size()) - 1;

		} // end for each index
	}

	void Terrain::resizeBVH(const glm::mat4& transform) {
		for (auto& node : tree) {
			
			node.aabb().center = static_cast<glm::mat3>(transform) * node.aabb().center;
			node.aabb().extent = static_cast<glm::mat3>(transform) * node.aabb().extent;
		}
	}

	void GameObject::translatePosition(const glm::vec3 translation) {
		transform[3] += glm::vec4(translation, 0.f);
	}

	void GameObject::setPosition(const glm::vec3 position) {
		transform[3] = glm::vec4(position, 1.f);
	}

	void GameObject::setScale(const float scale) {
		for (int i = 0; i < 3; ++i) {
			transform[i][i] = scale;
		}
	}

	void GameObject::setScale(const glm::vec3 scale) {
		for (int i = 0; i < 3; ++i) {
			transform[i][i] = scale[i];
		}
	}

	void GameObject::scaleMatrix(const float scale) {
		for (int i = 0; i < 3; ++i) {
			transform[i][i] *= scale;
		}
	}

	void GameObject::scaleMatrix(const glm::vec3 scale) {
		for (int i = 0; i < 3; ++i) {
			transform[i][i] *= scale[i];
		}
	}

} // namespace dxe