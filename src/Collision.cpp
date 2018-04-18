#include "obj/GameObject.hpp"
#include "phys/Collision.hpp"

#include "phys/CornerCollision.hpp"
#include "phys/SurfaceCollision.hpp"

Collision::Collision(GameObject* object, GridVec2 gridpos) : obj(object), place(gridpos) {
	startPos = obj->getPosition();
};

Collision* Collision::createCollision(Surface* surface, GameObject* obj, GridVec2 gridpos) {
	return new SurfaceCollision(surface, obj, gridpos);
};

Collision* Collision::createCollision(Corner* corner, GameObject* obj, GridVec2 gridpos) {
	return new CornerCollision(corner, obj, gridpos);
};

