#include "PlayerGroundState.hpp"
#include "PlayerLedgeState.hpp"

void PlayerGroundState::enter() {
	plr->rechargeBoost();

	//create effect if playing full land animation
	if (plr->getAnimSprite().isPlaying("land") && plr->getAnimSprite().getFrame() == 0) {

		Effect *landFXRO = new Effect(
			"sprites/player.png", 
			"land-FXover",
			Effect::OVER,
			true
			);

		Effect *landFXLO = new Effect(
			"sprites/player.png",
			"land-FXover",
			Effect::OVER,
			false
			);

		Effect *landFXRU = new Effect(
			"sprites/player.png",
			"land-FXunder",
			Effect::UNDER,
			true
			);

		Effect *landFXLU = new Effect(
			"sprites/player.png",
			"land-FXunder",
			Effect::UNDER,
			false
			);

		landFXRO->setPosition(plr->pos());
		landFXLO->setPosition(plr->pos());
		landFXRU->setPosition(plr->pos());
		landFXLU->setPosition(plr->pos());

		plr->createEffect(landFXRO);
		plr->createEffect(landFXLO);
		plr->createEffect(landFXRU);
		plr->createEffect(landFXLU);

	}

	tempMaxSpeed = std::max<float>(abs(plr->vel().x), runspeedMax);
}
void PlayerGroundState::exit()	{

	startBraking = false;
	isBraking = false;
}

int PlayerGroundState::update(sf::Time deltaTime) {
	int move = plr->getMoveDir();
	
	tempMaxSpeed = std::max<float>(abs(plr->vel().x), runspeedMax);
	if (!plr->isHeld(Player::PlayerInput::SPRINT))
		tempMaxSpeed = std::max(runspeedMax, tempMaxSpeed - runspeedAcc * deltaTime.asSeconds());

	if (move != 0 && (move < 0) != plr->getAnimSprite().getHFlip() && (move > 0) == (plr->vel().x > 0))
		plr->getAnimSprite().setHFlip(move < 0);

	if (move != 0 && (plr->vel().x == 0 || ((move > 0) == (plr->vel().x > 0)))) {

		if (abs(plr->vel().x) <= runspeedStart) {
			plr->setVelX(runspeedStart * (move > 0 ? 1 : -1));
		}
		plr->setVelX(plr->vel().x + runspeedAcc * move * deltaTime.asSeconds());
		
		if (abs(plr->vel().x) > tempMaxSpeed) {
			plr->setVelX(std::min(std::max(-tempMaxSpeed, plr->vel().x), tempMaxSpeed));
		}

		isBraking = false;
	}
	else {
		//implement braking
		if ((abs(plr->vel().x) <= runspeedStart) || (plr->getAnimSprite().getHFlip() == plr->vel().x > 0)) {
			plr->setVelX(0.f);
			isBraking = false;
		}
		else {
			//trigger braking animation
			if (!startBraking && !isBraking)
				startBraking = true;

			isBraking = true;

			//deccelerate
			//brake faster if player is holding other direction
			float deccelMag = move != 0 && (move > 0 != plr->vel().x > 0) ? 8.f : 3.f;
			float deccel = runspeedDecc * deccelMag * (plr->vel().x > 0 ? 1 : -1);
			plr->setVelX(plr->vel().x - deccel * deltaTime.asSeconds());
		}
	}

	plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());
	
	if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

		if (move != 0) {
			plr->getAnimSprite().setHFlip(move < 0);

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
	else if (plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {

		plr->confirmPress(Player::PlayerInput::SPRINT);
		return Player::PlayerState::DASH;
		
	}
	else if (plr->isHeld(Player::PlayerInput::DOWN) && !(plr->getAnimSprite().isPlaying("land") && plr->getAnimSprite().getFrame() < 2)) {

		//smoothing transition from landing animation to crouching a bit
		if (plr->getAnimSprite().isPlaying("land") && plr->getAnimSprite().getFrame() == 2) {
			plr->getAnimSprite().setAnimation("idle-to-crouch");
			plr->getAnimSprite().setFrame(1);
		}

		return Player::PlayerState::CROUCH;
	}

	if (!plr->collisionUp) {
		plr->setVelX(plr->vel().x / 2.f);
		return Player::PlayerState::AIR;
	}
	return objectState;
}

void PlayerGroundState::updateAnimation(sf::Time deltaTime) {
	
	if (!plr->getAnimSprite().isPlaying("jump-start") && !plr->getAnimSprite().isPlaying("idle-to-crouch")) {
		if (plr->vel().x == 0) {
			if (!plr->getAnimSprite().isPlaying("idle", true)) {

				plr->getAnimSprite().setAnimation("idle");

			}
		}
		else if (isBraking) {
			if (startBraking) {
				startBraking = false;
				if (abs(plr->vel().x) > runspeedStart) {

					if (!plr->getAnimSprite().isPlaying("brake")) {
						plr->getAnimSprite().setAnimation("brake");

						if (abs(plr->vel().x) < runspeedMax - 25.f) {
							plr->getAnimSprite().setFrame(3);
						}
					}

				}
			}
			if (abs(plr->vel().x) >= runspeedMax - 25.f && plr->getAnimSprite().isPlaying("brake")) {
				plr->getAnimSprite().setFrame(0);
			}
		}
		else {
			if (!plr->getAnimSprite().isPlaying("running", true)) {

				plr->getAnimSprite().setAnimation("idle-to-run");

				if (abs(plr->vel().x) >= PlayerState::runspeedMax) {
					plr->getAnimSprite().setFrame(1);
				}

			}
			else if (plr->getAnimSprite().isPlaying("running")) {

				float speed = abs(plr->getVelocity().x) / runspeedMax;

				plr->getAnimSprite().setTimeScale( abs(plr->getVelocity().x) / runspeedMax );
			}
		}
	}
	
	plr->getAnimSprite().update(deltaTime);
}
