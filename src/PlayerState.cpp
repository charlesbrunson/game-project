#include "PlayerState.hpp"

const float PlayerState::gravity = 605.f;

const float PlayerState::runspeedStart = 40.f;
const float PlayerState::runspeedMax = 140.f;
const float PlayerState::runspeedAcc = 300.f;
const float PlayerState::runspeedDecc = 150.f;

const float PlayerState::jumpVel = -205.f;
const float PlayerState::crouchJumpVel = -160.f;
const float PlayerState::fallVelMax = 330.f;

const float PlayerState::dashVel = 175.f;
const float PlayerState::dashDownVel = 150.f;

bool PlayerState::hasJumpClearance() {
	sf::FloatRect headRoom;

	if (!plr->isCrouched()) {
		headRoom = plr->getCollision();
	}
	else {
		headRoom = plr->getCollision();
		headRoom.top -= (plr->collisionStanding.y - headRoom.height);
		headRoom.height = plr->collisionStanding.y;
	}

	headRoom.height = 1.f + (32.f - plr->collisionStanding.y);
	headRoom.top -= headRoom.height;

	auto dCol = plr->getLevel()->getDownCol();
	for (auto i = dCol->begin(); i != dCol->end(); i++)
		if (i->intersects(headRoom))
			return false;

	return true;
}