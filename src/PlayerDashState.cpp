#include "obj/plr/PlayerDashState.hpp"
#include "obj/plr/PlayerLedgeState.hpp"

const sf::Time PlayerDashState::dashDuration = sf::seconds(0.25f);

void PlayerDashState::enter() {

	plr->rechargeBoost();
	int move = plr->getMoveDir(); 

	if (move != 0)
		plr->getAnimSprite().setHFlip(move < 0);
	
	if (!plr->getAnimSprite().isPlaying("dash"))
		plr->getAnimSprite().setAnimation("dash");

	createFX();
}
void PlayerDashState::exit()	{

	dashTimer = sf::Time::Zero;
}

int PlayerDashState::update(sf::Time deltaTime) {

	int move = plr->getMoveDir();

	dashTimer += deltaTime;

	plr->setVelX(dashVel * (plr->getAnimSprite().getHFlip() ? -1 : 1));
	plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());
	
	if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

		if (move != 0) { 
			PlayerLedgeState::LedgeGrab ledge =
				PlayerLedgeState::canLedgeGrab(plr, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT, deltaTime, 5.f);

			if (ledge.canGrab) {

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
					->activateAutoClimb();

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
					->setLedgePosition(ledge.ledgePoint, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT);

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))->goLedgeMode();

				plr->confirmPress(Player::PlayerInput::JUMP);
				return Player::PlayerState::LEDGE;
			}


			PlayerLedgeState::LedgeGrab waistLedge =
				PlayerLedgeState::canLedgeGrab(plr, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT, deltaTime, 5.f, false, true);

			if (waistLedge.canGrab) {

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
					->activateAutoClimb();

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
					->setLedgePosition(waistLedge.ledgePoint, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT);

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))->goClamberMode();

				plr->confirmPress(Player::PlayerInput::JUMP);
				return Player::PlayerState::LEDGE;
			}
		}

		if (hasJumpClearance()) {
			jump();
			plr->confirmPress(Player::PlayerInput::JUMP);
			return Player::PlayerState::AIR;
		}
	}
	else if (plr->isHeld(Player::PlayerInput::SPRINT) && plr->isHeld(Player::PlayerInput::DOWN)) {
		return Player::PlayerState::CROUCH;
	}
	else if (!plr->collisionUp) {
		return Player::PlayerState::AIR;
	}
	else if (dashTimer >= dashDuration || 
		(dashTimer > sf::milliseconds(100) && 
		((plr->getAnimSprite().getHFlip() && plr->collisionRight) || 
		(!plr->getAnimSprite().getHFlip() && plr->collisionLeft))	)) {

		if (plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {
			//plr->canDash = false;
			plr->confirmPress(Player::PlayerInput::SPRINT);
			reset();
			return objectState;

		}

		plr->getAnimSprite().setAnimation("dash-to-run");
		return Player::PlayerState::GROUND;
	}
	return objectState;
}

void PlayerDashState::updateAnimation(sf::Time deltaTime) {

	plr->getAnimSprite().update(deltaTime);
}


void PlayerDashState::createFX() {

	Effect *fx = new Effect(
		"sprites/player.png",
		"dash-FX",
		Effect::UNDER,
		plr->getAnimSprite().getHFlip());

	fx->setPosition(plr->pos());
	plr->createEffect(fx);

}
