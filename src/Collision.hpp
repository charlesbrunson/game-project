#ifndef COLLISION_H
#define COLLISION_H

#include "Math.hpp"
#include <SFML/Graphics.hpp>

struct Static {

	enum CollisionType {
		NONE,    //invalid
		FLOOR,
		CEILING,
		LEFTWALL,     //wall facing towards the left
		RIGHTWALL,    //wall facing towards the right
		SLOPE_ASC,    //ascending from bottom left to top right
		SLOPE_DESC    //descending from top left to bottom right
	};

	sf::FloatRect prevArea;
	sf::FloatRect area;
	CollisionType type;
};

//Dynamic solid for objects
struct Solid {
	enum CollisionFlag : int {
		DOWN  = 0,
		UP    = 1,
		LEFT  = 2,
		RIGHT = 3,
		COUNT = 4
	};

	sf::FloatRect prevArea;
	sf::FloatRect area;
	bool collisionDir[Cardinal::DIR_COUNT] = {false};
	bool clingDir[Cardinal::DIR_COUNT] = {false};
};

#endif

