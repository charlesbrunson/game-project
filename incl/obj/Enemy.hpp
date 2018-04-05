#ifndef ENEMY_H
#define ENEMY_H

#include "obj/GameObject.hpp"
#include "obj/Hurtbox.hpp"

class Enemy : public GameObject {
public:

	int spawnCounter = 0;
	bool respawning = true;
	bool removeOnDespawn = false;
	bool despawnOnExitCamera = true;
	bool activatedByDirector = false;
	bool forceSpawnOnActivate = false;
	bool active = true;


	Enemy(Level *l) : GameObject(l) {
		objectType = type::ENEMY;
		touchDamage = 1;
		killValue = 50;
	};

	bool isSleeping() const;
	bool isVisible();
	
	virtual void despawn();

	virtual void spawn();

	// Spawn if were within camera view
	virtual void forceSpawn();

	virtual void tryWake();

	virtual void setSpawn();

	virtual void update(sf::Time deltaTime);

	bool hurt(int amount, sf::Time stunTime);
	void kill();

	
	Hurtbox *getAttackBox() {
		return &attackBox;
	}

	bool usesAttackBox = false;

	int getKillValue() {
		return killValue;
	}

	bool isStunned() {
		return stun > sf::Time::Zero;
	}

protected:

	Hurtbox attackBox;

	bool stunImmune = false;

	bool onScreen = false;

	bool facePlayerOnSpawn = true;

	// Addition to camera borders to determine spawning
	int spawnMargin = 5;

	// Override this
	virtual void act(sf::Time deltaTime) = 0;

	// For enemies/bosses that need to keep track of what they're doing/last did
	int currentPattern = -1;

	sf::Time stun = sf::Time::Zero;

	bool initSpawn = false;
	sf::Vector2f spawnPos;
	bool spawnFlipped;

	bool asleep = true;

	// For point system?
	int killValue;

	bool tryForceSpawn = true;

	virtual void init() {};

	void facePlayer();
	
	void createDeathFX(sf::Vector2f offset) {
//		Effect *deathfx = new Effect(Effect::DEATH_GENERIC, false);
		Effect *deathfx = new Effect();
		deathfx->setPosition(getPosition() + offset);
		createEffect(deathfx);
	};

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		if (!asleep) {
			GameObject::draw(target, states);
#ifdef _DEBUG
			if (Gameplay_Globals::Debug::objectCollision && usesAttackBox && attackBox.getActive()) {
				target.draw(attackBox, states);
			}
#endif
		}
	}

};

#endif
