#include "phys/Solid.hpp"

Vec2 Solid::getFloorUnit() {
	Vec2 v(1.f, 0.f);

	float targetAngle = 0.f;
	bool hasSurf = false;
	float curAngle = targetAngle;
	float testAngle = targetAngle;

	for (auto& ss : curCollisions) {
		if (ss.second.collisionUp) {

			if (ss.first->line.isHorizontal())
				return v;

			if (hasSurf) {
				testAngle = Math::angle(ss.first->line.getVector());
				if (abs(testAngle - targetAngle) < abs(curAngle - targetAngle)) {
					curAngle = testAngle;
				}
			}
			else {
				hasSurf = true;
				curAngle = Math::angle(ss.first->line.getVector());
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
