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

	//inline Vec2 getOffset() { return newPos - obj->getPosition(); };
	inline bool isValid() { return valid; };
	inline GridVec2 getGridPos() { return place; };
	inline Surface* getSurface() { return sur; };
	inline Vec2 getSurfaceNormal() { return normal; };

	enum CollisionType : int {
		None = -1,
		FloorCollision = 0,
		CeilCollision,
		WallCollision,
		CornerCollision
	};

	Vec2 newPos;

private:

	CollisionType type = CollisionType::None;

	void surfaceCollision();
	void wallCollision();
	void cornerCollision();

	

	//Vec2 offset;
	bool valid = false;
	Vec2 normal;

	Surface* sur;
	GameObject* obj;
	GridVec2 place;
};

#endif

