#define _USE_MATH_DEFINES
#include <math.h>

#include "GameCamera.hpp"
#include "Player.hpp"

GameCamera::GameCamera() {
	followOffset = sf::Vector2f(0.f, -16.f);
	position = sf::Vector2f(150.f, 150.f);
	targetPos = sf::Vector2f(600.f, 150.f);
	camSpeed = camSpeedDefault;
};

void GameCamera::update(sf::Time deltaTime) {
	sf::Vector2f move;

	if (mode == CamModes::SMART && followTarget != nullptr) {

		//update target position
		updateTargetPos();

		move = moveTowards(targetPos) * deltaTime.asSeconds();

		if (move.y == camSpeed.y && followTarget->getVelocity().y > move.y) {
			move.y = followTarget->getVelocity().y;
		}
	}
	else if (mode == CamModes::STATIC) {
		move = moveTowards(targetPos) * deltaTime.asSeconds();
	}

	//snap to target if we pass it
	//horizontal
	if ((position.x < targetPos.x && position.x + move.x > targetPos.x) ||
		(position.x > targetPos.x && position.x + move.x < targetPos.x)) {

		move.x = 0;
		position.x = targetPos.x;
	}
	//vertical
	if ((position.y < targetPos.y && position.y + move.y > targetPos.y) ||
		(position.y > targetPos.y && position.y + move.y < targetPos.y)) {

		move.y = 0;
		position.y = targetPos.y;
	}

	position += move;

	if (shaking) {
		
		float shakeMag = (shakeTimer.asSeconds() / shakeDuration.asSeconds()) * sin(shakeSpeed * 2 * PI_F * shakeTimer.asSeconds());
		shakeOffset = shakeMagnitude * shakeMag;

		shakeTimer -= deltaTime;

		if (shakeTimer <= sf::Time::Zero) {
			shaking = false;
			shakeOffset = sf::Vector2f();
		}

	}
};

void GameCamera::updateTargetPos() {
	camSpeed = camSpeedDefault;
	if (mode == CamModes::SMART && followTarget != nullptr) {
		targetPos.x = followTarget->getPosition().x + followOffset.x + followTarget->camFollowOffset.x;

		if (followTarget->objectType == GameObject::PLAYER) {
			Player* p = ((Player*)followTarget);

			//if (p->getState() != Player::PlayerState::AIR)
				//targetPos.y = followTarget->getPosition().y + followOffset.y;


			if ((followTarget->getPosition().y + followOffset.y + followTarget->camFollowOffset.y >= targetPos.y && followTarget->getVelocity().y > 0) || p->getState() != Player::PlayerState::AIR) {

				//don't update pos y if object is in the air and not below camera
				targetPos.y = followTarget->getPosition().y + followOffset.y + followTarget->camFollowOffset.y;
			}

		}
		else {
			targetPos.y = followTarget->getPosition().y + followOffset.y + followTarget->camFollowOffset.y;
		}

		/*
		if ((followTarget->getPosition().y + followOffset.y >= targetPos.y && followTarget->getVelocity().y > 0) || followTarget->getGrounded()) {

			//don't update pos y if object is in the air and not below camera
			targetPos.y = followTarget->getPosition().y + followOffset.y;
		}
		
		else if (followTarget->objectType == GameObject::PLAYER) {
			Player* p = ((Player*)followTarget);

		}*/
		
	}
}

void GameCamera::setPosition(sf::Vector2f pos, bool smart) {

	position = pos + (smart? followOffset : sf::Vector2f());

};
void GameCamera::setTargetPos(sf::Vector2f pos) {
	targetPos = pos;
};


sf::Vector2f GameCamera::getPosition() const {
	return position + shakeOffset;
};

sf::Vector2f GameCamera::getTargetPos() const {
	return targetPos;
};


//follows entity
void GameCamera::setModeSmart(GameObject *e) {
	mode = CamModes::SMART;
	followTarget = e;
};

//static in position
void GameCamera::setModeStatic(sf::Vector2f pos) {
	mode = CamModes::STATIC;
	targetPos = pos;
	followTarget = nullptr;
};

int GameCamera::getMode() const {
	return mode;
};


sf::Vector2f GameCamera::moveTowards(sf::Vector2f target) const {
	sf::Vector2f vec;

	float xDir = 0;
	float yDir = 0;


	if (position.x > target.x)
		xDir = -1;
	else if (position.x < target.x)
		xDir = 1;

	if (position.y > target.y)
		yDir = -1;
	else if (position.y < target.y)
		//go down faster
		yDir = 1;

	sf::Vector2f acc(abs(target.x - position.x), abs(target.y - position.y));
	acc.x *= (acc.x / speedTaperDist);
	acc.y *= (acc.y / speedTaperDist);

	vec.x = xDir * (camSpeed.x + acc.x*acc.x);
	if (yDir > 0)
		vec.y = yDir * (camSpeed.y + acc.y*acc.y);
	else
		vec.y = yDir * camSpeed.y;

	return vec;
};