#ifndef CORNERCOLLISION_H
#define CORNERCOLLISION_H

#include "phys/Corner.hpp"
#include "phys/Collision.hpp"

class GameObject;

class CornerCollision : public Collision{
public:
	CornerCollision(Corner* cc, GameObject* object, GridVec2 gridpos);
	void eval();

	inline Corner* getCorner() {return corner;};

private:
	void surfaceCollision();
	void wallCollision();
	Corner* corner;
};

#endif
