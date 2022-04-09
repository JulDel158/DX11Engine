#pragma once

enum class ENT_TYPE {
	PLAYER = 0, ENEMY, NPC, STATIC_OBJ 
};

class entity {

public:
	entity();
	entity(int health, int maxHealth, int id);
	~entity() = default;

	void takeDamage(int val);
	void heal(int val, bool overload = false);
	bool isDead() const;
	int getHp() const;
	int getMaxHp() const;
	unsigned long long getId() const;

protected:
	int health;
	int maxHealth;
	unsigned long long id;
};

inline entity::entity() : health(0), maxHealth(0), id(0) {}
inline entity::entity(int health, int maxHealth, int id) : health(health), maxHealth(maxHealth), id(id) {}

void entity::takeDamage(int val) { health -= val; }
void entity::heal(int val, bool overload) { health += maxHealth; if (health > maxHealth && !overload) { health = maxHealth; } }
bool entity::isDead() const { return health <= 0; }
unsigned long long entity::getId() const { return id; }
int entity::getHp() const { return health; }
int entity::getMaxHp() const { return maxHealth; }