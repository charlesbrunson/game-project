#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include "GameObject.hpp"

// Basic gameobject for testing purposes
class TestObject : public GameObject {
public:
	TestObject(Level *level);

	void update(sf::Time deltaTime);
	void updateAnimation(sf::Time deltaTime);

private:
	static Animation anim_idle;

};

#endif