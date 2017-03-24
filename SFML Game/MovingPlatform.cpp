#include "MovingPlatform.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

bool MovingPlatform::setType(int t) {
	platformType = t;

	sf::Vector2f size(0.f, 0.f);
	int targetSpr = Resources::imageNames::PLATFORM;

	bool complete = false;

	if (t == platformTypes::DEFAULT) {
		size = sf::Vector2f(30.f, 10.f);
		targetSpr = Resources::imageNames::PLATFORM;
		complete = true;
	}

	setCollisionSize(size.x, size.y);

	createColliders();

	//set sprite
	sprite.setResources(getResources());
	sprite.getSprite()->setTexture(sprite.getResources()->getTexture(targetSpr));

	anim = AnimSprite::create(sf::IntRect(0, 0, (int)size.x, (int)size.y), sf::Vector2f((int)(size.x / 2), (int)(size.y)), 1, sf::seconds(1.f));
	sprite.setAnimation(anim);
	sprite.updateFrame();

	return complete;
};

void MovingPlatform::update(sf::Time deltaTime) {
	
	if (!targetInstantiated && hasPath()) {
		try {
			targetPoint = incrementPath();
			targetInstantiated = true;
			setPosition(getPreviousPoint());
			setVelocity(sf::Vector2f());

			waitTimer = waitAtPoint;

			sf::Vector2f diff = targetPoint - position;
			travelTimer = sf::Time::Zero;
			if (platformSpeed > 0)
				travelDuration = sf::seconds(sqrt(diff.x * diff.x + diff.y * diff.y) / platformSpeed);
		}
		catch (const char* err) {
			Log(*err);
			targetInstantiated = false;
		}
	}

	velocity = sf::Vector2f();
	if (waitTimer > deltaTime) {
		waitTimer -= deltaTime;
	}
	else if (waitTimer > sf::Time::Zero && waitTimer <= deltaTime) {
		sf::Time newTime = waitTimer;
		waitTimer = sf::Time::Zero;
		move(newTime);
	}
	else {
		move(deltaTime);
	}

	setPosition(getPosition() + velocity * deltaTime.asSeconds());
};


void MovingPlatform::move(sf::Time deltaTime) {

	if (!hasPath()) {
		setPosition(position);
		return;
	}

	sf::Vector2f oldPos = position;
	sf::Time remainingTime = sf::Time::Zero;

	//returns velocity
	sf::Vector2f diff;
	if (pathingPoints.size() > 0)
		diff = targetPoint - getPreviousPoint();

	if (travelTimer + deltaTime >= travelDuration) {
		remainingTime = travelTimer + deltaTime - travelDuration;
		travelTimer = travelDuration;
	}
	else {
		travelTimer += deltaTime;
	}

	sf::Vector2f newPos = getPreviousPoint();
	if (travelDuration > sf::Time::Zero) {
		newPos += diff * (travelTimer / travelDuration);
	}

	bool updateVelPos = true;

	if (teleportOnLoop && pointCounter == pathingPoints.size() - 1) {
		setPosition(targetPoint);
		setVelocity(sf::Vector2f(0.f, 0.f));
		updateVelPos = false;
	}

	if (updateVelPos) {
		setVelocity((newPos - oldPos) / deltaTime.asSeconds());
		//setPosition(newPos);
	}

	if (travelTimer >= travelDuration && hasPath()) {
		setVelocity((targetPoint - oldPos) / deltaTime.asSeconds());
		//setPosition(targetPoint);
		targetPoint = incrementPath();
		if (waitAtPoint > sf::Time::Zero) {
			waitTimer = waitAtPoint - remainingTime;
		}
		else if (remainingTime > sf::Time::Zero) {
			move(remainingTime);
		}
	}
};

void MovingPlatform::appendPathPoint(sf::Vector2f point) {
	pathingPoints.push_back(point);
};

bool MovingPlatform::insertPathPoint(sf::Vector2f point, int insert) {

	int count = 0;
	for (std::vector<sf::Vector2f>::iterator i = pathingPoints.begin(); i < pathingPoints.end(); i++) {
		if (count == insert && point != pathingPoints.at(count)) {
			pathingPoints.insert(i, point);
			return true;
		}
	}
	return false;

};

void MovingPlatform::clearPath() {
	pathingPoints.clear();
};

sf::Vector2f MovingPlatform::incrementPath() {

	if (pathingPoints.empty()) {
		throw "Platform has no path!";
		travelDuration = sf::Time::Zero;
		return position;
	}

	sf::Vector2f p;

	if (pathingPoints.size() > 0)
		pointCounter %= pathingPoints.size();
	else
		pointCounter = 0;


	int t = 0;
	for (std::vector<sf::Vector2f>::iterator i = pathingPoints.begin(); i < pathingPoints.end(); i++) {
		if (t == pointCounter) {
			p = *i;
			break;
		}
		t++;
	}

	//update travelduration
	sf::Vector2f diff = p - position;
	travelTimer = sf::Time::Zero;
	if (platformSpeed > 0)
		travelDuration = sf::seconds(sqrt(diff.x * diff.x + diff.y * diff.y) / platformSpeed);

	pointCounter++;
	return p;
}
sf::Vector2f MovingPlatform::getPreviousPoint() {
	return pathingPoints[ (pointCounter - 2) % pathingPoints.size() ];
}