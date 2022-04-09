#pragma once

#include "collision.hpp"
#include "entity.hpp"
#include "object_data.hpp"

// lib
#include <glm/glm.hpp>

class Player : public entity {
public:



private:
	dxe::GameObject* gameObject{ nullptr };
	dxe::aabb_t boxCollider;
	glm::vec3 pos{ 0.f };

	float speed{ 1.f };
	float runSpeed{ 1.f };
	float sOffsetY{ 1.f };
	float cOffsetY{ 1.f };
	bool crouched{ false };

};
