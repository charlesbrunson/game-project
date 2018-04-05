#ifndef OBJECTSTATE_H
#define OBJECTSTATE_H

#include <SFML/System.hpp>

// Abstraction for gameobject state machines
class ObjectState {
public:

	virtual ~ObjectState() = default;

	virtual void enter() {};
	virtual void exit() {};

	void reset() {
		exit();
		enter();
	}

	virtual int update(sf::Time deltaTime) = 0;
	virtual void updateAnimation(sf::Time deltaTime) = 0;

	virtual bool updatePos() {
		return true;
	}

	int objectState = 0;
};

#endif
