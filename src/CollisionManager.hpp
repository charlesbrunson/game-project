#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "memory"

//#include <SFML/System.hpp>

#include "Level.hpp"
#include "Collision.hpp"
//#include "GameObject.hpp"

class CollisionManager {
public:

	//get level collision map from level
	CollisionManager(Level* lvl);

	//update collision for all objects
	void update(sf::Time delta);

	//generates world collision
	void setWorldCollision(Level* lvl);

	//add physics object to pool
	//void addObject(const GameObject* obj);

	inline const std::vector<Static>& getStatics() { return statics; };

private:
	std::vector<Static> statics;
	std::map<GridVector, Static*> staticsByGrid;
	std::vector<Solid*> solids;

	//TODO
	void doLevelCollision();
	void doCollidableCollision();
	void validateCollisions();
	void resolveCollisions();
};

#endif
