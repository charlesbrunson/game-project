#ifndef PLAYERWALLCLINGSTATE_H
#define PLAYERWALLCLINGSTATE_H

#include "PlayerState.hpp"
#include "Player.hpp"

class PlayerWallClingState : public PlayerState {
public:

	PlayerWallClingState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::WALLCLING;
	}

	void enter();
	void exit();

	void reset() {
		exit();
		enter();
	}

	int update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

	static bool canCling(Player *p, bool leftside);

	const static float clingVelMin;
	const static float clingVelMax;
	const static float clingAcc;

	enum WallSide {
		LEFT = 0,
		RIGHT = 1
	};

	void setSide(WallSide s);

	const sf::Time stickMin = sf::seconds(0.25f);
	sf::Time stickTimer;

	const static float maxAllowableHangoffTop;
	const static float maxAllowableHangoffBottom;

protected:
	int side;

};

#endif