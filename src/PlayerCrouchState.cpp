#include "obj/plr/PlayerCrouchState.hpp"
#include "obj/plr/PlayerLedgeState.hpp"

const sf::Time PlayerCrouchState::slideDuration = sf::seconds(0.5f);

void PlayerCrouchState::enter() {

	plr->rechargeBoost();
	if (!plr->isCrouched())
		plr->crouch();
	
	sliding = abs(plr->vel().x) >= PlayerState::dashVel;

	if ((!plr->getAnimSprite().isPlaying("land") || plr->getAnimSprite().getFrame() > 2) && (!sliding && !plr->getAnimSprite().isPlaying("crouch"))) 
		plr->getAnimSprite().setAnimation(sliding ? "slide" : "idle-to-crouch");
	
	if (sliding) {
		slideTimer = sf::Time::Zero;
	}
}
void PlayerCrouchState::exit() {
	
	AnimSprite *plrSpr = &plr->getAnimSprite();
	if (plr->collisionUp) {

		if (plrSpr->isPlaying("idle-to-crouch")) {

			int f = plrSpr->getFrame();

			if (f >= plrSpr->getTexFile()->getAnimation("crouch-to-idle")->numOfFrames - 1) {
				plrSpr->setAnimation("crouch-to-idle");
				plrSpr->setFrame(plrSpr->getAnimation()->numOfFrames - 1 - f);
			}
		}
		else if (plrSpr->isPlaying("crouch", true) || sliding) {
			int move = plr->getMoveDir();

			if (move != 0) {
				plrSpr->setAnimation("crouch-to-run");
			}
			else {
				plrSpr->setAnimation("crouch-to-idle");
			}
		}

	}
	else if (!plrSpr->isPlaying("jump-start")) {
		plrSpr->setAnimation("jump-start");
		if (plr->vel().y >= 0)
			plrSpr->setFrame(3);
	}
}

int PlayerCrouchState::update(sf::Time deltaTime) {

	int move = plr->getMoveDir();
	plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());

	bool standable = canStand();

	if (plr->isHeld(Player::PlayerInput::DOWN) || sliding || !standable) {

		if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100)) && standable) {

			plr->stand();

		//	if (sliding && move != 0 && (move < 0 != plr->getAnimSprite().getHFlip()) && hasJumpClearance()) {
			//	plr->setVelX(0.f);
			//	plr->getAnimSprite().setHFlip(!plr->getAnimSprite().getHFlip());
			//}
			
			if (move != 0) {
				//ledge grabbing
				PlayerLedgeState::LedgeGrab ledge =
					PlayerLedgeState::canLedgeGrab(plr, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT, deltaTime, 5.f);

				if (ledge.canGrab) {

					((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
						->activateAutoClimb();

					((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
						->setLedgePosition(ledge.ledgePoint, move < 0 ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT);

					((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))->goLedgeMode();

					plr->getAnimSprite().setHFlip(move < 0);
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

					plr->getAnimSprite().setHFlip(move < 0);
					plr->confirmPress(Player::PlayerInput::JUMP);
					return Player::PlayerState::LEDGE;
				}
			}
			if (hasJumpClearance()) {

				if (sliding && move != 0 && ((move < 0) != plr->getAnimSprite().getHFlip())) {
					plr->setVelX(0.f);
					plr->getAnimSprite().setHFlip(!plr->getAnimSprite().getHFlip());
				}

				jump();
				plr->confirmPress(Player::PlayerInput::JUMP);
				return Player::PlayerState::AIR;
			}
			plr->crouch();
		}

		if (plr->collisionUp || !standable) {

			if (!sliding) {

				plr->setVelX(0.f);
				if (move != 0 && plr->getAnimSprite().isPlaying("crouch"))
					plr->getAnimSprite().setHFlip(move < 0);


				if (plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(250))) {


					plr->confirmPress(Player::PlayerInput::SPRINT);
					slide(this, plr);
				}

			}
			else {
				
				sf::Time smokeInterval = sf::seconds(2.f / 60);
				sf::Time startSmokeDelay = sf::seconds(4.f / 60);
				if (plr->collisionUp && slideTimer >= startSmokeDelay && ceil(slideTimer / smokeInterval) < ceil((slideTimer + deltaTime) / smokeInterval)) {

					Effect *smokeFX = new Effect(
						"sprites/player.png",
						"smoke-FX",
						Effect::UNDER,
						plr->getAnimSprite().getHFlip()
						);

					smokeFX->setVelocity(sf::Vector2f(0.f, -30.f));
					smokeFX->setPosition(plr->pos());

					plr->createEffect(smokeFX);
				}



				slideTimer += deltaTime;
				
				if ((slideTimer >= slideDuration && standable) || plr->vel().x == 0) {

					if (move == 0 || ((plr->vel().x > 0) != (move > 0)))
						plr->setVelX(0.f);

					if (plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(250)) && plr->isHeld(Player::PlayerInput::DOWN)) {
						plr->confirmPress(Player::PlayerInput::SPRINT);

						if (move != 0) {

							if (plr->getAnimSprite().getHFlip() != (move < 0))
								plr->getAnimSprite().setAnimation("slide");

							plr->getAnimSprite().setHFlip(move < 0);
						}

						slide(this, plr);
					}
					else {
						sliding = false;
						plr->getAnimSprite().setAnimation("slide-to-crouch");
					}
				}
			}

			return objectState;
		}
		else {

			plr->stand();
			return Player::PlayerState::AIR;
		}
	}

	plr->stand();
	return Player::PlayerState::GROUND;
}
void PlayerCrouchState::updateAnimation(sf::Time deltaTime){

	if (plr->getAnimSprite().isPlaying("slide") && !sliding) {
		plr->getAnimSprite().setAnimation("crouch");
	}
	else if (!plr->getAnimSprite().isPlaying("slide") && sliding) {
		plr->getAnimSprite().setAnimation("slide");
	}

	plr->getAnimSprite().update(deltaTime);
}

void PlayerCrouchState::slide(PlayerCrouchState *state, Player* plr) {

	plr->setVelX(PlayerState::dashVel * (plr->getAnimSprite().getHFlip() ? -1.f : 1.f));
	state->sliding = true;
	state->slideTimer = sf::Time::Zero;
}

bool PlayerCrouchState::canStand() {

	sf::FloatRect standCol = sf::FloatRect(plr->getPosition(), plr->collisionStanding);
	standCol.left -= plr->collisionStanding.x / 2.f;
	standCol.top -= plr->collisionStanding.y;
	
	std::vector<sf::FloatRect> *ceilings = plr->getLevel()->getDownCol();

	for (std::vector<sf::FloatRect>::const_iterator i = ceilings->begin(); i != ceilings->end(); i++) {

		if (standCol.intersects(*i)) {
			return false;
		}
	}

	return true;
}
