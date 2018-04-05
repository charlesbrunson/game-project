#ifndef PLAYERWALLJUMPSTATE_H
#define PLAYERWALLJUMPSTATE_H

#include "obj/plr/PlayerState.hpp"
#include "obj/plr/Player.hpp"

class PlayerWallJumpState : public PlayerState {
public:

	PlayerWallJumpState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::WALLJUMP;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	bool cancelling;

	static void walljump(bool left, sf::Time deltaTime, Player *plr);

private:
	float wallJumpVel = 100.f;
	float wallJumpAcc = 50.f;
};

#endif
