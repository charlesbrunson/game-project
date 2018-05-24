#include "phys/Solid.hpp"

void Solid::clearFlags() {
	curFlags.collisionUp    = false;
	curFlags.collisionDown  = false;
	curFlags.collisionLeft  = false;
	curFlags.collisionRight = false;
}


Vec2 Solid::getFloorUnit() {
	Vec2 v(1.f, 0.f);

	float targetAngle = 0.f;
	bool hasSurf = false;
	float curAngle = targetAngle;
	float testAngle = targetAngle;

	for (auto& ss : curCollisions) {
		if (ss.flags.collisionUp) {
			Vec2 normal = ss.collision->getNormal();
			//Log::msg(std::to_string(normal.x) + ", " + std::to_string(normal.y));

			if (normal == Vec2(0.f, -1.f))
				return v;

			if (hasSurf) {
				testAngle = Math::angle(normal) + Math::toRad(90);
				if (abs(testAngle - targetAngle) < abs(curAngle - targetAngle)) {
					curAngle = testAngle;
				}
			}
			else {
				hasSurf = true;
				curAngle = Math::angle(normal) + Math::toRad(90);
			}
		}
	}

	if (hasSurf)
		return Math::rotate(v, curAngle);

	return v;
}

Vec2 Solid::getCeilUnit() {
	Vec2 v(1.f, 0.f);

	return v;
}
