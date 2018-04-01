#include "obj/GroundEnemy.hpp"


void GroundEnemy::setSpawn() {
	if (!collisionActive)
		snapToSurface();
	Enemy::setSpawn();
};

bool GroundEnemy::checkEdge(bool LeftElseRight) {
	//COLFIX
	/*
	if (LeftElseRight) {
		//check left side
		if (!checkedLeft) {
			for (auto it = lvl->getUpCol()->begin(); it != lvl->getUpCol()->end(); it++) {
				if (it->top == surfaceHeight && it->left + it->width == surfaceLeft) {
					surfaceLeft -= (int)(it->width);
					checkedLeft = false;
					return true;
				}
			}
			checkedLeft = true;
		}
	}
	else {
		//check right side
		if (!checkedRight) {
			for (auto it = lvl->getUpCol()->begin(); it != lvl->getUpCol()->end(); it++) {
				if (it->top == surfaceHeight && it->left == surfaceRight) {
					surfaceRight += (int)(it->width);
					checkedRight = false;
					return true;
				}
			}
			checkedRight = true;
		}
	}
	*/
	return false;

};

bool GroundEnemy::snapToSurface() {

	//COLFIX
	//find closest surface directly below us
	/*
	sf::FloatRect e;
	bool pickedOne = false;
	for (auto it = lvl->getUpCol()->begin(); it != lvl->getUpCol()->end(); it++) {
		//check we're between its width
		if (it->top >= collisionBox.top + collisionBox.height) {
			//check we're between its width
			if (collisionBox.left + collisionBox.width > it->left && collisionBox.left < it->left + it->width) {
				//check if it's closer than currently held one, if there is one
				if (!pickedOne) {
					e = *it;
					pickedOne = true;
				}
				else if (e.top > it->top) {
					//swap
					e = *it;
				}
			}
		}
	}
	if (!pickedOne) {
		//if there isn't any ground below us return false
		return false;
	}
	//found it
	else {
		//offset y position so be on that surface
		int diffY = static_cast<int>(e.top - (collisionBox.top + collisionBox.height));

		//offset x position if we're hanging off edge
		int diffX = 0;
		if (collisionBox.left < e.left) {
			diffX = static_cast<int>(e.left - collisionBox.left);
		}
		else if (collisionBox.left + collisionBox.width > e.left + e.width) {
			diffX = static_cast<int>((e.left + e.width) - (collisionBox.left + collisionBox.width));
		}

		setPosition(sf::Vector2f(position.x + diffX, position.y + diffY));

		//update surface values
		surfaceLeft = (int)(e.left);
		surfaceRight = (int)(e.left + e.width);
		surfaceHeight = (int)(e.top);

		setGrounded(true);

		return true;
	}
	
	*/
	return false;
};
