#ifndef COLLISION_H
#define COLLISION_H

#include "util/Math.hpp"
#include <SFML/Graphics.hpp>

#include "Surface.hpp"
#include "SurfaceMap.hpp"

class GameObject;

//virtual class for collisions
class Collision {
public:

	enum CollisionType : int {
		None = -1,
		SurfaceType = 0,
		CornerType
	};

	Collision(GameObject* object, GridVec2 gridpos);
	virtual ~Collision() = default;

	static Collision* createCollision(Surface* surface, GameObject* obj, GridVec2 gridpos);
	static Collision* createCollision(Corner* corner, GameObject* obj, GridVec2 gridpos);

	virtual void eval() = 0;

	inline bool isValid() const { return valid; };
	inline GridVec2 getGridPos() const { return place; };
	inline Vec2 getSurfaceNormal() const { return normal; };
	inline Vec2 getStartPos() const {return startPos;};
	inline Vec2 getEndPos() const {return endPos;};
	inline GameObject* getObject() const {return obj;};
	inline CollisionType getType() const {return type;};

protected:
	CollisionType type = None;
	Vec2 startPos; // position of object before the collision
	Vec2 endPos; //position of object after collision

	bool valid = false;
	Vec2 normal;

	GameObject* obj;
	GridVec2 place;
};

#endif
