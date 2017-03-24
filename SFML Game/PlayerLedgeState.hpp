#ifndef PLAYERLEDGESTATE_H
#define PLAYERLEDGESTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerLedgeState : public PlayerState {
public:

	PlayerLedgeState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::LEDGE;

		ledgeMove = sf::Vector2f(p->collisionStanding.x, -p->collisionStanding.y);
		clamberMove = sf::Vector2f(p->collisionCrouched.x, -(int)tileSpacing);

		goLedgeMode();
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	struct LedgeGrab {
		bool canGrab = false;
		sf::Vector2f ledgePoint = sf::Vector2f(0, 0);

		enum Side {
			LEFT,
			RIGHT
		};
	};

	static LedgeGrab canLedgeGrab(Player *plr, LedgeGrab::Side side, sf::Time deltaTime, float leniance = 0.f, bool noUpwardsGrab = false, bool waistheight = false);
	
	inline void setLedgePosition(sf::Vector2f p, LedgeGrab::Side side) {
		ledgePosition = p;
		ledgeSide = side;
	}

	//resets to false on exit
	void activateAutoClimb() {
		toAutoClimb = true;
	}

	//head height climb
	void goLedgeMode() {
		climb1 = &plr->anim_ledgeclimb_1;
		climb2 = &plr->anim_ledgeclimb_2;
		mode = LEDGE;
	}

	//waistheight climb
	void goClamberMode() {
		climb1 = &plr->anim_clamber_1;
		climb2 = &plr->anim_clamber_2;
		mode = CLAMBER;
	}

protected:
	sf::Vector2f ledgePosition;
	LedgeGrab::Side ledgeSide;
	bool isClimbing = false;
	bool toAutoClimb = false;

	sf::Vector2f ledgeMove;
	sf::Vector2f clamberMove;

	enum Mode {
		LEDGE,
		CLAMBER
	};

	Mode mode = LEDGE;

	Animation *climb1;
	Animation *climb2;

};

#endif