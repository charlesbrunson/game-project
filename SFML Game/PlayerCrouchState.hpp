#ifndef PLAYERCROUCHSTATE_H
#define PLAYERCROUCHSTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerCrouchState : public PlayerState {
public:

	PlayerCrouchState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::CROUCH;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	bool canStand();

	static void slide(PlayerCrouchState *state, Player *plr);

protected:
	bool sliding = false;

	float slideDist = 150.f;

	const static sf::Time slideDuration;
	sf::Time slideTimer;

};

#endif