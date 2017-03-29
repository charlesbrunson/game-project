#ifndef ENEMYSPAWNER_H
#define ENEMYSPAWNER_H

//#include "PositionRef.hpp"

#include "GameObject.hpp"
#include "TemplateUser.hpp"




class EnemySpawner : public GameObject, public TemplateUser {
public:
	EnemySpawner(ResourceLoader *rloader, Level *level, int ID = -1) : GameObject(rloader, level, ID) {
		attackable = false;
		doNotDraw = true;
		drawPriority = -1;
	};

	struct SpawnedObjTracker {
		sf::Time deathTimer = sf::Time::Zero;
		GameObject* obj = nullptr;
	};

	void update(sf::Time deltaTime);

	void setArea(sf::FloatRect a) {
		area = a;
	}
	const sf::FloatRect getArea() {
		return area;
	}

	void trackSpawnedObjects(sf::Time deltaTime);

	bool trySpawn(std::vector<SpawnedObjTracker>::iterator *it, int e);

	void spawnAt(std::vector<SpawnedObjTracker>::iterator *it, sf::Vector2f pos, int side);

	virtual bool isTemplateUser() {
		return true;
	}

	void readObjArgs();

	void* getTemplateUser() {
		return (TemplateUser*)this;
	};

protected:
	sf::Time spawnRate;
	sf::Time spawnTimer;
	sf::Time deathCooldown;

	int maxObjects = 0;

	std::vector<SpawnedObjTracker> spawnedObjs;

	sf::FloatRect area;

	int preferSide = SPAWN_LEFT;
	int spawnHeight = 0;

	enum spawnSide {
		SPAWN_LEFT,
		SPAWN_RIGHT
	};

};

#endif