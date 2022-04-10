#pragma once

#include "collision.hpp"
#include "entity.hpp"
#include "object_data.hpp"

// lib
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Player : public entity {
public:
	Player();
	Player(int maxHealth, int id);
	Player(int maxHealth, int id, float speed, float runSpeed, float standOffset, float crouchOffset, float jumpForce);
	~Player() = default;

	bool isCrouched() const;
	bool isRunning() const;
	void toggleCrouch();
	void setCrouch(bool crouch);
	void toggleRun();
	void setRun(bool run);
	glm::vec3 getPosition() const;
	glm::vec3 getRotation() const;
	dxe::aabb_t getBox() const;
	void setPosition(glm::vec3 position);
	void translate(glm::vec3 translation, bool yAxisMovement = false);
	void FPControls(glm::vec3 translation, float dx, float dy);
	void resizeCollider(glm::vec3 extent);
	void addForce(float yForce);
	void applyForce();
	void update(float dt);
	
private:
	dxe::GameObject* gameObject{ nullptr };
	dxe::aabb_t boxCollider;
	glm::vec3 pos{ 0.f };
	glm::vec3 rotation{ 0.f };
	bool crouched{ false };
	bool running{ false };
	float cameraSensitivity{ 0.75f };
	float yAcceleration{ 0.f };

public:
	float speed{ 1.f };
	float runSpeed{ 1.f };
	float standingOffset{ 1.f };
	float crouchingOffset{ 1.f };
	float currentOffset{ 1.f }; // We should interpolate from offsets when transitioning from crouch to standing
	float crouchSpeed{ 1.f };
	float jumpForce{ 1.f };
};

inline Player::Player() : entity(100, 100, 0, ENT_TYPE::PLAYER) {}
inline Player::Player(int maxHealth, int id) : entity(maxHealth, maxHealth, id, ENT_TYPE::PLAYER) {}
inline Player::Player(int maxHealth, int id, float speed, float runSpeed, float standOffset, float crouchOffset, float jumpForce) :
	entity(maxHealth, maxHealth, id, ENT_TYPE::PLAYER),
	pos(0.f),
	rotation(0.f),
	crouched(false),
	running(false),
	cameraSensitivity( 0.75f),
	speed(speed),
	runSpeed(runSpeed),
	standingOffset(standOffset),
	crouchingOffset(crouchOffset),
	currentOffset(standOffset),
	crouchSpeed(50.f),
	jumpForce(jumpForce) {}

inline bool Player::isCrouched() const{ return crouched; }
inline bool Player::isRunning() const{ return running; }
inline void Player::toggleCrouch() { crouched = !crouched; }
inline void Player::setCrouch(bool crouch) { crouched = crouch; }
inline void Player::toggleRun() { running = !running; }
inline void Player::setRun(bool run) { running = run; }
inline glm::vec3 Player::getPosition() const{ return pos; }
inline glm::vec3 Player::getRotation() const { return rotation; }
inline dxe::aabb_t Player::getBox() const { return boxCollider; }
inline void Player::setPosition(glm::vec3 position) { 
	pos = boxCollider.center = position;
	boxCollider.center.y += crouched ? crouchingOffset : standingOffset;
	if (gameObject) gameObject->setPosition(position);
}
inline void Player::translate(glm::vec3 translation, bool yAxisMovement) {
	glm::mat4 rotationMat{ 1.f }, rotY{ 1.f };
	rotY = glm::rotate(rotationMat, glm::radians(rotation.y), { 0.f, 1.f, 0.f });
	rotationMat = rotY * glm::rotate(rotationMat, glm::radians(rotation.x), { 1.f, 0.f, 0.f });
	glm::vec4 foward = rotY * glm::vec4(translation, 0.f); // we get the foward based on just the Y rotation so that movement can be consitent regardles of camera rotation
	pos.x += foward.x;
	pos.z += foward.z;
	if (yAxisMovement) { 
		pos.y += foward.y;
	}
}
inline void Player::FPControls(glm::vec3 translation, float dx, float dy) {
	rotation.x += dx * cameraSensitivity;
	rotation.y += dy * cameraSensitivity;
	rotation.x = glm::clamp(rotation.x, -89.9f, 89.9f);
	translate(translation);
}
inline void Player::resizeCollider(glm::vec3 extent) {
	boxCollider.extent = extent;
	boxCollider.center = pos;
	boxCollider.isMinMax = false;
}

inline void Player::addForce(float yForce) {
	yAcceleration += yForce;
}

inline void Player::applyForce() {
	pos.y += yAcceleration;
	yAcceleration = 0.f;
}

inline void Player::update(float dt) {
	if (crouched) {
		currentOffset -= crouchSpeed * dt;
	} else {
		currentOffset += crouchSpeed * dt;
	}

	currentOffset = glm::clamp(currentOffset, crouchingOffset, standingOffset);
}
