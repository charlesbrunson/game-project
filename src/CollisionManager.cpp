#include "CollisionManager.hpp"


CollisionManager::CollisionManager(Level* lvl) {
	setWorldCollision(lvl);
}

void CollisionManager::update(sf::Time delta) {

}

void CollisionManager::setWorldCollision(Level* lvl) {

}

Static* CollisionManager::createStatic(sf::FloatRect area, Static::CollisionType colType) {
	statics.push_back(Static());
	statics.back().setArea(area);
	statics.back().resetPrevArea();
	statics.back().type = colType;
	return &statics.back();
}

Solid* CollisionManager::createSolid(sf::FloatRect area) {
	solids.push_back(Solid());
	solids.back().setArea(area);
	solids.back().resetPrevArea();
	return &solids.back();
}

Static* CollisionManager::removeStatic(Static* statObj) {
	for (auto i = statics.begin(); i != statics.end(); i++) {
		if (i._Ptr == statObj) {
			statics.erase(i);
			//successful
			return nullptr;
		}
	}
	return statObj;
}

Solid* CollisionManager::removeSolid(Solid* solid) {
	for (auto i = solids.begin(); i != solids.end(); i++) {
		if (i._Ptr == solid) {
			solids.erase(i);
			//successful
			return nullptr;
		}
	}
	return solid;
}

void CollisionManager::collide() {

}

void CollisionManager::validateCollisions() {

}

void CollisionManager::resolveCollisions() {

}
