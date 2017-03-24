#ifndef PLAYERAIRSTATE_H
#define PLAYERAIRSTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerAirState : public PlayerState {
public:

	PlayerAirState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::AIR;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	static void downDash(Player *plr);

	float startVelX;
	float endVelY;
	const static float maxAirControl;
	const static float hardLandingThreshold;
	const static float softLandingThreshold;

};

#endif