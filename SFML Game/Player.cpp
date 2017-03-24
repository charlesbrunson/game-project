#include "Player.hpp"
#include "Enemy.hpp"

#include "PlayerGroundState.hpp"
#include "PlayerAirState.hpp"
#include "PlayerDashState.hpp"
#include "PlayerWallClingState.hpp"
#include "PlayerWallJumpState.hpp"
#include "PlayerCrouchState.hpp"
#include "PlayerBoostJumpState.hpp"
#include "PlayerLedgeState.hpp"

//defining animations
Animation Player::anim_idle(sf::IntRect(0, 0, 16, 32), sf::Vector2f(8, 32), 4, sf::seconds(12.f / 60));
Animation Player::anim_idle_to_run(sf::IntRect(320, 0, 16, 32), sf::Vector2f(8, 32), 2, sf::seconds(4.f / 60), 0, &anim_running);
Animation Player::anim_running(sf::IntRect(64, 0, 32, 32), sf::Vector2f(16, 32), 8, sf::seconds(5.f / 60));

Animation Player::anim_idle_to_crouch(sf::IntRect(0, 144, 16, 32), sf::Vector2f(8, 32), 2, sf::seconds(4.f / 60), 0, &anim_crouch);
Animation Player::anim_crouch(sf::IntRect(32, 144, 16, 32), sf::Vector2f(8, 32), 2, sf::seconds(24.f / 60));
Animation Player::anim_crouch_to_idle(sf::IntRect(64, 144, 16, 32), sf::Vector2f(8, 32), 2, sf::seconds(4.f / 60), 0, &anim_idle);
Animation Player::anim_slide(sf::IntRect(96, 144, 32, 32), sf::Vector2f(16, 31), 2, sf::seconds(5.f / 60), 0);
Animation Player::anim_slide_to_crouch(sf::IntRect(160, 160, 32, 16), sf::Vector2f(16, 15), 1, sf::seconds(6.f / 60), 0, &anim_crouch);
Animation Player::anim_crouch_to_run(sf::IntRect(192, 144, 32, 32), sf::Vector2f(16, 32), 1, sf::seconds(5.f / 60), 0, &anim_running);

Animation Player::anim_jump_start(sf::IntRect(0, 32, 32, 48), sf::Vector2f(16, 47), 5, sf::seconds(5.f / 60), 0);
Animation Player::anim_jump_fall_transition(sf::IntRect(160, 32, 32, 48), sf::Vector2f(16, 47), 2, sf::seconds(6.f / 60), 0, &anim_jump_fall);
Animation Player::anim_jump_fall(sf::IntRect(224, 32, 32, 48), sf::Vector2f(16, 47), 4, sf::seconds(6.f / 60));

Animation Player::anim_land(sf::IntRect(352, 0, 32, 32), sf::Vector2f(16, 32), 4, sf::seconds(6.f / 60), 0, &anim_idle);
Animation Player::anim_dash(sf::IntRect(0, 80, 32, 32), sf::Vector2f(16, 32), 4, sf::seconds(4.f / 60));
Animation Player::anim_dash_to_run(sf::IntRect(384, 64, 32, 32), sf::Vector2f(16, 32), 2, sf::seconds(3.f / 60), 0, &anim_running, 2);
Animation Player::anim_brake(sf::IntRect(128, 80, 32, 32), sf::Vector2f(16, 32), 4, sf::seconds(6.f / 60), 0, &anim_idle);

Animation Player::anim_wallcling(sf::IntRect(0, 112, 16, 32), sf::Vector2f(6, 32), 3, sf::seconds(5.f / 60), 0);
Animation Player::anim_walljump(sf::IntRect(48, 112, 32, 32), sf::Vector2f(16, 35), 4, sf::seconds(5.f / 60), 0, &anim_jump_fall_transition, 1);

Animation Player::anim_ledgehang(sf::IntRect(480, 0, 16, 32), sf::Vector2f(10, 36), 2, sf::seconds(30.f / 60));
Animation Player::anim_ledgeclimb_1(sf::IntRect(352, 32, 16, 48), sf::Vector2f(9, 53), 2, sf::seconds(4.f / 60), 0);
Animation Player::anim_ledgeclimb_2(sf::IntRect(384, 32, 32, 32), sf::Vector2f(19, 19), 2, sf::seconds(4.f / 60), 0);
Animation Player::anim_clamber_1(sf::IntRect(256, 80, 32, 32), sf::Vector2f(11, 34), 2, sf::seconds(4.f / 60), 0);
Animation Player::anim_clamber_2(sf::IntRect(320, 80, 32, 32), sf::Vector2f(20, 21), 2, sf::seconds(4.f / 60), 0);
Animation Player::anim_ledgehang_look(sf::IntRect(448, 32, 32, 32), sf::Vector2f(18, 37), 2, sf::seconds(5.f / 60), 0);
Animation Player::anim_ledgehang_look_return(sf::IntRect(448, 32, 32, 32), sf::Vector2f(18, 37), 1, sf::seconds(5.f / 60), 0, &anim_ledgehang);

Animation Player::anim_boostjump(sf::IntRect(224, 112, 32, 48), sf::Vector2f(16, 47), 8, sf::seconds(4.f / 60), 0, &anim_jump_fall_transition, 1);

Animation Player::anim_boostdash(sf::IntRect(256, 80, 32, 32), sf::Vector2f(16, 32), 4, sf::seconds(4.f / 60));
Animation Player::anim_boostdash_to_fall_transition(sf::IntRect(384, 80, 32, 32), sf::Vector2f(16, 32), 2, sf::seconds(5.f / 60), 0, &anim_jump_fall_transition);

//----------------------

Player::Player(ResourceLoader *rloader, Level *l, int ID) : GameObject(rloader, l, ID) {
	state_ = nullptr;

	isGlobal = true;

	godMode = Gameplay_Globals::Debug::playerInvulnerable;
	dead = false;

	sprite.setAnimation(anim_idle);
	sprite.updateFrame();

	sprite.getSprite()->setTexture(*sprite.getResources()->getTexture("player.png"));

	stand();
	
	drawPriority = 100;
	objectType = type::PLAYER;
	objectID = ID;

	collisionActive = true;

	groundState = new PlayerGroundState(this);
	airState = new PlayerAirState(this);
	dashState = new PlayerDashState(this);
	wallClingState = new PlayerWallClingState(this);
	wallJumpState = new PlayerWallJumpState(this);
	crouchState = new PlayerCrouchState(this);
	boostJumpState = new PlayerBoostJumpState(this);
	ledgeState = new PlayerLedgeState(this);
	state_ = groundState;
	playerState = PlayerState::GROUND;
};

Player::~Player() {
	delete groundState;
	delete airState;
	delete dashState;
	delete wallClingState;
	delete wallJumpState;
	delete crouchState;
	delete boostJumpState;
	delete ledgeState;
}

ObjectState *Player::getObjectState(Player::PlayerState state) {
	switch (state) {
	case PlayerState::GROUND:	return groundState;
	case PlayerState::CROUCH:	return crouchState;
	case PlayerState::AIR:		return airState;
	case PlayerState::DASH:		return dashState;
	case PlayerState::WALLCLING:return wallClingState;
	case PlayerState::WALLJUMP:	return wallJumpState;
	case PlayerState::BOOSTJUMP:return boostJumpState;
	case PlayerState::LEDGE:	return ledgeState;
	}
	return nullptr;
}


void Player::crouch() {
	setCollisionSize(collisionCrouched.x, collisionCrouched.y);
	setCollisionPosition(position);
	crouched = true;
}
void Player::stand() {
	setCollisionSize(collisionStanding.x, collisionStanding.y);
	setCollisionPosition(position);
	crouched = false;
}

void Player::interact(GameObject *obj) {
	//...
}

void Player::update(sf::Time deltaTime) {

	lifeTime += deltaTime;
		
	//manage states
	if (playerState != state_->objectState) {
		previousState = state_->objectState;
		state_->exit();
		
		switch (playerState) {
		case PlayerState::GROUND: state_ = groundState; break;
		case PlayerState::CROUCH: state_ = crouchState; break;
		case PlayerState::AIR: state_ = airState; break;
		case PlayerState::DASH: state_ = dashState; break;
		case PlayerState::WALLCLING: state_ = wallClingState; break;
		case PlayerState::WALLJUMP: state_ = wallJumpState; break;
		case PlayerState::BOOSTJUMP: state_ = boostJumpState; break;
		case PlayerState::LEDGE: state_ = ledgeState; break;
		}

		state_->enter();
	}


	updateInput(deltaTime);

	droppingThroughFloor = false;
	if (state_ != nullptr) {
		playerState = state_->update(deltaTime);
		if (state_->updatePos())
			setPosition(position += velocity * deltaTime.asSeconds());
	}
	
	onGround = false;
	collisionUp = false;
	collisionRight = false;
	collisionLeft = false;
	collisionDown = false;
	if (ledgeGrabDisabledFor > sf::Time::Zero)
		ledgeGrabDisabledFor -= deltaTime;
}


void Player::updateAnimation(sf::Time deltaTime) {
	//figure out collisions in this frame, by comparing current vel to oldvel
	if (deltaTime > sf::Time::Zero) {
		setGrounded(collisionUp && velocity.y >= 0);
		bool left = collisionRight;
		bool right = collisionLeft;
		bool ceiling = collisionDown;
	}

	if (state_ != nullptr)
		state_->updateAnimation(deltaTime);
}

bool Player::hurt(int amount, sf::Time stunTime) {

	return false;
};


void Player::disableInput() {
	inputEnabled = false;
	inputStates[PlayerInput::JUMP].active = false;
	inputStates[PlayerInput::SPRINT].active = false;
	inputStates[PlayerInput::ATTACK].active = false;
	inputStates[PlayerInput::LEFT].active = false;
	inputStates[PlayerInput::RIGHT].active = false;
	inputStates[PlayerInput::UP].active = false;
	inputStates[PlayerInput::DOWN].active = false;
}

void Player::enableInput() {
	inputEnabled = true;
	inputStates[PlayerInput::JUMP].active = false;
	inputStates[PlayerInput::SPRINT].active = false;
	inputStates[PlayerInput::ATTACK].active = false;
	inputStates[PlayerInput::LEFT].active = false;
	inputStates[PlayerInput::RIGHT].active = false;
	inputStates[PlayerInput::UP].active = false;
	inputStates[PlayerInput::DOWN].active = false;
}

void Player::setInput(PlayerInput i, bool set) {
	inputStates[i].active = set;
	inputStates[i].confirmed = !set;
	if (set) {
		inputStates[i].lastPressed = sf::Time::Zero;
	}
}

void Player::updateInput(sf::Time deltaTime) {
	bool controller = false;
	if (sf::Joystick::isConnected(0)) {
		controller = true;
	}

	if (inputEnabled) {

		evaluateInput(PlayerInput::JUMP, Controls::JumpActive.active, deltaTime);
		evaluateInput(PlayerInput::SPRINT, Controls::SprintActive.active, deltaTime);
		evaluateInput(PlayerInput::ATTACK, Controls::AttackActive.active, deltaTime);
		evaluateInput(PlayerInput::LEFT, Controls::LeftActive.active, deltaTime);
		evaluateInput(PlayerInput::RIGHT, Controls::RightActive.active, deltaTime);
		evaluateInput(PlayerInput::UP, Controls::UpActive.active, deltaTime);
		evaluateInput(PlayerInput::DOWN, Controls::DownActive.active, deltaTime);

	}
	else {
		inputStates[PlayerInput::JUMP].active = false;
		inputStates[PlayerInput::SPRINT].active = false;
		inputStates[PlayerInput::ATTACK].active = false;
		inputStates[PlayerInput::LEFT].active = false;
		inputStates[PlayerInput::RIGHT].active = false;
		inputStates[PlayerInput::UP].active = false;
		inputStates[PlayerInput::DOWN].active = false;
	}
}


void Player::evaluateInput(Player::PlayerInput key, bool state, sf::Time deltaTime) {
	if (!inputStates[key].active && !inputStates[key].lastFrameActive && state) {
		inputStates[key].active = true;
		inputStates[key].confirmed = false;
		inputStates[key].lastPressed = sf::Time::Zero;

	}
	else {
		inputStates[key].lastPressed += deltaTime;
		if (!state)
			inputStates[key].active = false;
	}
	inputStates[key].lastFrameActive = state;
}


void Player::useBoost() {
	canBoost = false;
}
bool Player::hasBoost() {
	return canBoost;
}
void Player::rechargeBoost() {
	canBoost = true;
}