#ifndef OBJECTSPAWNERCONFIGURATION_H
#define OBJECTSPAWNERCONFIGURATION_H

#include "Node.h"

enum PositionStrategy : int {
	EDGE_LEFT,
	EDGE_RIGHT,

	//refers to horizontal center of area
	CENTER
};

struct SpawnConfiguration {
	//object data to copy from when creating new objs
	objNode objPrototype;

	//how long to wait after spawning obj before spawning another
	sf::Time spawnInterval = sf::Time::Zero;

	//how long to wait after object's death before respawning it
	sf::Time deathCooldown = sf::Time::Zero;

	//y distance to place obj from area's bottom border
	float heightOffset = 0;

	//y distance is offset from players y coordinate
	bool offsetFromPlayer = false;

	//max number of objects to have active at once
	unsigned int maxObjects;

	//determine x positioning
	int spawnPosStrategy = EDGE_RIGHT;

	sf::FloatRect triggerArea;
};

#endif