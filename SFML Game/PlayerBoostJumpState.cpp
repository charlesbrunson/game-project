#include "PlayerBoostJumpState.hpp"
#include "PlayerAirState.hpp"
#include "PlayerWallClingState.hpp"
#include "PlayerWallJumpState.hpp"

const sf::Time PlayerBoostJumpState::boostDuration = sf::seconds(16.f / 60.f);

const float PlayerBoostJumpState::maxSpeed = 100.f;
const float PlayerBoostJumpState::horizontalAcc = 600.f;

void PlayerBoostJumpState::enter() {
	if (!plr->collisionUp) {
		plr->useBoost();

		createFX();



		boostTimer = sf::Time::Zero;

		plr->getAnimSprite().setAnimation(plr->anim_boostjump);
		
		int move = plr->getMoveDir();
		if (move != 0)
			plr->getAnimSprite().setHFlip(move < 0);


		boostVel = jumpVel - 15.f;
		plr->setVelY(boostVel);

		boostDecc = abs(plr->vel().x) / plr->getAnimSprite().getAnimationLength().asSeconds();
	}
	else {
		//convert to normal jump
		jump();
		plr->getAnimSprite().setAnimation(plr->anim_jump_start);
	}
}

void PlayerBoostJumpState::exit() {
	if (jetboost != nullptr && !jetboost->toDelete)
		jetboost->toDelete = true;

	jetboost = nullptr;
}

int PlayerBoostJumpState::update(sf::Time deltaTime) {

	if (jetboost != nullptr && jetboost->toDelete)
		jetboost = nullptr;

	if (plr->collisionUp) {
		if (plr->getAnimSprite().isPlaying(plr->anim_boostjump))
			return Player::PlayerState::GROUND;
		else {
			return Player::PlayerState::AIR;
		}	
	}

	int move = plr->getMoveDir();

	boostVel -= deltaTime.asSeconds() * gravity;
	if (boostVel > 0)
		plr->setVelY(-boostVel);
	else
		plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());

	float aircontrolAcc = 0.f;
	if (plr->vel().y < 0) {
		aircontrolAcc = 100.f + (plr->vel().y / 4.f);
	}
	else {
		aircontrolAcc = 100.f + (plr->vel().y / 4.f);
	}
	aircontrolAcc = std::max(std::min(PlayerAirState::maxAirControl, aircontrolAcc), 0.f);


	float airResistance = 200.f;

	if (abs(plr->getVelocity().x) > airResistance * deltaTime.asSeconds())
		plr->setVelX(plr->vel().x + (-airResistance * (plr->getVelocity().x > 0 ? 1.f : -1.f) * deltaTime.asSeconds()));
	else
		plr->setVelX(0.f);


	if (move != 0) {
		plr->setVelX(plr->vel().x + std::min(aircontrolAcc * 8.f, 600.f) * (move > 0 ? 1.f : -1.f) * deltaTime.asSeconds());
	}
	plr->setVelX(std::max(std::min(85.f, plr->vel().x), -85.f));


	if (boostTimer < boostDuration) {
		boostTimer += deltaTime;

		plr->setVelY(std::min(-50.f, plr->vel().y));
		
	}

	if (move != 0 && PlayerWallClingState::canCling(plr, move < 0)) {
		if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

			PlayerWallJumpState::walljump(move < 0, deltaTime, plr);
			plr->confirmPress(Player::PlayerInput::JUMP);
			
			return Player::PlayerState::WALLJUMP;
		}
		else if (plr->vel().y >= PlayerWallClingState::clingVelMin) {
			((PlayerWallClingState*)plr->getObjectState(Player::PlayerState::WALLCLING))
				->setSide(move < 0 ? PlayerWallClingState::LEFT : PlayerWallClingState::RIGHT);
			return Player::PlayerState::WALLCLING;
		}
	}

	//air down dash
	if (plr->vel().y >= -50.f && plr->vel().y < 200.f && plr->isHeld(Player::PlayerInput::DOWN) && plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {
		plr->confirmPress(Player::PlayerInput::SPRINT);

		PlayerAirState::downDash(plr);
		return Player::PlayerState::AIR;
	}

	if (!plr->getAnimSprite().isPlaying(plr->anim_boostjump))
		return Player::PlayerState::AIR;
	else
		return Player::PlayerState::BOOSTJUMP;
}
void PlayerBoostJumpState::updateAnimation(sf::Time deltaTime) {
	plr->getAnimSprite().update(deltaTime);
}

void PlayerBoostJumpState::createFX() {
	if (jetboost != nullptr)
		jetboost->toDelete = true;

	jetboost = new Effect(
		"player.png",
		Animation(
			sf::IntRect(192, 240, 9, 16),
			sf::Vector2f(5.f, -1.f),
			5,
			sf::seconds(4.f/60.f),
			0),
		Effect::UNDER,
		plr->getAnimSprite().getHFlip(),
		plr->getResources()
		);

	Effect *smokeUnder = new Effect(
		"player.png",
		Animation(
			sf::IntRect(0, 240, 32, 8),
			sf::Vector2f(16.f, 5.f),
			6,
			sf::seconds(4.f / 60.f),
			0),
		Effect::UNDER,
		plr->getAnimSprite().getHFlip(),
		plr->getResources()
		);

	Effect *smokeOver = new Effect(
		"player.png",
		Animation(
			sf::IntRect(0, 248, 32, 8),
			sf::Vector2f(16.f, 5.f),
			6,
			sf::seconds(4.f / 60.f),
			0),
		Effect::OVER,
		plr->getAnimSprite().getHFlip(),
		plr->getResources()
		);

	jetboost->setParent(plr);
	smokeUnder->setPosition(plr->pos());
	smokeOver->setPosition(plr->pos());

	plr->createEffect(jetboost);
	plr->createEffect(smokeUnder);
	plr->createEffect(smokeOver);

}