#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "ObjectState.hpp"
#include "Player.hpp"

class PlayerState : public ObjectState {
public:

	PlayerState(Player *p) {
		plr = p;
	}

	Player *plr;

	static const float jumpVel;

protected:

	void jump(bool crouchJump = false) {
		int move = plr->getMoveDir();
		plr->getAnimSprite().setAnimation("jump-start");

		float j = crouchJump ? crouchJumpVel : jumpVel;
		
		if (move == 0 || abs(plr->vel().x) > 100.f)
			plr->setVelocity(sf::Vector2f(plr->vel().x, j));
		else
			plr->setVelocity(sf::Vector2f(100.f * move, j));
	}

	bool hasJumpClearance();

	static const float gravity;

	static const float runspeedStart;
	static const float runspeedMax;
	static const float runspeedAcc;
	static const float runspeedDecc;

	static const float crouchJumpVel;
	static const float fallVelMax;

	static const float dashVel;
	static const float dashDownVel;
};

#endif