
#ifndef NODEBUILDER_H
#define NODEBUILDER_H

#include "GameObject.hpp"
#include "ObjectTypes.hpp"
#include "Level.hpp"
#include "Node.hpp"

#include <assert.h>

//builds an object from an objnode
static GameObject* gBuildObject(objNode objN, Level* lvl, bool giveNode = true) {
	
	GameObject* obj = nullptr;
	if (objN.type == "PLAYER") {
		obj = new Player(lvl);
		obj->setPosition(sf::Vector2f(objN.x, objN.y));
		obj->getAnimSprite().setHFlip(objN.faceLeft);
	}
	else if (objN.type == "TESTOBJECT") {
		obj = new TestObject(lvl);
		obj->setPosition(sf::Vector2f(objN.x, objN.y));
		obj->getAnimSprite().setHFlip(objN.faceLeft);
	}
	else if (objN.type == "ENEMYSPAWNER") {
		obj = new EnemySpawner(lvl);

		sf::FloatRect area;
		area.left = objN.x - (objN.width / 2);
		area.top = objN.y - objN.height;
		area.width = objN.width;
		area.height = objN.height;

		((EnemySpawner*)obj)->setArea(area);
	}
	if (obj != nullptr) {
		if (giveNode)
			obj->setObjectNode(objN);

		obj->name = objN.name;
	}
	else {
		Log::msg("Build failed: " + objN.name + " - " + objN.type + "\n");
		assert(obj != nullptr);
	}
	return obj;
}

#endif