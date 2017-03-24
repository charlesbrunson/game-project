#include "Collidable.hpp"

void Collidable::moveCollidingObj(GameObject *collidedObj, int dir, const sf::Time deltaTime) {

	GameObject *obj = collidedObj;

	sf::Vector2f deltaVel = collidableParent->getVelocity() * deltaTime.asSeconds();
	sf::Vector2f velocity = collidableParent->getVelocity();
	sf::Vector2f position = collidableParent->getPosition();

	Level *lvl = obj->getLevel();
	bool hcol = false, vcol = false;

	auto testCollision = [](sf::FloatRect &objCol, sf::FloatRect &lvlCol, bool &flag, float offset, bool hori) {
		if (lvlCol.intersects(objCol)) {
			flag = true;
			if (hori)
				objCol.left = lvlCol.left + offset;
			else
				objCol.top = lvlCol.top + offset;
			return true;
		}
		return false;
	};

	// Object is on top
	if (dir == UP) {
		
		obj->setVelocity(obj->getVelocity() + sf::Vector2f(0.f, velocity.y));

		// Moving horizontally, prevent rider object from clipping into level collision
		if (deltaVel.x != 0) {

			sf::FloatRect objCol = obj->getCollision();
			objCol.left += deltaVel.x;

			// Moving right
			if (deltaVel.x > 0) {
				// Test leftside collisions
				for (auto i = lvl->getLeftCol()->begin(); i != lvl->getLeftCol()->end(); i++) {

					hcol |= testCollision(objCol, *i, obj->collisionLeft, -objCol.width, true);

					/*
					if (i->intersects(test)) {
						obj->collisionLeft = true;
						test.left = i->left - test.width;
						hcol = true;
					}
					*/
				}
			}
			// Moving left
			else if (deltaVel.x < 0) {
				// Test rightside collisions
				for (auto i = lvl->getRightCol()->begin(); i != lvl->getRightCol()->end(); i++) {

					hcol |= testCollision(objCol, *i, obj->collisionRight, i->width, true);

					/*
					if (i->intersects(test)) {
						obj->collisionRight = true;
						test.left = i->left + i->width;
						hcol = true;
					}
					*/
				}
			}
			if (hcol) {
				// If there is a collision, stop object from moving with collidable
				obj->setVelocity(sf::Vector2f(0.f, obj->getVelocity().y));
			}
			// Use results to update object position
			obj->setPosition(obj->getPosition() + sf::Vector2f(objCol.left - obj->getCollision().left, 0.f), false);
		}
		// Going up
		if (deltaVel.y < 0) {
			sf::FloatRect objCol = obj->getCollision();
			objCol.top += deltaVel.y;

			for (auto i = lvl->getDownCol()->begin(); i != lvl->getDownCol()->end(); i++) {

				vcol |= testCollision(objCol, *i, obj->collisionDown, i->height, false);
				/*
				if (i->intersects(objCol)) {
					obj->collisionDown = true;
					objCol.top = i->top + i->height;
					vcol = true;
				}
				*/
			}
			if (vcol) {
				obj->setVelocity(sf::Vector2f(obj->getVelocity().x, 0.f));
				obj->setPosition(obj->getPosition() + sf::Vector2f(0.f, objCol.top - obj->getCollision().top), false);
			}
		}
		
		// Solve problem that only occurs when pixel snapping is enabled where the object and 
		// Collidable moves at the same rate but different offsets, which makes it less like the object is moving with the collidable
		if (obj->getVelocity().x == 0.f && velocity.x != 0.f && !hcol && pixelSnapping) {
			float objDeci = obj->getPosition().x - floor(obj->getPosition().x);
			float colliderDeci = position.x - floor(position.x);

			if (objDeci != colliderDeci)
				obj->setPosition(sf::Vector2f(floor(obj->getPosition().x) + colliderDeci, obj->getPosition().y), false);

		}
	}
	else {
		// On right or on left
		if ((dir == RIGHT && velocity.x > 0) || (dir == LEFT && velocity.x < 0)) {
			//obj->setPosition(sf::Vector2f(obj->getPosition().x + deltaVel.x, obj->getPosition().y), false);
			obj->setVelocity(sf::Vector2f(obj->getVelocity().x + velocity.x, obj->getVelocity().y));

			if (dir == RIGHT) {

				hcol = false;
				sf::FloatRect objCol = obj->getCollision();
				objCol.left += deltaVel.x;

				for (auto i = lvl->getLeftCol()->begin(); i != lvl->getLeftCol()->end(); i++) {

					hcol |= testCollision(objCol, *i, obj->collisionLeft, -objCol.width, true);
					/*
					if (i->intersects(test)) {
						obj->collisionLeft = true;
						test.left = i->left - test.width;
						hcol = true;
					}
					*/
				}
				if (hcol) {
					obj->setVelocity(sf::Vector2f(0.f, obj->getVelocity().y));
				}
				obj->setPosition(obj->getPosition() + sf::Vector2f(objCol.left - obj->getCollision().left, 0.f), true);


			}
			else {
				hcol = false;
				sf::FloatRect objCol = obj->getCollision();
				objCol.left += deltaVel.x;

				for (auto i = lvl->getRightCol()->begin(); i != lvl->getRightCol()->end(); i++) {

					hcol |= testCollision(objCol, *i, obj->collisionRight, i->width, true);
					/*
					if (i->intersects(test)) {
						obj->collisionRight = true;
						test.left = i->left + i->width;
						hcol = true;
					}
					*/
				}
				if (hcol) {
					obj->setVelocity(sf::Vector2f(0.f, obj->getVelocity().y));
				}
				obj->setPosition(obj->getPosition() + sf::Vector2f(objCol.left - obj->getCollision().left, 0.f), true);

			}

		}
		// Beneath
		else if (dir == DOWN && velocity.y > 0) {
			//obj->setPosition(sf::Vector2f(obj->getPosition().x, obj->getPosition().y + deltaVel.y), false);
			obj->setVelocity(sf::Vector2f(obj->getVelocity().x, obj->getVelocity().y + velocity.y));

			vcol = false;
			sf::FloatRect objCol = obj->getCollision();
			objCol.left += deltaVel.x;

			for (auto i = lvl->getRightCol()->begin(); i != lvl->getRightCol()->end(); i++) {
				vcol |= testCollision(objCol, *i, obj->collisionDown, i->height, false);
				/*
				if (i->intersects(test)) {
					obj->collisionDown = true;
					test.top = i->top + i->height;
					vcol = true;
				}
				*/
			}
			if (vcol) {
				obj->setVelocity(sf::Vector2f(obj->getVelocity().x, 0.f));
			}
			obj->setPosition(obj->getPosition() + sf::Vector2f(0.f, objCol.top - obj->getCollision().top), false);


		}
	}
};

bool Collidable::isColliding(GameObject* obj) {
	std::vector<Collider>::const_iterator it;
	for (it = upColliders.begin(); it != upColliders.end(); it++) {
		if (it->box.intersects(obj->getCollision())) {
			return true;
		}
	}
	for (it = rightColliders.begin(); it != rightColliders.end(); it++) {
		if (it->box.intersects(obj->getCollision())) {
			return true;
		}
	}
	for (it = downColliders.begin(); it != downColliders.end(); it++) {
		if (it->box.intersects(obj->getCollision())) {
			return true;
		}
	}
	for (it = leftColliders.begin(); it != leftColliders.end(); it++) {
		if (it->box.intersects(obj->getCollision())) {
			return true;
		}
	}
	return false;
}

void Collidable::updateCollider(sf::Time deltaTime) {

	sf::Vector2f pos;
	sf::FloatRect nBoundingBox;
	bool initBoundingBox = true;
	if (collidableParent != nullptr) {
		pos = collidableParent->getPosition();
	}
	
	auto updateColliderPositions = [&pos, &nBoundingBox, &initBoundingBox](std::vector<Collider> *col) {
		for (std::vector<Collider>::iterator it = col->begin(); it != col->end(); it++) {
			it->box.left = it->offset.x + pos.x;
			it->box.top = it->offset.y + pos.y;

			if (initBoundingBox) {
				nBoundingBox = it->box;
				initBoundingBox = false;
			}
			else {
				sf::FloatRect b = nBoundingBox;
				b.left = std::min(nBoundingBox.left, it->box.left);
				b.top = std::min(nBoundingBox.top, it->box.top);
				b.width = std::max(nBoundingBox.left + nBoundingBox.width, it->box.left + it->box.width) - b.left;
				b.height = std::max(nBoundingBox.top + nBoundingBox.height, it->box.top + it->box.height) - b.top;
			}
		}
	};

	updateColliderPositions(&upColliders);
	updateColliderPositions(&rightColliders);
	updateColliderPositions(&downColliders);
	updateColliderPositions(&leftColliders);

	boundingBox = nBoundingBox;

	/*
	if (!upColliders.empty()) {
		for (std::vector<Collider>::iterator it = upColliders.begin(); it != upColliders.end(); it++) {
			it->box.left = it->offset.x + pos.x;
			it->box.top = it->offset.y + pos.y;
		}
	}
	if (!rightColliders.empty()) {
		for (std::vector<Collider>::iterator it = rightColliders.begin(); it != rightColliders.end(); it++) {
			it->box.left = it->offset.x + pos.x;
			it->box.top = it->offset.y + pos.y;
		}
	}
	if (!downColliders.empty()) {
		for (std::vector<Collider>::iterator it = downColliders.begin(); it != downColliders.end(); it++) {
			it->box.left = it->offset.x + pos.x;
			it->box.top = it->offset.y + pos.y;
		}
	}
	if (!leftColliders.empty()) {
		for (std::vector<Collider>::iterator it = leftColliders.begin(); it != leftColliders.end(); it++) {
			it->box.left = it->offset.x + pos.x;
			it->box.top = it->offset.y + pos.y;
		}
	}

	if (collidableParent != nullptr && collidableActive) {
		while (!collidingObjs.empty()) {
			moveCollidingObj(collidingObjs.back().first, collidingObjs.back().second, deltaTime);
			collidingObjs.pop_back();
		}
	}
	*/
};

bool Collidable::canCollide(int dir) {
	if (dir == UP)
		return !upColliders.empty();
	else if (dir == RIGHT)
		return !rightColliders.empty();
	else if (dir == DOWN)
		return !downColliders.empty();
	else if (dir == LEFT)
		return !leftColliders.empty();
	else
		return false;
};

bool Collidable::addCollider(sf::FloatRect rect, sf::Vector2f off, int dir) {
	Collider c;
	c.box = rect;
	c.offset = off;
	return addCollider(c, dir);
};

bool Collidable::addCollider(Collider col, int dir) {
	std::vector<Collider>* targetVector = nullptr;

	if (dir == UP)
		targetVector = &upColliders;
	else if (dir == RIGHT)
		targetVector = &rightColliders;
	else if (dir == DOWN)
		targetVector = &downColliders;
	else if (dir == LEFT)
		targetVector = &leftColliders;

	if (targetVector != nullptr) {

		for (std::vector<Collider>::const_iterator comp = targetVector->begin(); comp != targetVector->end(); comp++) {
			if (comp->box == col.box && comp->offset == col.offset) {
				return true;
			}
		}
		targetVector->push_back(col);

		return true;
	}
	else {
		return false;
	}
};

std::vector<Collidable::Collider>* Collidable::getColliders(int dir) {
	if (dir == UP)
		return &upColliders;
	else if (dir == RIGHT)
		return &rightColliders;
	else if (dir == DOWN)
		return &downColliders;
	else if (dir == LEFT)
		return &leftColliders;
	else
		return nullptr;
};


const sf::FloatRect Collidable::getBoundingBox() {
	return boundingBox;
}