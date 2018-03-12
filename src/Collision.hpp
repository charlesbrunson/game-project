#ifndef COLLISION_H
#define COLLISION_H

#include "Math.hpp"
#include <SFML/Graphics.hpp>

namespace {
	class PhysBox {
	public:
		inline sf::FloatRect& getArea() { return area; };
		inline void setArea(sf::FloatRect nArea) { prevArea = area;  area = nArea; };
		inline void resetPrevArea() { prevArea = area; };
	protected:
		sf::FloatRect prevArea;
		sf::FloatRect area;
	};

};

class Static : public PhysBox {
public:
	enum CollisionType {
		NONE,    //invalid
		FLOOR,
		CEILING,
		LEFTWALL,     //wall facing towards the left
		RIGHTWALL,    //wall facing towards the right
		SLOPE_ASC,    //ascending from bottom left to top right
		SLOPE_DESC    //descending from top left to bottom right
	};
	
	CollisionType type;

	inline bool isFlat()  { return type == FLOOR     || type == CEILING;    };
	inline bool isWall()  { return type == LEFTWALL  || type == RIGHTWALL;  };
	inline bool isSlope() { return type == SLOPE_ASC || type == SLOPE_DESC; };
	inline bool isValid() { return type != NONE; };
};

//Dynamic solid for objects
class Solid : public PhysBox {
public:
	Solid() {
		for (int i = 0; i < Cardinal::DIR_COUNT; i++) {
			collisionDir[i] = false;
			clingDir[i]     = false;
		}
	}

	enum CollisionFlag : int {
		DOWN  = 0,
		UP    = 1,
		LEFT  = 2,
		RIGHT = 3,
		COUNT = 4
	};

	bool collisionDir[Cardinal::DIR_COUNT];
	bool clingDir[Cardinal::DIR_COUNT];
	
};

struct Collision {
	// absolute center points of the solid, where it occured
	sf::Vector2f initial;

	// shortest offset to move the solid out of the static, from initial
	sf::Vector2f offset;

	// collision occured between these two
	Solid*  solid = nullptr;
	Static* stat = nullptr;

	// this collision may be overrided by another collision
	bool valid = false;
};

#endif

