#include "Director.hpp"

Director::Director(Zone *z, ObjectManager *o, GameCamera *c) {
	zone = z;
	objMan = o;
	cam = c;
	waitTimer = sf::Time::Zero;
}

void Director::update(sf::Time deltaTime, bool gamePaused) {
	if (waitTimer > sf::Time::Zero)
		waitTimer -= deltaTime;
	else {

		if (!initialized) {
			initialize();
			initialized = true;
		}

		updateTracker();
		act(deltaTime);
	}
}

bool Director::isComplete() {
	return complete; 
}

void Director::trackObject(sf::String levelName, sf::String objName) {
	if (!hasObject(objName)) {

		ScriptedObject object;
		object.onLevel = levelName;
		if (levelName == zone->getCurrentLevelName()) {
			object.obj = objMan->getObjectByName(objName);
		}
		else {
			object.obj = nullptr;
		}

		objTracker.insert(std::pair<sf::String, ScriptedObject>(objName, object));
	}
};

bool Director::hasObject(sf::String objName) {
	std::map<sf::String, ScriptedObject>::iterator find = objTracker.find(objName);

	return (find != objTracker.end()) &&
		find->second.obj != nullptr &&
		zone->getCurrentLevelName() == find->second.onLevel;

};

template <class T>
T* Director::getObject(sf::String objName) {
	return (T*)(objTracker.at(objName).obj);
};

void Director::wait(sf::Time wait) {
	waitTimer = wait;
}

void Director::updateTracker() {
	for (std::map<sf::String, ScriptedObject>::iterator it = objTracker.begin(); it != objTracker.end(); it++) {
		if (zone->getCurrentLevelName() == it->second.onLevel) {
			it->second.obj = objMan->getObjectByName(it->first);
		}
		else {
			it->second.obj = nullptr;
		}
	}
}