#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window/Keyboard.hpp>

#include "GameObject.hpp"
#include "Hurtbox.hpp"
#include "Controls.hpp"

#include "ObjectState.hpp"

#include <iostream>

// The player
class Player : public GameObject {
public:
	
	enum PlayerInput : int {
		LEFT = 0,
		RIGHT,
		UP,
		DOWN,
		JUMP,
		ATTACK,
		SPRINT,
		COUNT
	};
	enum PlayerState : int {
		GROUND = 0,
		AIR,
		DASH,
		WALLCLING,
		LEDGE,
		WALLJUMP,
		CROUCH,
		BOOSTJUMP
	};
	
	int health = 1;
	const int maxHealth = 1;
	
	bool inputEnabled = true;
	sf::Time inputAutoEnableTimer = sf::Time::Zero;

	Player(Level *l);
	~Player();

	void interact(GameObject *obj);
	void update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);
	bool hurt(int amount, sf::Time stunTime = sf::seconds(0));
	
	bool isInvincible() { 
		return invincibility > sf::Time::Zero && !dead;
	};
	
	int getMoveDir() {
		return isHeld(PlayerInput::RIGHT) - isHeld(PlayerInput::LEFT);
	}

	bool isDead() {	
		return dead; 
	};
	
	sf::Vector2f vel() {
		return getVelocity();
	}
	sf::Vector2f pos() {
		return getPosition();
	}
	void setVelX(float x) {
		velocity.x = x;
	}
	void setVelY(float y) {
		velocity.y = y;
	}

	void crouch();
	void stand();

	inline bool isCrouched() {
		return crouched;
	}

	bool isPressed(int key, sf::Time bufferLength = sf::Time::Zero) {

		if (!(key >= 0 && key < PlayerInput::COUNT))
			return false;
		
		return !inputStates[key].confirmed && inputStates[key].lastPressed <= bufferLength;
	};
	bool isHeld(int key) {

		if (!(key >= 0 && key < PlayerInput::COUNT))
			return false;

		return inputStates[key].lastFrameActive;
	};
	void confirmPress(int key) {
		if (key >= 0 && key < PlayerInput::COUNT) {
			inputStates[key].confirmed = true;
			//keyStates[PlayerInput::JUMP].active = false;
		}
	};
	void disableInput();
	void enableInput();
	void setInput(PlayerInput i, bool set);

	void setGodMode(bool set) {	godMode = set; }
	
	int getState() { return playerState; }
	
	const sf::Vector2f collisionStanding = sf::Vector2f(10.f, 30.f);
	const sf::Vector2f collisionCrouched = sf::Vector2f(10.f, 16.f);

	static Animation anim_idle;
	static Animation anim_idle_to_run;
	static Animation anim_running;

	static Animation anim_crouch;
	static Animation anim_idle_to_crouch;
	static Animation anim_crouch_to_idle;
	static Animation anim_slide;
	static Animation anim_slide_to_crouch;
	static Animation anim_crouch_to_run;

	static Animation anim_jump_start;
	static Animation anim_jump_fall_transition;
	static Animation anim_jump_fall;

	static Animation anim_land;
	static Animation anim_dash;
	static Animation anim_dash_to_run;
	static Animation anim_brake;

	static Animation anim_wallcling;
	static Animation anim_walljump;

	static Animation anim_ledgehang;
	static Animation anim_ledgeclimb_1;
	static Animation anim_ledgeclimb_2;
	static Animation anim_clamber_1;
	static Animation anim_clamber_2;
	static Animation anim_ledgehang_look;
	static Animation anim_ledgehang_look_return;

	static Animation anim_boostjump;
	static Animation anim_boostdash;
	static Animation anim_boostdash_to_fall_transition;
	
	void useBoost();
	bool hasBoost();
	void rechargeBoost();

	inline bool canLedgeGrab() { return ledgeGrabDisabledFor <= sf::Time::Zero; };
	void disableLedgeGrab(sf::Time t) { ledgeGrabDisabledFor = t; };

	ObjectState *getObjectState(PlayerState state);
protected:
	
	bool crouched = false;
	bool canBoost = true;

	//pointer to active state
	ObjectState *state_;

	//player states
	ObjectState *groundState;
	ObjectState *airState;
	ObjectState *dashState;
	ObjectState *wallClingState;
	ObjectState *ledgeState;
	ObjectState *wallJumpState;
	ObjectState *crouchState;
	ObjectState *boostJumpState;
	
	int playerState = 0;
	int previousState = 0;
	
	//control flags
	bool isSprinting = false;
	bool canAttack = false; //can player attack right now

	bool godMode;
	bool dead;

	//jump & attack buffers
	sf::Time bufferDuration = sf::seconds(0.25);
	sf::Time invincibility = sf::Time::Zero;
	sf::Time invincibilityDuration = sf::seconds(1.f);
				
	sf::Time ledgeGrabDisabledFor = sf::Time::Zero;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		GameObject::draw(target, states);
	};

	//wrapper interface for Controls class that adds input buffering
	struct Input {
		//how long since this button was pressed, and still isn't confirmed
		sf::Time lastPressed = sf::Time::Zero;

		//input has been caught by some function if true and shouldn't be read until released and pressed again
		bool confirmed = true;

		//active state of this key last frame
		bool lastFrameActive = false;

		//is being actively pressed this frame
		bool active = false;
	};
	Input inputStates[PlayerInput::COUNT];
	void updateInput(sf::Time deltaTime);
	void evaluateInput(PlayerInput i, bool state, sf::Time deltaTime);

};


#endif
