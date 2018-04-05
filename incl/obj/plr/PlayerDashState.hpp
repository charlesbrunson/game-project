#ifndef PLAYERDASHSTATE_H
#define PLAYERDASHSTATE_H

#include "obj/plr/PlayerState.hpp"
#include "obj/plr/Player.hpp"

class PlayerDashState : public PlayerState {
public:

	PlayerDashState(Player *p) : PlayerState(p) {
		objectState = Player::PlayerState::DASH;
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
	static const sf::Time dashDuration;
	sf::Time dashTimer = sf::Time::Zero;

	void createFX();

};

#endif
