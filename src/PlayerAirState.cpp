#include "PlayerAirState.hpp"
#include "PlayerWallClingState.hpp"
#include "PlayerWallJumpState.hpp"
#include "PlayerLedgeState.hpp"

const float PlayerAirState::maxAirControl = 200.f;
const float PlayerAirState::softLandingThreshold = 225.f;
const float PlayerAirState::hardLandingThreshold = 300.f;

void PlayerAirState::enter() {
	startVelX = abs(plr->vel().x);
	endVelY = 0.f;

	AnimSprite* spr = &plr->getAnimSprite();

	if (!spr->isPlaying("jump-start") &&
		!spr->isPlaying("jump-fall-transition") &&
		!spr->isPlaying("jump-fall") /*&&
		!spr->isPlaying("boostdash_to_fall_transition")*/) {


		if (plr->getVelocity().y < 0) {
			spr->setAnimation("jump-start");
			spr->setFrame(spr->getAnimation()->numOfFrames - 1);
		}
		else {
			spr->setAnimation("jump-fall-transition");
		}
	}
}
void PlayerAirState::exit()	{

}

int PlayerAirState::update(sf::Time deltaTime) {
	int move = plr->getMoveDir();
	
	float aircontrolAcc = 100.f + (plr->vel().y / 4.f);
	/*
	if (plr->vel().y < 0) {
		aircontrolAcc = 100.f + (plr->vel().y / 4.f);
	}
	else {
		aircontrolAcc = 100.f + (plr->vel().y / 4.f);
	}
	*/
	aircontrolAcc = std::max(std::min(maxAirControl, aircontrolAcc), 0.f);
	
	if (move != 0) {
		
		startVelX = std::min<float>(abs(plr->vel().x), startVelX);

		float maxV = std::max<float>(abs(startVelX), 75.f);

		plr->setVelX(plr->vel().x + std::min(aircontrolAcc * 8.f, 600.f) * (move > 0 ? 1.f : -1.f) * deltaTime.asSeconds());
		plr->setVelX(std::max(std::min(maxV, plr->vel().x), -maxV));

	}
	else {

		//apply air resistance
		float airResistance = 100.f;

		if (abs(plr->getVelocity().x) > airResistance * deltaTime.asSeconds())
			plr->setVelX(plr->vel().x + (-airResistance * (plr->getVelocity().x > 0 ? 1.f : -1.f) * deltaTime.asSeconds()));
		else
			plr->setVelX(0.f);

		//down dash
		if (plr->vel().y >= -50.f && plr->vel().y < 200.f && plr->isHeld(Player::PlayerInput::DOWN) && plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {

			plr->confirmPress(Player::PlayerInput::SPRINT);

			downDash(plr);

		}
	}

	plr->setVelY(plr->vel().y + (gravity * deltaTime.asSeconds()));
	plr->setVelY(std::min(plr->getVelocity().y, fallVelMax));

	if (plr->collisionUp) {

		if (endVelY > 100.f && (move == 0 || (move > 0 != plr->vel().x > 0)) && abs(plr->vel().x) <= runspeedMax) {
			plr->getAnimSprite().setAnimation("land");
			plr->setVelX(0.f);

			if (endVelY < softLandingThreshold)
				plr->getAnimSprite().setFrame(3);
			else if (endVelY < hardLandingThreshold)
				plr->getAnimSprite().setFrame(2);

		}
		else {
			plr->getAnimSprite().setAnimation("idle");
		}

		if (plr->isHeld(Player::PlayerInput::DOWN) && plr->isHeld(Player::PlayerInput::SPRINT)) {

			if (plr->vel().x != 0.f)
				plr->getAnimSprite().setHFlip(plr->vel().x < 0);

			plr->getAnimSprite().setAnimation("idle-to-crouch");
			return Player::PlayerState::CROUCH;
		}
		else
			return Player::PlayerState::GROUND;
		
	}
	else {

		if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

			//wall jump, boost jump
			if (PlayerWallClingState::canCling(plr, true)) {

				PlayerWallJumpState::walljump(true, deltaTime, plr);
				plr->confirmPress(Player::PlayerInput::JUMP);

				return Player::PlayerState::WALLJUMP;
			}
			else if (PlayerWallClingState::canCling(plr, false)) {

				PlayerWallJumpState::walljump(false, deltaTime, plr);
				plr->confirmPress(Player::PlayerInput::JUMP);

				return Player::PlayerState::WALLJUMP;
			}
			else if (plr->hasBoost()) {
				//boost jump
				plr->confirmPress(Player::PlayerInput::JUMP);
				return Player::PlayerState::BOOSTJUMP;
			}
		}
		endVelY = plr->vel().y;
	}
	
	if (move != 0) {

		PlayerLedgeState::LedgeGrab ledge = 
			PlayerLedgeState::canLedgeGrab(plr, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT, deltaTime, 0.f, true);

		if (ledge.canGrab) {
			
			((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
				->setLedgePosition(ledge.ledgePoint, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT);

			((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))->goLedgeMode();

			return Player::PlayerState::LEDGE;
		}
		else if (plr->vel().y >= PlayerWallClingState::clingVelMin && PlayerWallClingState::canCling(plr, move < 0)) {
			((PlayerWallClingState*)plr->getObjectState(Player::PlayerState::WALLCLING))
				->setSide(move < 0 ? PlayerWallClingState::LEFT : PlayerWallClingState::RIGHT);
			return Player::PlayerState::WALLCLING;
		}
	}
	return objectState;
}

void PlayerAirState::updateAnimation(sf::Time deltaTime) {

	if (!plr->getAnimSprite().isPlaying("crouch", true) && !plr->getAnimSprite().isPlaying("idle", true)) {

		
		if (plr->vel().y > 50.f) {
			if (plr->getAnimSprite().isPlaying("jump-start") && plr->getAnimSprite().isComplete())
				plr->getAnimSprite().setAnimation("jump-fall-transition");

		}
		/*
		else if (plr->vel().y < -180.f) {
			plr->getAnimSprite().setAnimation(plr->anim_jump_start);
		}*/
	}
	
	plr->getAnimSprite().update(deltaTime);
}

void PlayerAirState::downDash(Player* plr) {
	plr->setVelX(std::min(runspeedMax, std::max(plr->vel().x, -runspeedMax)));
	plr->setVelY(200.f);

	if (!plr->getAnimSprite().isPlaying("jump-fall") && !plr->getAnimSprite().isPlaying("jump-fall-transition"))
		plr->getAnimSprite().setAnimation("jump-fall-transition");
}
