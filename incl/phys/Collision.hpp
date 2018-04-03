#ifndef COLLISION_H
#define COLLISION_H

#include "util/Math.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

struct Collision {
	Vec2 overlap;
	bool valid = false;
};

Collision isOverlapping(const Rect& a, const Rect& b) {
	Collision r;
	Vec2 dist = Math::center(a) - Math::center(b);

	float xlen = abs(dist.x) - (a.width/2.f) - (b.width/2.f);
	float ylen = abs(dist.y) - (a.height/2.f) - (b.height/2.f);
		
	if (xlen < 0 && ylen < 0.f)
	{
		//overlap

	}
	return r;
};

Collision isOverlapping(const Line& a, const Rect& b) {
	Collision r;
	//TODO
	return r;
};


#endif

