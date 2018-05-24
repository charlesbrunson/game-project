#include "obj/GameObject.hpp"
#include "phys/Collision.hpp"

#include "phys/CornerCollision.hpp"
#include "phys/SurfaceCollision.hpp"

Collision::Collision(GameObject* object, GridVec2 gridpos) : obj(object), place(gridpos) {
	startPos = obj->getPosition();
};

void Collision::apply() {

	if (!isValid())
		return;

	Vec2 newVel = obj->getVelocity();
	Line line(Point(0.f, 0.f), getEndPos() - obj->getPosition());
	if (line.start != line.end)
		newVel = Math::projection(newVel, line.getLeftHandUnit(), true);


	obj->setVelocity(newVel);
	Vec2 offset = getEndPos() - obj->getPosition();
	obj->setPosition(getEndPos(), false);

	Solid::SolidCollision soCol;
	soCol.flags.collisionUp = offset.y < 0.f;
	soCol.flags.collisionDown = offset.y > 0.f;
	soCol.flags.collisionRight = offset.x < 0.f && offset.y == 0.f;
	soCol.flags.collisionLeft = offset.x > 0.f && offset.y == 0.f;
	soCol.collision.reset(this);

	obj->collisionUp |= soCol.flags.collisionUp;
	obj->collisionDown |= soCol.flags.collisionDown;
	obj->collisionRight |= soCol.flags.collisionRight;
	obj->collisionLeft |= soCol.flags.collisionLeft;

	obj->getCollision().curCollisions.push_back(std::move(soCol));

	invalidate();
};

std::string Collision::toString() {
	std::string r;
	if (type == SurfaceType) {
		r += "S";
	}
	else if (type == CornerType) {
		r += "C";
	}
	else if (type == WedgeType) {
		r += "W";
	}
	else {
		r += "U";
	}

	r += ":(";
	r += std::to_string(endPos.x - startPos.x);
	r += ", ";
	r += std::to_string(endPos.y - startPos.y);
	r += ")";

	return r;
}

Collision* Collision::createCollision(Surface* surface, GameObject* obj, GridVec2 gridpos) {
	return new SurfaceCollision(surface, obj, gridpos);
};

Collision* Collision::createCollision(Corner* corner, GameObject* obj, GridVec2 gridpos) {
	return new CornerCollision(corner, obj, gridpos);
};

