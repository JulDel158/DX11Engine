#pragma once

// lib
#include <glm/glm.hpp>

namespace dxe {

	struct alignas(16)ColoredVertex {
		glm::vec3 pos;
		glm::vec4 color;
	};

	struct View_t {
		glm::mat4 view{ 1.f };
		glm::mat4 projection{ 1.f };

		void setPerspectiveProjection(float fovy, float aspect, float n, float f);

		// these functions won't work in this engine
		void setViewDirection(glm::vec3 position, glm::vec3 direction,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		void LookAtTarget(glm::vec3 position, glm::vec3 target,
			glm::vec3 up = glm::vec3{ 0.f, 1.f, 0.f });

		// replaced by FPSViewRH
		// void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

		void FPSViewRH(glm::vec3 eye, float pitchrad, float yawrad);

		void dPrintViewMat();
	};

	struct Object_cb {
		glm::mat4 modeling{ 1.f };
	};

	struct Frame_cb {
		glm::mat4 view{ 1.f };
		glm::mat4 projection{ 1.f };
	};

} // namespace dxe