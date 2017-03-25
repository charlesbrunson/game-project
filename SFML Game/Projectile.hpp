#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/System.hpp>
#include "GameObject.hpp"
#include "Hurtbox.hpp"
#include "PositionRef.hpp"

class Projectile : public GameObject {

public:
	Projectile(ResourceLoader *rloader, Level *l, int ID = -1) : GameObject(rloader, l, ID) {
		maxLifeTime = sf::seconds(10.f);
		lifeTime = sf::Time::Zero;

		objectType = GameObject::type::PROJECTILE_ENEMY;

		hurtBox.setActive(true);
	};

	virtual void update(sf::Time deltaTime) {
		lifeTime += deltaTime;

		if (lifeTime >= maxLifeTime) {
			toDelete = true;
		}
		if (removeIfOutOfLevel && !((sf::FloatRect)(lvl->levelArea)).intersects(collisionBox)) {
			toDelete = true;
		}
		if (removeIfOffScreen && !getCameraArea().intersects(collisionBox)) {
			toDelete = true;
		}

		updateVelocity(deltaTime);

		hurtBox.update();
		setPosition(position + (velocity * deltaTime.asSeconds()));
		hurtBox.setCenter(getPosition());

	};

	virtual void updateAnimation(sf::Time deltaTime) { sprite.update(deltaTime); };

	//controls projectile's velocity, called every update
	virtual void updateVelocity(sf::Time deltaTime) {};

	//does this thing collide with level
	bool collisionActive = false;

	//hurtbox
	Hurtbox hurtBox;

	void kill() {
		GameObject::kill();
	}

	bool canRemoveOnContact() {
		return removeOnContact;
	}
	
protected:
	
	sf::Time maxLifeTime;
	bool removeIfOutOfLevel = true;
	bool removeIfOffScreen = false;
	bool removeOnHit = false;

	bool removeOnContact = false;
	//gameobject has lifeTime var

	bool subweaponCheckHit(GameObject *obj) {
		bool hitEnemy = obj->isEnemy();
		bool hitNeutral = obj->isNeutral() && obj->isAttackable() && !obj->isPlayerAttackableOnly();
		return hitEnemy || hitNeutral;
		
	}

};


#endif