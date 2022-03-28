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

	inline void swapFormat(aabb_t& box) {
		if (box.isMinMax) {
			box.center = (box.min + box.max) / 2.f;
			box.extent -= box.center;
		} else {
			box.min -= box.max;
			box.max = box.min + box.extent + box.extent;
		}

		box.isMinMax = !box.isMinMax;
	}

	inline const bool ComputeBounds(const aabb_t& ls, const aabb_t& rs, aabb_t& result) {
		// invalid input, boxes must be in the same format
		if (ls.isMinMax != rs.isMinMax) { return false; }

		result.isMinMax = ls.isMinMax;

		if (ls.isMinMax) { 
			// for the min max format we just need the smallest and largest values
			result.min = glm::min(ls.min, rs.min);
			result.max = glm::max(ls.max, rs.max);

		} else {
			result.max = glm::max(ls.center + ls.extent, rs.center + rs.extent);
			result.min = glm::min(ls.center - ls.extent, rs.center - rs.extent);

			result.center = (result.min + result.max) / 2.f;
			result.extent -= result.center;
		}

		return true;
	}

	inline const bool AabbToAabbCollision(const aabb_t& ls, const aabb_t& rs) {
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

	inline const bool RayToAabbCollision(const glm::vec3 pos, glm::vec3 direction, const aabb_t& target, glm::vec3& point) {
		// https://web.archive.org/web/20090803054252/http://tog.acm.org/resources/GraphicsGems/gems/RayBox.c
		glm::vec3 minB = (target.isMinMax) ? target.min : target.center - target.extent;
		glm::vec3 maxB = (target.isMinMax) ? target.max : target.center + target.extent;
		
		bool inside = true;
		char quadrant[3];
		int i = 0;
		int whichPlane = 0;
		glm::vec3 maxT{ 0.f };
		glm::vec3 candidatePlane{ 0.f };

		enum { RIGHT = 0, LEFT = 1, MIDDLE = 2};

		// find candidate planes
		for (i = 0; i < 3; ++i) {
			if (pos[i] < minB[i]) {
				quadrant[i] = LEFT;
				candidatePlane[i] = minB[i];
				inside = false;
			} else if (pos[i] > maxB[i]) {
				quadrant[i] = RIGHT;
				candidatePlane[i] = maxB[i];
				inside = false;
			} else {
				quadrant[i] = MIDDLE;
			}
		}

		// origin inside box
		if (inside) {
			point = pos;
			return true;
		}

		// calculate T distances to candidate planes
		for (i = 0; i < 3; ++i) {
			if (quadrant[i] != MIDDLE && direction[i] != 0.f) {
				maxT[i] = (candidatePlane[i] - pos[i]) / direction[i];
			} else {
				maxT[i] = -1;
			}
		}

		// get largest of the maxT's for final choice of intersection
		for (i = 1; i < 3; ++i) {
			if (maxT[whichPlane] < maxT[i]) {
				whichPlane = i;
			}
		}

		// check if final candidate is inside box
		if (maxT[whichPlane] < 0.f) { return false; }
		for (i = 0; i < 3; ++i) {
			if (whichPlane != i) {
				point[i] = pos[i] + maxT[whichPlane] * direction[i];
				if (point[i] < minB[i] || point[i] > maxB[i]) { return false; }
			} else {
				point[i] = candidatePlane[i];
			}
		}

		return true; // ray hits box
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

	inline const bool SphereToAabbCollision(const sphere_t& sphere, const aabb_t& target) {

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

	inline const bool RayToTriangleCollision(glm::vec3 pos, glm::vec3 dir, const glm::mat3 triangle, glm::vec3& intersectionPoint) {

		glm::vec3 E1 = triangle[1] - triangle[0];
		glm::vec3 E2 = triangle[2] - triangle[0];

		dir = glm::normalize(dir);
		glm::vec3 normal = (dir.y >= 0.f) ? glm::cross(E2, E1) : glm::cross(E1, E2);

		float det = -glm::dot(dir, normal);
		float invdet = 1.0f / det;

		glm::vec3 AO = pos - triangle[0];
		glm::vec3 DAO = (dir.y >= 0.f) ? glm::cross(dir, AO) : glm::cross(AO, dir);

		float u = glm::dot(E2, DAO) * invdet;
		float v = -glm::dot(E1, DAO) * invdet;
		float t = glm::dot(AO, normal) * invdet;

		intersectionPoint = pos + t * dir;
		return (det >= std::numeric_limits<float>::epsilon() && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
	}

} // namespace dxe