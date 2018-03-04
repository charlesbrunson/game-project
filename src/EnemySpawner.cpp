#include "EnemySpawner.hpp"
#include "Enemy.hpp"
#include "CamRef.hpp"

#include "NodeBuilder.hpp"

void EnemySpawner::update(sf::Time deltaTime) {
	trackSpawnedObjects(deltaTime);

	if (spawnTimer > sf::Time::Zero)
		spawnTimer -= deltaTime;

	// Attempt to spawn
	if (spawnTimer <= sf::Time::Zero) {
		for (std::vector<SpawnedObjTracker>::iterator it = spawnedObjs.begin(); it != spawnedObjs.end(); it++) {
			if (it->deathTimer <= sf::Time::Zero && it->obj == nullptr && getCameraArea().intersects(area)) {
				// Try to spawn on this slot

				bool spawned = false;
				if (preferSide == SPAWN_LEFT) {
					if (trySpawn(&it, SPAWN_LEFT) || trySpawn(&it, SPAWN_RIGHT)) {
						spawned = true;
					}
				}
				else {
					if (trySpawn(&it, SPAWN_RIGHT) || trySpawn(&it, SPAWN_LEFT)) {
						spawned = true;
					}
				}

				if (spawned) {
					spawnTimer = spawnRate;
					break;
				}
			}
		}
	}
}

void EnemySpawner::trackSpawnedObjects(sf::Time deltaTime) {
	for (std::vector<SpawnedObjTracker>::iterator objIt = spawnedObjs.begin(); objIt != spawnedObjs.end(); objIt++) {

		if (objIt->obj != nullptr && objIt->obj->toDelete) {
			objIt->obj = nullptr;
			objIt->deathTimer = deathCooldown;
		}
		else if (objIt->obj == nullptr && objIt->deathTimer > sf::Time::Zero) {
			objIt->deathTimer -= deltaTime;
			if (objIt->deathTimer < sf::Time::Zero)
				objIt->deathTimer = sf::Time::Zero;
		}
		else if (objIt->obj != nullptr) {
			if (!area.intersects(getCameraArea()) && !objIt->obj->getCollision().intersects(getCameraArea())) {
				objIt->obj->toDelete = true;
				objIt->obj = nullptr;
				objIt->deathTimer = deathCooldown;
			}
		}

	}
}

bool EnemySpawner::trySpawn(std::vector<SpawnedObjTracker>::iterator *it, int e) {
	
	sf::Vector2f pos;
	sf::FloatRect camArea = getCameraArea();

	pos.x = camArea.left + (e == SPAWN_LEFT ? -(getTemplateNode().width / 2) : camArea.width + (getTemplateNode().width / 2));
	pos.y = area.top + area.height - spawnHeight;

	if (pos.x >= area.left && pos.x <= area.left + area.width) {
		if (pos.y >= camArea.top && pos.y <= camArea.top + camArea.height) {
			spawnAt(it, pos, e);
			return true;
		}
	}

	return false;
}

void EnemySpawner::spawnAt(std::vector<SpawnedObjTracker>::iterator *it, sf::Vector2f pos, int side) {

	(*it)->obj = gBuildObject(getTemplateNode(), lvl, false);
	
	(*it)->obj->setPosition(pos);
	(*it)->obj->getAnimSprite().setHFlip(side == SPAWN_RIGHT ? true : false);

	if ((*it)->obj->isEnemy() && !(*it)->obj->isProjectile()) {
		Enemy *enemyObj = (Enemy*)(*it)->obj;
		enemyObj->setSpawn();
		enemyObj->removeOnDespawn = true;
		enemyObj->respawning = false;
	}
	createObject((*it)->obj);
}

void EnemySpawner::readObjArgs() {
	setTemplateName(findTemplateName(getObjectNode()));
	
	for (std::pair<std::string, std::string> arg : getObjectNode().args) {
		if (arg.first == "PREFERSIDE") {
			if (arg.second == "LEFT") {
				preferSide = SPAWN_LEFT;
			}
			else if (arg.second == "RIGHT") {
				preferSide = SPAWN_RIGHT;
			}
		}
		else if (arg.first == "SPAWNRATE") {
			spawnRate = sf::milliseconds(std::stoi((std::string)arg.second));
		}
		else if (arg.first == "DEATHCOOLDOWN") {
			deathCooldown = sf::milliseconds(std::stoi((std::string)arg.second));
		}
		else if (arg.first == "SPAWNHEIGHT") {
			spawnHeight = std::stoi((std::string)arg.second);
		}
		else if (arg.first == "MAXOBJECTS") {
			maxObjects = std::stoi((std::string)arg.second);
			spawnedObjs.clear();
			for (int i = 0; i < maxObjects; i++) {
				spawnedObjs.push_back(SpawnedObjTracker());
			}
		}
	}
}
