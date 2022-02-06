#pragma once

#include <glm/glm.hpp>

namespace dxe {

	struct sphere {
		glm::vec3 pos{ 0.f };
		float radius{ 0.f };
	};

	inline const bool RayToSphereCollision(const glm::vec3 pos, glm::vec3 direction, const sphere target, float& time) {

		glm::vec3 targetTopos = pos - target.pos;
		direction = glm::normalize(direction);

		const float b = glm::dot(targetTopos, direction);
		// squared distance from start of ray to sphere surface
		const float c = glm::dot(targetTopos, targetTopos) - target.radius * target.radius;

		// ray starts outside of sphere and points away
		if (c > 0.f && b > 0.f) { return false; }

		const float discriminant = b * b - c;

		// the ray missed
		if (discriminant < 0.f) { return false; }

		time = -b - glm::sqrt(discriminant);
		if (time < 0.f) { time = 0.f; } // negative value means we started inside sphere, clamp to 0

		// we can calculate the point at which we collided like this
		// Vector3 hitpoint = pos + time * direction;

		// otherwise the ray hits
		return true;
	}


} // namespace dxe