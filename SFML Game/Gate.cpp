#include "Gate.hpp"
#include "Player.hpp"
#include "PositionRef.hpp"

void Gate::setHeight(int h) {
	segmentCount = (int)ceil((float)h / (float)Level::spacing) - 1;


	setCollisionSize(Level::spacing, h);
	setCollisionPosition(position);

	gateEnd.setTexture(sprite.getResources()->getTexture(Resources::MECHANISMS));
	gateEnd.setTextureRect(sf::IntRect(0, Level::spacing, Level::spacing, Level::spacing));

	gateEnd.setPosition(getPosition() + sf::Vector2f(-5.f, -Level::spacing));

	gateSegments.clear();

	for (int i = 1; i <= segmentCount; i++) {
		sf::Sprite gateSegment;
		gateSegment.setTexture(sprite.getResources()->getTexture(Resources::MECHANISMS));
		gateSegment.setTextureRect(sf::IntRect(0, 0, Level::spacing, Level::spacing));

		gateSegment.setPosition(getPosition() + sf::Vector2f(-5, -Level::spacing - (i * Level::spacing)));

		gateSegments.push_back(gateSegment);
	}

	moveGateSegments(currentState == OPEN);

};


void Gate::readObjArgs() {

	for (std::pair<sf::String, sf::String> arg : getObjectNode().args) {
		if (arg.first == "STATE") {
			if (arg.second == "OPEN") {
				currentState = OPEN;
			}
			else {
				currentState = CLOSED;
			}
		}
		else if (arg.first == "TRANSITION") {
			if (arg.second == "LEFT") {
				transitionOnTouch = LEFT;
			}
			else if (arg.second == "RIGHT") {
				transitionOnTouch = RIGHT;
			}
		}
	}


	createColliders();
};

void Gate::setOpen(bool open) {
	bool changedState = false;
	if (open && currentState == CLOSED) {
		currentState = OPEN;
		changedState = true;
	}
	else if (!open && currentState == OPEN) {
		currentState = CLOSED;
		changedState = true;
	}

	if (changedState) {
		if (animTimer >= gateMoveTime)
			animTimer = sf::Time::Zero;
		else
			animTimer = gateMoveTime - animTimer;

		for (unsigned int i = 0; i < getObjectNode().args.size(); i++) {
			if (getObjectNode().args.at(i).first == "STATE") {
				if (currentState == CLOSED)
					getObjectNode().args.at(i).second = "CLOSED";
				else
					getObjectNode().args.at(i).second = "OPEN";
			}
		}


	}

};

void Gate::update(sf::Time deltaTime) {

	if (!hasColliders) {
		createColliders();

	}

	if (transitionOnTouch != NONE && gPlayer != nullptr) {
		GameObject *p = gPlayer;
		
		if (p->getGrounded() && p->getPosition().y == position.y && ((Player*)p)->getState() == Player::states::DEFAULT) {
			
			/*
			if (transitionOnTouch == transDir::LEFT && p->getCollision().left <= collisionBox.left + collisionBox.width) {
				UtilTransitionController *c = new UtilTransitionController(p, -1, resLoader, lvl);
				setOpen(true);
				transitionOnTouch = transDir::NONE;
				createObject(c);
			}
			else if (transitionOnTouch == transDir::RIGHT && p->getCollision().left + p->getCollision().width >= collisionBox.left) {
				UtilTransitionController *c = new UtilTransitionController(p, 1, resLoader, lvl);
				setOpen(true);
				transitionOnTouch = transDir::NONE;
				createObject(c);
			}
			*/
		}
	}

	collidableActive = currentState != OPEN;
	if (animTimer <= gateMoveTime) {
		//move segments
		moveGateSegments(currentState == OPEN);
		animTimer += deltaTime;
	}
	else if (currentState != OPEN) {
		gateEnd.setPosition(getPosition() + sf::Vector2f(-5.f, -Level::spacing));
		int i = 1;
		for (std::vector<sf::Sprite>::iterator it = gateSegments.begin(); it != gateSegments.end(); it++) {
			it->setPosition(getPosition() + sf::Vector2f(-5, -Level::spacing - (i * Level::spacing)));
			i++;
		}
	}
};

void Gate::moveGateSegments(bool opening) {

	float fract = (animTimer / gateMoveTime);
	if (!opening)
		fract = 1 - fract;

	float moveDist = (segmentCount + 1) * Level::spacing;
	float toMove = moveDist * fract;

	gateEnd.setPosition(getPosition() + sf::Vector2f(-5.f, std::min(-Level::spacing - toMove, moveDist)));
	int i = 1;
	for (std::vector<sf::Sprite>::iterator it = gateSegments.begin(); it != gateSegments.end(); it++) {
		it->setPosition(getPosition() + sf::Vector2f(-5, std::min(-Level::spacing - (i * Level::spacing) - toMove, moveDist)));
		i++;
	}
}

void Gate::createColliders() {
	sf::FloatRect col;
	col.left = position.x - (Level::spacing / 2);
	col.top = position.y - (segmentCount * Level::spacing);

	col.width = Level::spacing;
	col.height = (segmentCount + 1) * Level::spacing;

	sf::Vector2f offset(-(Level::spacing / 2), -((segmentCount + 1) * Level::spacing));

	Collidable::Collider collider;
	collider.box = col;
	collider.offset = offset;

	addCollider(collider, Collidable::LEFT);
	addCollider(collider, Collidable::RIGHT);
	hasColliders = true;
};