#include "PlayerWallJumpState.hpp"
#include "PlayerWallClingState.hpp"
#include "PlayerAirState.hpp"

void PlayerWallJumpState::enter() {
	plr->getAnimSprite().setAnimation(plr->anim_walljump);
	
	plr->setVelY(-280.f);
	plr->setVelX(wallJumpVel * (!plr->getAnimSprite().getHFlip() ? 1.f : -1.f));

	cancelling = false;
}

void PlayerWallJumpState::exit() {

}

int PlayerWallJumpState::update(sf::Time deltaTime) {
	int move = plr->getMoveDir();
	
	if (plr->getAnimSprite().isPlaying(plr->anim_walljump) || plr->getAnimSprite().isPlaying(plr->anim_jump_fall_transition)) {

		if (!cancelling && (move != 0 && plr->getAnimSprite().getHFlip() == move > 0 && plr->getAnimSprite().getFrame() > 1) || plr->vel().x == 0.f)
			cancelling = true;

		plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());

		if (abs(plr->vel().x) < wallJumpVel && move != 0 && move > 0 != plr->getAnimSprite().getHFlip()) {
			plr->setVelX(plr->vel().x + wallJumpAcc * (move > 0 ? 1.f : -1.f));
			plr->setVelX(std::min(std::max(-wallJumpVel, plr->vel().x), wallJumpVel));
		}

		//air down dash
		if (plr->vel().y >= -50.f && plr->vel().y < 200.f && plr->isHeld(Player::PlayerInput::DOWN) && plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {

			PlayerAirState::downDash(plr);
			return Player::PlayerState::AIR;
		}
		
		//clinging/walljump
		if ((move == 0 || move > 0 != plr->getAnimSprite().getHFlip()) && PlayerWallClingState::canCling(plr, plr->getAnimSprite().getHFlip())) {
			

			if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {
				
				walljump(plr->getAnimSprite().getHFlip(), deltaTime, plr);

				plr->confirmPress(Player::PlayerInput::JUMP);

				//walljump
				reset();
				return Player::PlayerState::WALLJUMP;
			}
			else if (plr->vel().y >= PlayerWallClingState::clingVelMin) {
				((PlayerWallClingState*)plr->getObjectState(Player::PlayerState::WALLCLING))
					->setSide(plr->getAnimSprite().getHFlip() ? PlayerWallClingState::LEFT : PlayerWallClingState::RIGHT);

				return Player::PlayerState::WALLCLING;
			}
		}

		//boostjump
		if (plr->hasBoost() && plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

			plr->confirmPress(Player::PlayerInput::JUMP);
			return Player::PlayerState::BOOSTJUMP;
			
		}
		
		if (plr->collisionUp)
			return Player::PlayerState::GROUND;
		else if (plr->collisionDown || (cancelling && plr->getAnimSprite().isPlaying(plr->anim_jump_fall_transition)))
			return Player::PlayerState::AIR;
		else
			return Player::PlayerState::WALLJUMP;
	}
	else {
		if (!plr->collisionUp)
			return Player::PlayerState::AIR;
		else
			return Player::PlayerState::GROUND;
	}
}

void PlayerWallJumpState::updateAnimation(sf::Time deltaTime) {
	int move = plr->getMoveDir();

	if (plr->getAnimSprite().isPlaying(plr->anim_walljump)) {

		if (plr->vel().y < -150.f && plr->getAnimSprite().getFrame() >= 0)
			plr->getAnimSprite().setFrame(0);

	}

	plr->getAnimSprite().update(deltaTime);
}


void PlayerWallJumpState::walljump(bool left, sf::Time deltaTime, Player *plr) {

	plr->getAnimSprite().setHFlip(!left);
	plr->getAnimSprite().setAnimation(plr->anim_walljump);

	//move away from wall, to prevent clipping
	plr->setVelX((!left ? -1.f : 1.f) * (6.f / deltaTime.asSeconds()));
	plr->setVelY(-2.f / deltaTime.asSeconds());
}