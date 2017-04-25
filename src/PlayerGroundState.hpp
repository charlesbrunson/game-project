#ifndef PLAYERGROUNDSTATE_H
#define PLAYERGROUNDSTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerGroundState : public PlayerState {
public:
	
	PlayerGroundState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::GROUND;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	float tempMaxSpeed;
	bool isBraking = false;
	bool startBraking = false;

};

#endif