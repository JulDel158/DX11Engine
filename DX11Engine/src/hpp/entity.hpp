#pragma once

enum class ENT_TYPE {
	NONE = 0, PLAYER = 1, ENEMY = 2, NPC = 3, STATIC_OBJ = 4
};

class entity {
public:
	entity();
	entity(int health, int maxHealth, int id);
	entity(int health, int maxHealth, int id, ENT_TYPE type);
	~entity() = default;

	void takeDamage(int val);
	void heal(int val, bool overload = false);
	bool isDead() const;
	int getHp() const;
	int getMaxHp() const;
	unsigned long long getId() const;
	ENT_TYPE getType() const;

protected:
	int health;
	int maxHealth;
	unsigned long long id;
	ENT_TYPE type;
};

inline entity::entity() : health(0), maxHealth(0), id(0), type(ENT_TYPE::NONE) {}
inline entity::entity(int health, int maxHealth, int id) : health(health), maxHealth(maxHealth), id(id), type(ENT_TYPE::NONE) {}
inline entity::entity(int health, int maxHealth, int id, ENT_TYPE type) : health(health), maxHealth(maxHealth), id(id), type(type) {}

inline void entity::takeDamage(int val) { health -= val; }
inline void entity::heal(int val, bool overload) { health += maxHealth; if (health > maxHealth && !overload) { health = maxHealth; } }
inline bool entity::isDead() const { return health <= 0; }
inline int entity::getHp() const { return health; }
inline int entity::getMaxHp() const { return maxHealth; }
inline unsigned long long entity::getId() const { return id; }
inline ENT_TYPE entity::getType() const{ return type; }