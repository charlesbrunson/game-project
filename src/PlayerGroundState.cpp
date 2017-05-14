#include "PlayerGroundState.hpp"
#include "PlayerLedgeState.hpp"

void PlayerGroundState::enter() {
	plr->rechargeBoost();

	//create effect if playing full land animation
	if (plr->getAnimSprite().isPlaying(plr->anim_land) && plr->getAnimSprite().getFrame() == 0) {

		Animation landFXROani(
			sf::IntRect(0, 232, 16, 8),
			sf::Vector2f(16.f, 8.f),
			6,
			sf::seconds(4.f / 60.f),
			0
			);
		Effect *landFXRO = new Effect(
			"sprites/player.png", 
			landFXROani,
			Effect::OVER,
			true
			);


		Animation landFXLOani(
			sf::IntRect(0, 232, 16, 8),
			sf::Vector2f(16.f, 8.f),
			6,
			sf::seconds(4.f / 60.f),
			0);
		Effect *landFXLO = new Effect(
			"sprites/player.png",
			landFXLOani,
			Effect::OVER,
			false
			);

		Animation landFXRUani(
			sf::IntRect(0, 224, 16, 8),
			sf::Vector2f(16.f, 8.f),
			6,
			sf::seconds(4.f / 60.f),
			0);
		Effect *landFXRU = new Effect(
			"sprites/player.png",
			landFXRUani,
			Effect::UNDER,
			true
			);


		Animation landFXLUani(
			sf::IntRect(0, 224, 16, 8),
			sf::Vector2f(16.f, 8.f),
			6,
			sf::seconds(4.f / 60.f),
			0);
		Effect *landFXLU = new Effect(
			"sprites/player.png",
			landFXLUani,
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
	else if (plr->isHeld(Player::PlayerInput::DOWN) && !(plr->getAnimSprite().isPlaying(plr->anim_land) && plr->getAnimSprite().getFrame() < 2)) {

		//smoothing transition from landing animation to crouching a bit
		if (plr->getAnimSprite().isPlaying(plr->anim_land) && plr->getAnimSprite().getFrame() == 2) {
			plr->getAnimSprite().setAnimation(plr->anim_idle_to_crouch);
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
	
	if (!plr->getAnimSprite().isPlaying(plr->anim_jump_start) && !plr->getAnimSprite().isPlaying(plr->anim_idle_to_crouch)) {
		if (plr->vel().x == 0) {
			if (!plr->getAnimSprite().isPlaying(plr->anim_idle, true)) {

				plr->getAnimSprite().setAnimation(plr->anim_idle);

			}
		}
		else if (isBraking) {
			if (startBraking) {
				startBraking = false;
				if (abs(plr->vel().x) > runspeedStart) {

					if (!plr->getAnimSprite().isPlaying(plr->anim_brake)) {
						plr->getAnimSprite().setAnimation(plr->anim_brake);

						if (abs(plr->vel().x) < runspeedMax - 25.f) {
							plr->getAnimSprite().setFrame(3);
						}
					}

				}
			}
			if (abs(plr->vel().x) >= runspeedMax - 25.f && plr->getAnimSprite().isPlaying(plr->anim_brake)) {
				plr->getAnimSprite().setFrame(0);
			}
		}
		else {
			if (!plr->getAnimSprite().isPlaying(plr->anim_running, true)) {

				plr->getAnimSprite().setAnimation(plr->anim_idle_to_run);

				if (abs(plr->vel().x) >= PlayerState::runspeedMax) {
					plr->getAnimSprite().setFrame(1);
				}

			}
			else {

				float speed = abs(plr->getVelocity().x) / runspeedMax;

				plr->getAnimSprite().setTimeScale( abs(plr->getVelocity().x) / runspeedMax );
			}
		}
	}
	
	plr->getAnimSprite().update(deltaTime);
}
