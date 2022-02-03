#pragma once

// lib
#include <glm/glm.hpp>
#include <vector>

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

		void loadObject(const char* filepath, bool invertY = true);

		void dMakeCube(float offset);

		void dMakePlane();
	};

	struct GameObject {
		glm::mat4 transform{ 1.f };
		Objectdata model;
		uint32_t resourceId{ 0 };
		// TODO: IMPLEMENT TEXTURE REFERENCES ON OBJECTS, CONSIDER USING AN ID TO AN ARRAY OF ID3D11ShaderResourceView INSTEAD TO KEEP ABSTRACTION
		// ID3D11ShaderResourceView* resourceView;
	};

	struct View_t {
		glm::mat4 view{ 1.f };
		glm::mat4 rotationMat{ 1.f };
		glm::vec3 rotation{ 0.f };
		float rotationSpeed{ 100.0f };
		glm::vec3 position{ 0.f };
		float translationSpeed{ 25.5f };
		bool invertView{ false };

		void getView(const glm::vec4& translation = {0.f, 0.f, 0.f, 0.f});

		// these functions won't work in this engine
		void setViewDirection(glm::vec3 position, glm::vec3 direction,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		void LookAtTarget(glm::vec3 position, glm::vec3 target,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		void FPSViewRH(glm::vec3 eye, float pitchrad, float yawrad, bool invertView = 0);

		void UpdateView();

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
		glm::vec4 color{ 0.f }; // 16 BYTES
		glm::vec3 direction{ 0.f };	// 16 BYTES TOTAL: 32 BYTES
	};

	struct alignas(16)PointLight {
		glm::vec4 color{ 0.f }; // 16 BYTES
		glm::vec3 pos{ 0.f }; 
		float radius{ 0.f }; // 16 BYTES TOTAL: 32 BYTES
	};

	struct alignas(16)SpotLight {
		glm::vec4 color{ 0.f };  // 16 BYTES
		glm::vec3 pos{ 0.f };
		float att2{ 0.f }; // +16 BYTES
		glm::vec3 direction{ 0.f };
		float att1{ 0.f }; // +16 BYTES
		float range{ 0.f }; 
		float cone{ 0.f };
		float att3{ 0.f };
		//  +16 TOTAL: 64 BYTES
	};

	struct alignas(16)Scene_cb{
		PointLight pointLight; // 32 BYTES
		DirLight dirLight; // 32 BYTES
		SpotLight spotLight; // 64 BYTES
		glm::vec4 ambient{ 0.f }; // 16 BYTES TOTAL: 144 BYTES X-X
	};

} // namespace dxe