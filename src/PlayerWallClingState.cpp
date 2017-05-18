#include "PlayerWallClingState.hpp"
#include "PlayerWallJumpState.hpp"
#include "PlayerAirState.hpp"
#include "PlayerLedgeState.hpp"

const float PlayerWallClingState::clingVelMin = -50.f; //-50.f;
const float PlayerWallClingState::clingVelMax = 100.f;
const float PlayerWallClingState::clingAcc = 120.f;

const float PlayerWallClingState::maxAllowableHangoffTop = 24.f;
const float PlayerWallClingState::maxAllowableHangoffBottom = 16.f;

void PlayerWallClingState::enter() {
	
	plr->setVelY(std::max(plr->vel().y, clingVelMin));

	plr->getAnimSprite().setHFlip(side != LEFT);
	plr->getAnimSprite().setAnimation("wallcling");

	stickTimer = sf::Time::Zero;
}

void PlayerWallClingState::exit() {
	if (plr->collisionUp) {
		plr->getAnimSprite().setAnimation("land");
		plr->getAnimSprite().setFrame(3);
	}
	else {
		plr->getAnimSprite().setAnimation("jump-fall-transition");
	}
	
}


int PlayerWallClingState::update(sf::Time deltaTime) {
	int move = plr->getMoveDir();
		
	bool cling = canCling(plr, side == LEFT);
	
	if (!plr->collisionUp && plr->vel().y >= clingVelMin && (plr->vel().y <= clingVelMax || (move != 0 && side == RIGHT) == (move > 0)) &&
		((side == LEFT && move < 0) || (side == RIGHT && move > 0) || stickTimer <= stickMin) && cling) {
		
		if (plr->vel().y >= 0.f) {

			stickTimer += deltaTime;

			if (plr->vel().y > clingVelMax)
				plr->setVelY(std::max(clingVelMax, plr->vel().y - 8 * clingAcc * deltaTime.asSeconds()));
			else
				plr->setVelY(std::min(clingVelMax, plr->vel().y + clingAcc * deltaTime.asSeconds()));

		}
		else
			plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());
		
		//air down dash
		if (plr->vel().y >= -50.f && plr->vel().y < 200.f && plr->isHeld(Player::PlayerInput::DOWN) && plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100.f))) {
			
			PlayerAirState::downDash(plr);
			return Player::PlayerState::AIR;
		}

		//wall jump
		if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

			PlayerWallJumpState::walljump(side == LEFT, deltaTime, plr);
			plr->confirmPress(Player::PlayerInput::JUMP);
			return Player::PlayerState::WALLJUMP;
		}

		//pushing towards wall, try ledge grabbing
		if (move != 0 && (move < 0) == (side == LEFT)) {
			PlayerLedgeState::LedgeGrab ledge =
				PlayerLedgeState::canLedgeGrab(plr, side == LEFT ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT, deltaTime, 5.f, true);
			
			if (ledge.canGrab) {

				//plr->getAnimSprite().setHFlip(move < 0);

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))
					->setLedgePosition(ledge.ledgePoint, side == LEFT ? PlayerLedgeState::LedgeGrab::Side::LEFT : PlayerLedgeState::LedgeGrab::Side::RIGHT);

				((PlayerLedgeState*)plr->getObjectState(Player::PlayerState::LEDGE))->goLedgeMode();

				return Player::PlayerState::LEDGE;
			}
		}

		return objectState;
	} 
	else {
		if (plr->collisionUp) {
			plr->getAnimSprite().setHFlip(!plr->getAnimSprite().getHFlip());
			return Player::PlayerState::GROUND;
		}
		else {
			plr->getAnimSprite().setHFlip(!plr->getAnimSprite().getHFlip());

			plr->setVelX(0.f);
			return Player::PlayerState::AIR;
		}
	}
}

void PlayerWallClingState::updateAnimation(sf::Time deltaTime) {
	plr->getAnimSprite().update(deltaTime);
}


bool PlayerWallClingState::canCling(Player *p, bool leftside) {
	//try can find a valid surface

	Level *l = p->getLevel();
	std::vector<sf::FloatRect> *levelSurfaces = nullptr;

	sf::FloatRect pC = p->getCollision();

	if (leftside) {
		levelSurfaces = l->getRightCol();
		

		for (std::vector<sf::FloatRect>::const_iterator i = levelSurfaces->begin(); i != levelSurfaces->end(); i++) {


			float t = pC.top + maxAllowableHangoffTop;
			float b = pC.top + pC.height - maxAllowableHangoffBottom;

			bool h = t >= i->top && b <= i->top + i->height;
			bool w = pC.intersects(*i) || pC.left == i->left + i->width;
			
			if (h && w)
				return true;
		}

	} 
	else {

		levelSurfaces = l->getLeftCol();

		for (std::vector<sf::FloatRect>::const_iterator i = levelSurfaces->begin(); i != levelSurfaces->end(); i++) {

			float t = pC.top + maxAllowableHangoffTop;
			float b = pC.top + pC.height - maxAllowableHangoffBottom;

			bool h = t >= i->top && b <= i->top + i->height;
			bool w = pC.intersects(*i) || pC.left + pC.width == i->left;
			
			if (h && w)
				return true;
		}
	}
	return false;
}

void PlayerWallClingState::setSide(WallSide s) {
	side = s;
}
