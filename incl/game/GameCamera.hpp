#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include <SFML/System.hpp>
#include "obj/GameObject.hpp"

class GameCamera {
public:

	GameCamera();

	enum CamModes : int {
		//remain in one spot
		STATIC = 0,
		//follow player 
		SMART = 1
	};

	void update(sf::Time deltaTime);
	void updateTargetPos();

	void setPosition(sf::Vector2f pos, bool smart = false);
	void setTargetPos(sf::Vector2f pos);

	sf::Vector2f getPosition() const;
	sf::Vector2f getTargetPos() const;


	//follows entity
	void setModeSmart(GameObject *e);

	//static in position
	void setModeStatic(sf::Vector2f pos);

	int getMode() const;

	void shake(sf::Vector2f mag, sf::Time duration, float oscillationsPerSec) {
		shaking = true;
		shakeMagnitude = mag;
		shakeDuration = duration;
		shakeTimer = shakeDuration;
		shakeSpeed = oscillationsPerSec;
	};

private:
	bool shaking = false;
	sf::Vector2f shakeOffset;
	sf::Vector2f shakeMagnitude;
	sf::Time shakeDuration;
	sf::Time shakeTimer;
	float shakeSpeed;

	const sf::Vector2f camSpeedDefault = sf::Vector2f(225.f, 225.f);
	const sf::Vector2f camSpeedLadder = sf::Vector2f(225.f, 30.f);
	sf::Vector2f camSpeed;
	float speedTaperDist = 20.f;

	sf::Vector2f targetPos;

	//center of camera
	sf::Vector2f position;

	sf::Vector2f followOffset;

	int mode = CamModes::STATIC;

	GameObject *followTarget = nullptr;

	//returns a speed vector solution to approach target position
	//could eventually make this directional based rather than orthogonal
	sf::Vector2f moveTowards(sf::Vector2f target) const;
};


#endif
