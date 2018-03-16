#include "obj/Hurtbox.hpp"
#include "game/AlertSystem.hpp"

void Hurtbox::update() {
	if (isActive) {
		updateIgnoreList();
	}
};

void Hurtbox::setCenter(sf::Vector2f pos) {
	top = pos.y - (height / 2);
	left = pos.x - (width / 2);
};

void Hurtbox::clearIgnoreList() {
	ignoreObjs.clear();
}

bool Hurtbox::tryHurt(GameObject *obj) {
	if (isActive) {
		//update ignore
		updateIgnoreList();

		for (std::vector<GameObject*>::const_iterator it = ignoreObjs.begin(); it != ignoreObjs.end(); it++) {
			if (obj == (*it)) {
				return false;
			}
		}


		// do hurt
		bool attempt = obj->hurt(damage, stunTime);
		if (attempt) {
			ignoreObjs.push_back(obj);
			return true;
		}
		else {
			return false;
		}
	}
	else {

		ignoreObjs.clear();

	}
	return false;
};

void Hurtbox::updateIgnoreList() {
	if (!ignoreObjs.empty()) {
		for (std::vector<GameObject*>::iterator it = ignoreObjs.begin(); it != ignoreObjs.end();) {
			if (*it == nullptr || (*it)->toDelete || !intersects((*it)->getCollision())) {
				it = ignoreObjs.erase(it);
			}
			else {
				it++;
			}
		}
	}
};

void Hurtbox::setActive(bool i) {
	isActive = i;
};

bool Hurtbox::getActive() const {
	return isActive;
};
