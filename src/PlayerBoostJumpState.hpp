#ifndef PLAYERBOOSTJUMPSTATE_H
#define PLAYERBOOSTJUMPSTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerBoostJumpState : public PlayerState {
public:

	PlayerBoostJumpState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::BOOSTJUMP;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);
	
protected:
	const static sf::Time boostDuration;
	sf::Time boostTimer;

	float boostVel;

	float boostDecc;

	const static float maxSpeed;
	const static float horizontalAcc;

	void createFX();

	Effect *jetboost = nullptr;
};

#endif