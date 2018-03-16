#include "obj/plr/PlayerLedgeState.hpp"
#include "obj/plr/PlayerWallJumpState.hpp"
#include "obj/plr/PlayerCrouchState.hpp"
#include "obj/plr/PlayerAirState.hpp"

bool contains(const sf::FloatRect r, const sf::Vector2f p);


void PlayerLedgeState::enter() {

	plr->getAnimSprite().setHFlip(ledgeSide == LedgeGrab::Side::LEFT);
	
	float x = ledgePosition.x + (plr->getCollision().width / 2.f * (ledgeSide == LedgeGrab::Side::LEFT ? 1.f : -1.f));
	float y = ledgePosition.y + (mode == LEDGE ? plr->getCollision().height : tileSpacing);

	plr->setPosition(sf::Vector2f(x, y));

	plr->setVelY(0.f);
	plr->setVelX(0.f);

	isClimbing = toAutoClimb || mode == CLAMBER;
	if (isClimbing)
		plr->getAnimSprite().setAnimation(climb1);
	else if (!plr->getAnimSprite().isPlaying("ledgehang", true)) {
		plr->getAnimSprite().setAnimation("ledgehang");
	}
}

void PlayerLedgeState::exit() {
	toAutoClimb = false;
	plr->camFollowOffset = sf::Vector2f();
}

int PlayerLedgeState::update(sf::Time deltaTime) {

	int move = plr->getMoveDir();
	plr->camFollowOffset = sf::Vector2f();

	if (!isClimbing) {

		if (move != 0 && (move > 0) == (ledgeSide == LedgeGrab::Side::LEFT)) {
			if (!plr->getAnimSprite().isPlaying("ledgehang-look"))
				plr->getAnimSprite().setAnimation("ledgehang-look");
		}
		else if (plr->getAnimSprite().isPlaying("ledgehang-look")) {
			plr->getAnimSprite().setAnimation("ledgehang-look-return");
		}

		if (plr->isPressed(Player::PlayerInput::JUMP, sf::milliseconds(100))) {

			plr->confirmPress(Player::PlayerInput::JUMP);

			if (!plr->isHeld(Player::PlayerInput::DOWN)) {
				//walljump

				PlayerWallJumpState::walljump(ledgeSide == LedgeGrab::Side::LEFT, deltaTime, plr);

				return Player::PlayerState::WALLJUMP;
			}
			else {
				plr->disableLedgeGrab(sf::milliseconds(100));
				return Player::PlayerState::AIR;
			}

		}
		else if (plr->isHeld(Player::PlayerInput::DOWN) && plr->isPressed(Player::PlayerInput::SPRINT, sf::milliseconds(100))) {


			plr->confirmPress(Player::PlayerInput::SPRINT);

			PlayerAirState::downDash(plr);

			plr->disableLedgeGrab(sf::milliseconds(100));

			return Player::PlayerState::AIR;

		}
		else if (plr->isHeld(Player::PlayerInput::UP)) {
			isClimbing = true;
			plr->getAnimSprite().setAnimation(climb1);
		}

	}
	else {
		if (plr->getAnimSprite().isPlaying(climb1) && plr->getAnimSprite().isComplete()) {

			if (mode == LEDGE) {
				plr->setPosition(plr->pos() + sf::Vector2f(ledgeMove.x * (ledgeSide == LedgeGrab::Side::LEFT ? -1.f : 1.f), ledgeMove.y), true);
			}
			else if (mode == CLAMBER) {
				plr->setPosition(plr->pos() + sf::Vector2f(clamberMove.x * (ledgeSide == LedgeGrab::Side::LEFT ? -1.f : 1.f), clamberMove.y), true);
			}

			plr->crouch();
			plr->getAnimSprite().setAnimation(climb2);
		}
		else if (plr->getAnimSprite().isPlaying(climb2)) {

			//slide?
			if (plr->getAnimSprite().getFrame() == 0 && plr->getAnimSprite().isNextFrameWithin(deltaTime) &&
				!((PlayerCrouchState*)plr->getObjectState(Player::PlayerState::CROUCH))->canStand()) {

				plr->setVelX(PlayerState::dashVel * (ledgeSide == LedgeGrab::Side::LEFT ? -1.f : 1.f));
				plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());
				return Player::PlayerState::CROUCH;
				
			}
			//crouch
			else if (plr->getAnimSprite().isComplete()) {
				plr->getAnimSprite().setAnimation("crouch");
				plr->getAnimSprite().setFrame(plr->getAnimSprite().getAnimation()->numOfFrames - 1);
				plr->setVelY(plr->vel().y + gravity * deltaTime.asSeconds());
				return Player::PlayerState::CROUCH;
			}
		}

		sf::Vector2f off = mode == LEDGE ? ledgeMove : clamberMove;
		off.x *= ledgeSide == LedgeGrab::Side::LEFT ? -1.f : 1.f;
		off /= 2.f;

		if (plr->getAnimSprite().isPlaying(climb1)) {
			plr->camFollowOffset = off * plr->getAnimSprite().getAnimProgress();
		}
		else if(plr->getAnimSprite().isPlaying(climb2)) {
			plr->camFollowOffset = (off * plr->getAnimSprite().getAnimProgress()) - off;
		}

	}
	return objectState;
}

void PlayerLedgeState::updateAnimation(sf::Time deltaTime) {
	plr->getAnimSprite().update(deltaTime); 
}

PlayerLedgeState::LedgeGrab PlayerLedgeState::canLedgeGrab(Player *plr, LedgeGrab::Side side, sf::Time deltaTime, float leniance, bool noUpwardsGrab, bool waistheight) {
	PlayerLedgeState::LedgeGrab ledge;

	if (!plr->canLedgeGrab())
		return ledge;

	//get grab area
	sf::Vector2f grabP(plr->getCollision().left + (side == LedgeGrab::Side::LEFT ? 0.f : plr->getCollision().width), 
		!waistheight ? plr->getCollision().top : plr->getCollision().top  + plr->getCollision().height/ 2.f);

	sf::Vector2f grabPN(grabP + (plr->getVelocity() * deltaTime.asSeconds()));

	sf::FloatRect grabArea;

	grabArea.left = std::min(grabP.x - leniance, grabPN.x - leniance);
	grabArea.width = std::max(grabP.x + (leniance * 2.f), grabPN.x + (leniance * 2.f)) - grabArea.left;
	if (!noUpwardsGrab) {
		grabArea.top = std::min(grabP.y - leniance, grabPN.y - leniance);
		grabArea.height = std::max(grabP.y + (leniance * 2.f), grabPN.y + (leniance * 2.f)) - grabArea.top;
	}
	else {
		grabArea.top = std::min(grabP.y, grabPN.y);
		grabArea.height = std::max(grabP.y + leniance, grabPN.y + leniance) - grabArea.top;
	}

	std::vector<sf::Vector2f> ledgePoints;

	bool leftside = side == LedgeGrab::Side::LEFT;
	Level *lvl = plr->getLevel();

	if (grabArea.width > 0.f || grabArea.height > 0.f) {
		//find all ledge points on map
		//this might have to be pre-calculated by level class
		for (auto i = lvl->getUpCol()->begin(); i != lvl->getUpCol()->end(); i++) {

			if (!contains(grabArea, sf::Vector2f(i->left + (leftside ? i->width : 0.f), i->top)))
				continue;
			
			auto walls = side != LedgeGrab::Side::LEFT ? lvl->getLeftCol() : lvl->getRightCol();
			for (auto j = walls->begin(); j != walls->end(); j++) {
				if (!contains(grabArea, sf::Vector2f(j->left + (leftside ? j->width : 0.f), j->top)))
					continue;
				
				if (i->left + (leftside ? i->width : 0.f) == j->left + (leftside ? j->width : 0.f) && i->top == j->top) {
					ledgePoints.push_back(sf::Vector2f(i->left + (leftside ? i->width : 0.f), i->top));

				}
			}
		}

		//check if grab area contains point
		//if so check if our collision can fit there
		for (auto i = ledgePoints.begin(); i != ledgePoints.end();) {
			//area where the player hangs on the ledge
			sf::FloatRect hangArea;
			hangArea.width = plr->collisionStanding.x;
			hangArea.height = !waistheight ? plr->collisionStanding.y : 16.f;
			hangArea.left = i->x - (leftside ? 0.f : hangArea.width);
			hangArea.top = i->y;

			//area where the player needs empty to climb to ledge
			sf::FloatRect climbArea;
			hangArea.width = plr->collisionCrouched.x * 2.f;
			hangArea.height = plr->collisionCrouched.y;
			hangArea.left = i->x - plr->collisionCrouched.x;
			hangArea.top = i->y - plr->collisionCrouched.y;


			bool collided = false;
			//up
			for (auto v = lvl->getUpCol()->begin(); !collided && v != lvl->getUpCol()->end(); v++) {
				if (hangArea.intersects(*v) || climbArea.intersects(*v)) {
					collided = true;
					break;
				}
			}
			//down
			for (auto v = lvl->getDownCol()->begin(); !collided && v != lvl->getDownCol()->end(); v++) {
				if (hangArea.intersects(*v) || climbArea.intersects(*v)) {
					collided = true;
					break;
				}
			}
			//left
			for (auto v = lvl->getLeftCol()->begin(); !collided && v != lvl->getLeftCol()->end(); v++) {
				if (hangArea.intersects(*v) || climbArea.intersects(*v)) {
					collided = true;
					break;
				}
			}
			//right
			for (auto v = lvl->getRightCol()->begin(); !collided && v != lvl->getRightCol()->end(); v++) {
				if (hangArea.intersects(*v) || climbArea.intersects(*v)) {
					collided = true;
					break;
				}
			}

			if (collided) {
				i = ledgePoints.erase(i);
			}
			else {
				i++;
			}
		}
	}

	if (ledgePoints.empty()) {
		ledge.canGrab = false;
		//Log("no\n");
	}
	else {
		ledge.canGrab = true;
		ledge.ledgePoint = ledgePoints.at(0);
		//Log("yes\n");
	}

	return ledge;
}

bool contains(const sf::FloatRect r, const sf::Vector2f p) {
	return p.x >= r.left && p.x <= r.left + r.width &&
		p.y >= r.top && p.y <= r.top + r.height;
}
