#ifndef SURFACECOLLISION_H
#define SURFACECOLLISION_H

#include "util/Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

#include "Surface.hpp"

class GameObject;

class SurfaceCollision {

public:
	SurfaceCollision(Surface* ss, GameObject* object, GridVec2 gridpos);

	void eval();

	inline Vec2 getOffset() { return offset; };
	inline bool isValid() { return valid; };
	inline GridVec2 getGridPos() { return place; };
	inline Surface* getSurface() {return sur;};

private:

	Vec2 offset;
	bool valid = false;

	Surface* sur;
	GameObject* obj;
	GridVec2 place;
};

#endif

