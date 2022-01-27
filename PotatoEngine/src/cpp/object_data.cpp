#include "../hpp/object_data.hpp"

// std
#include <cassert>
#include <limits>

namespace dxe {
	void View_t::setPerspectiveProjection(float fovy, float aspect, float near, float far)
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

	void View_t::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
		const glm::vec3 w{ glm::normalize(direction) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		view = glm::mat4{ 1.f };
		view[1][0] = u.y;
		view[2][0] = u.z;
		view[0][1] = v.x;
		view[0][0] = u.x;

		view[1][1] = w.z; // swapped with 2 2

		view[2][1] = v.z;
		view[0][2] = w.x;
		view[1][2] = w.y;

		view[2][2] = v.y;

		view[3][0] = -glm::dot(u, position);
		view[3][1] = -glm::dot(v, position);
		view[3][2] = -glm::dot(w, position); // swaped from - to +
	}

	void View_t::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
		setViewDirection(position, target - position, up);
	}

	void View_t::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
		const float c3 = glm::cos(rotation.z);
		const float s3 = glm::sin(rotation.z);
		const float c2 = glm::cos(rotation.x);
		const float s2 = glm::sin(rotation.x);
		const float c1 = glm::cos(rotation.y);
		const float s1 = glm::sin(rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		view = glm::mat4{ 1.f };
		view[0][0] = u.x;
		view[1][0] = u.y;
		view[2][0] = u.z;
		view[0][1] = v.x;
		view[1][1] = v.y;
		view[2][1] = v.z;
		view[0][2] = w.x;
		view[1][2] = w.y;
		view[2][2] = w.z;
		view[3][0] = glm::dot(u, position);
		view[3][1] = -glm::dot(v, position);
		view[3][2] = glm::dot(w, position);
	}

	void View_t::FPSViewRH(glm::vec3 eye, float pitch, float yaw)
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

		view = {
			glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0),
			glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0),
			glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0),
			glm::vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
		};
	}

}