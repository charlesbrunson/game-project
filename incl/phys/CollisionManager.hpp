#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "memory"

//#include <SFML/System.hpp>

#include "game/lvl/Level.hpp"
#include "phys/Collision.hpp"
//#include "GameObject.hpp"

class CollisionManager {
public:

	//get level collision map from level
	CollisionManager();
	CollisionManager(sf::Vector2i staticSize);
	CollisionManager(Level* lvl);

	//update collision for all objects
	void update(sf::Time delta);

	//generates world collision
	void setWorldCollision(Level* lvl);

	//add physics object to pool
	//void addObject(const GameObject* obj);
	Static* createStatic(sf::FloatRect area, Static::CollisionType colType);
	Solid* createSolid(sf::FloatRect area);

	Static* removeStatic(Static* statObj);
	Solid* removeSolid(Solid* solid);

	inline const std::vector<Static>& getStatics() { return statics; };

private:
	std::vector<Static> statics;
	std::map<GridVector, std::vector<Static*>> staticsByGrid;
	std::map<Static::CollisionType, std::vector<Static*>> staticsByType;
	std::vector<Solid> solids;

	//TODO
	void collide(Solid* solid);
	void collideWithStatic(Solid* solid, std::vector<Collision> *collisions);
	void validateCollisions(std::vector<Collision> *collisions);
	void resolveCollisions(Solid* solid, std::vector<Collision> *collisions);
};

#endif
