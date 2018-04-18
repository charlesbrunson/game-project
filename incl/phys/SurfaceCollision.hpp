#ifndef SURFACECOLLISION_H
#define SURFACECOLLISION_H

#include "Surface.hpp"
#include "Collision.hpp"

class GameObject;

class SurfaceCollision : public Collision{
public:
	SurfaceCollision(Surface* ss, GameObject* object, GridVec2 gridpos);
	void eval();

	inline Surface* getSurface() {return sur;};

private:
	void surfaceCollision();
	void wallCollision();
	Surface* sur;
};

#endif

