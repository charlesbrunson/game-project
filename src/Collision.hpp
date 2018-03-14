#ifndef COLLISION_H
#define COLLISION_H

#include "Math.hpp"
#include <SFML/Graphics.hpp>

class PhysBox {
public:
	inline sf::FloatRect getArea() { return area; };
	inline sf::FloatRect getPrevArea() {return prevArea;};
	inline void setArea(sf::FloatRect nArea, bool updatePrev = true) {
		if (updatePrev)
			prevArea = area;
		area = nArea;
	};
	inline void resetPrevArea() { prevArea = area; };
protected:
	sf::FloatRect prevArea;
	sf::FloatRect area;
};

class Static : public PhysBox {
public:
	enum CollisionType : int {
		NONE = -1,    //invalid
		FLOOR = 0,
		CEILING,
		LEFTWALL,     //wall facing towards the left
		RIGHTWALL,    //wall facing towards the right
		SLOPE_FLOOR_ASC,    //ascending from bottom left to top right
		SLOPE_FLOOR_DESC,    //descending from top left to bottom right
		SLOPE_CEIL_ASC,
		SLOPE_CEIL_DESC,
		COUNT
	};
	
	CollisionType type;

	inline bool isValid()   { return type != NONE; };
	inline bool isFlat()    { return type == FLOOR     || type == CEILING;    };
	inline bool isWall()    { return type == LEFTWALL  || type == RIGHTWALL;  };
	inline bool isFloor()   { return type == FLOOR   || type == SLOPE_FLOOR_ASC || type == SLOPE_FLOOR_DESC; };
	inline bool isCeiling() { return type == CEILING || type == SLOPE_CEIL_ASC  || type == SLOPE_CEIL_DESC;  };
	inline bool isSlope() {
		return type == SLOPE_FLOOR_ASC  ||
			   type == SLOPE_FLOOR_DESC ||
			   type == SLOPE_CEIL_ASC   ||
			   type == SLOPE_CEIL_DESC;
	};
};

//Dynamic solid for objects
class Solid : public PhysBox {
public:
	Solid() {
		for (int i = 0; i < Cardinal::DIR_COUNT; i++) {
			collisionDir[i] = false;
			clingDir[i]     = false;
			ignoreDir[i]    = false;
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
	bool ignoreDir[Cardinal::DIR_COUNT];
	
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

