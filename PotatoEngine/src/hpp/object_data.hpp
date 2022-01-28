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
		glm::mat4 rotationMat{ 1.f };
		glm::vec3 rotation{ 0.f };
		float rotationSpeed{ 25.0f };
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

		void setPerspectiveProjection(float fovy, float aspect, float n, float f);
	};

} // namespace dxe