#pragma once

#include "collision.hpp"
#include "bvh_node.hpp"

// lib
#include <glm/glm.hpp>

// std
#include <vector>
#include <string>

namespace dxe {

	struct alignas(16)ColoredVertex {
		glm::vec3 pos{ 0.f };
		glm::vec4 color{ 0.f };
	};

	struct alignas(16)ObjVertex {
		glm::vec3 pos{ 0.f };
		glm::vec3 nrm{ 0.f };
		glm::vec2 uv{ 0.f };

		bool operator==(const ObjVertex& other) const {
			return (pos == other.pos && uv == other.uv && nrm == other.nrm);
		}
	};

	struct Objectdata {
		std::vector<ObjVertex> vertices;
		std::vector<uint32_t> indices;

		void loadObject(const char* filepath, bool invertY = false);

		void dMakeCube(float offset);

		void dMakePlane();

		~Objectdata() { vertices.clear(); indices.clear(); }
	};

	struct GameObject {
		glm::mat4 transform{ 1.f };
		uint32_t resourceId{ 0 };
		bool isActive{ false };
		Objectdata model;
		// TODO: IMPLEMENT TEXTURE REFERENCES ON OBJECTS, CONSIDER USING AN ID TO AN ARRAY OF ID3D11ShaderResourceView INSTEAD TO KEEP ABSTRACTION
		// ID3D11ShaderResourceView* resourceView;

		void translatePosition(const glm::vec3 translation);
		void setPosition(const glm::vec3 position);
		void setScale(const float scale);
		void setScale(const glm::vec3 scale);
		void scaleMatrix(const float scale);
		void scaleMatrix(const glm::vec3 scale);
	};

	struct View_t {
		glm::mat4 view{ 1.f };
		glm::mat4 rotationMat{ 1.f };
		glm::vec3 rotation{ 0.f };
		float rotationSpeed{ 100.0f };
		glm::vec3 position{ 0.f };
		float translationSpeed{ 25.5f };
		bool invertView{ false };

		void updateView(const glm::vec4& translation = {0.f, 0.f, 0.f, 0.f});

		void setPosition(glm::vec3 pos);

		glm::vec3 getFoward()const;

		// these functions won't work in this engine
		void setViewDirection(glm::vec3 position, glm::vec3 direction,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		void LookAtTarget(glm::vec3 position, glm::vec3 target,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		void FPSViewRH(glm::vec3 eye, float pitchrad, float yawrad, bool invertView = 0);

		void dPrintViewMat();
	};

	struct Object_cb {
		glm::mat4 modeling{ 1.f };
	};

	struct Frame_cb {
		glm::mat4 view{ 1.f };
	};

	struct Window_cb {
		glm::mat4 projection{ 1.f };

		void setPerspectiveProjection(float fovy, float aspect, float nearz, float farz);
	};

	struct alignas(16)DirLight {
		glm::vec4 color{ 0.f };		// 16 BYTES
		glm::vec3 direction{ 0.f };	// 16 BYTES TOTAL: 32 BYTES
	};

	struct alignas(16)PointLight {
		glm::vec4 color{ 0.f };		// 16 BYTES
		glm::vec3 pos{ 0.f }; 
		float radius{ 0.f };		// 16 BYTES TOTAL: 32 BYTES
	};

	struct alignas(16)SpotLight {
		glm::vec4 color{ 0.f };		// 16 BYTES
		glm::vec3 pos{ 0.f };
		float innerRatio{ 0.f };	// +16 BYTES
		glm::vec3 direction{ 0.f };
		float outerRatio{ 0.f };	// +16 BYTES
		float range{ 0.f }; 
		float focus{ 0.f };
		float falloff{ 0.f };		// 12 
		//  + 4 padding TOTAL: 64 BYTES
	};

	struct alignas(16)Scene_cb{
		PointLight pointLight;		// 32 BYTES
		DirLight dirLight;			// 32 BYTES
		SpotLight spotLight;		// 64 BYTES
		glm::vec4 ambient{ 0.f };	// 16 BYTES TOTAL: 144 BYTES X-X
	};

	struct Textwrap {
		glm::vec4 color{0.f};		// 16
		glm::vec2 position{ 0.f };
		glm::vec2 origin{0.f};		// 16
		glm::vec2 scale{0.f};
		float rotation{0.f};
		float layer{0.f};			// 16
		std::wstring text;
		// TODO: ADD FONT TYPE ONCE WE ADD MORE FONTS INTO THE PROJECT
	};

	struct Particle {
		glm::vec3 pos{ 0.f };
		float scale{ 1.f };			// 16
		glm::vec3 velocity{ 0.f };
		float lifeSpan{ 0.f };		// 16
	};

	struct ParticleFlyWeight { // shared data among particles
		glm::vec3 velMinVals{ 0.f };
		float scaleStart{ 1.f };	// 16
		glm::vec3 velMaxVals{ 0.f };
		float scaleRate{ 1.f };		// 16
		uint32_t texture{ 0 };
		float minTime{ 0.f };
		float maxTime{ 0.f };		// 12
		// + 4 padding TOTAL: 48
	};

	struct Emitter {
		glm::vec3 pos{ 0.f };
		bool updated{ false };
		ParticleFlyWeight flyweigth;
		std::vector<Particle> particles;
	};

	// used to updated particles inside compute shader
	struct alignas(16)Particle_cb {
		glm::vec3 velMin{ 0.f };
		float scaleStart{ 1.f };	// 16
		glm::vec3 velMax{ 0.f };
		float scaleRate{ 0.f };		// 16
		glm::vec3 startPos{ 0.f };
		float minTime{ 0.f };		// 16
		float maxTime{ 0.f };
		float deltaTime{ 0.f };		// 8
	};

	struct Triangle_t {
		glm::vec3 pos[3];
		glm::vec3 centroid{ 0 };
		aabb_t box;
	};

	struct Triangle_i {
		uint32_t indx[3];
		glm::vec3 centroid{ 0 };
		aabb_t box;
	};

	struct Terrain {
		GameObject object;
		std::vector<bvh_node> tree;
		std::vector<Triangle_i> triangles;

		void loadTerrain(const char* filepath, const bool invertY = true, const bool minMaxFormat = false);

		void resizeBVH(const glm::mat4& transform);

		template<typename f>
		void traverseTree(const aabb_t& box, f& lamda);

		~Terrain() { tree.clear(); triangles.clear(); }

	private:
		template<typename f>
		bool traverseRecurse(const aabb_t& box, uint32_t inx, f &lamda, bool& check);

	};

} // namespace dxe