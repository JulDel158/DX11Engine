#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace dxe {

	struct sphere_t {
		glm::vec3 pos{ 0.f };
		float radius{ 0.f };
	};

	struct aabb_t {
		union {
			glm::vec3 center;
			glm::vec3 min{std::numeric_limits<float>::max()};
		};
		bool isMinMax{ false };
		union {
			glm::vec3 extent;
			glm::vec3 max{ std::numeric_limits<float>::min() };
		};
	};


	inline const bool ComputeBounds(const aabb_t& ls, const aabb_t& rs, aabb_t& result) {
		// invalid input, boxes must be in the same format
		if (ls.isMinMax != rs.isMinMax) { return false; }

		result.isMinMax = ls.isMinMax;

		if (ls.isMinMax) { 
			// for the min max format we just need the smallest and largest values
			result.min = glm::min(ls.min, rs.min);
			result.max = glm::max(ls.max, ls.max);

		} else {

			result.max = glm::max(glm::vec3(ls.center + ls.extent), glm::vec3(rs.center + rs.extent));
			result.min = glm::min(ls.center, rs.center);

			result.center = glm::vec3(result.min + result.max) / 2.f;
			result.extent = glm::vec3(result.max - result.center);
		}

		return true;
	}

	inline const bool aabbToaabbCollision(const aabb_t& ls, const aabb_t& rs) {
		if (ls.isMinMax != rs.isMinMax) { // error, boxes in different formats
			return false;
		}

		glm::vec3 lmin{ 0.f }, lmax{ 0.f }, rmin{ 0.f }, rmax{ 0.f };

		if (ls.isMinMax) {
			lmin = ls.min;
			lmax = ls.max;
			rmin = rs.min;
			rmax = rs.max;
		} else { // center and extent reinterpretaion
			lmin = ls.center - ls.extent;
			lmax = ls.center + ls.extent;
			rmin = rs.center - rs.extent;
			rmax = rs.center + rs.extent;
		}

		// since aabb boxes do not rotate, we can perform a simple check along each axis to determine if any of the points is outside the bounds of the other box
		// if all points are within all bounds, then and only then we have a collision
		if (lmax.x < rmin.x || lmin.x > rmax.x) { return false; }
		if (lmax.y < rmin.y || lmin.y > rmax.y) { return false; }
		if (lmax.z < rmin.z || lmin.z > rmax.z) { return false; }

		return true;
	}

	inline const bool RayToSphereCollision(const glm::vec3 pos, glm::vec3 direction, const sphere_t& target, float& time) {

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

	inline const bool SphereToSphereCollision(const sphere_t& ls, const sphere_t& rs) {

		if (ls.pos == rs.pos) { return true; }

		const glm::vec3 l_to_r = rs.pos - ls.pos;

		const float distancesqr = (l_to_r.x * l_to_r.x) + (l_to_r.y * l_to_r.y) + (l_to_r.z * l_to_r.z);

		return (distancesqr <= (ls.radius + rs.radius) * (ls.radius + rs.radius));
	}

	inline const bool SphereToaabbCollision(const sphere_t& sphere, const aabb_t& target) {

		glm::vec3 closestPoint{ 0.f };
		
		if (target.isMinMax) {
			closestPoint = glm::clamp(sphere.pos, target.min, target.max);
		} else {
			closestPoint = glm::clamp(sphere.pos, target.center - target.extent, target.center + target.extent);
		}

		const float distancesqr = (closestPoint.x * closestPoint.x) + (closestPoint.y * closestPoint.y) + (closestPoint.z * closestPoint.z);

		if (distancesqr <= (sphere.radius * sphere.radius)) { return true; }

		return false;
	}

} // namespace dxe