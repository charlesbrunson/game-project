#include "phys/CornerCollision.hpp"

#include "util/Log.hpp"

#include "obj/GameObject.hpp"

CornerCollision::CornerCollision(Corner* cc, GameObject* object, GridVec2 gridpos) : Collision(object, gridpos), corner(cc) {
	type = CollisionType::CornerType;
	eval();
};

void CornerCollision::eval() {
	valid = false;

	normal = corner->normal;

	sf::FloatRect rect = obj->getCollision();
	sf::FloatRect prev = obj->getPrevFrameCollision();

	bool goingLeft  = rect.left < prev.left;
	bool goingRight = rect.left + rect.width > prev.left + prev.width;

	auto rectIntersects = [](const sf::FloatRect& r, const Point& p) -> bool {
		return p.x > r.left && p.x < r.left + r.width &&
			   p.y > r.top  && p.y < r.top  + r.height;

	};

	if ((goingLeft && corner->normal.x > 0.f) || (goingRight && corner->normal.x < 0.f)) {
		if (rectIntersects(rect, corner->position)) {
			if (goingLeft && prev.left >= corner->position.x) {
				endPos = obj->getPosition() + (normal * (corner->position.x - rect.left));
				valid = true;
			}
			else if (goingRight && prev.left + prev.width <= corner->position.x) {
				endPos = obj->getPosition() + (normal * (rect.left + rect.width - corner->position.x));
				valid = true;
			}
			//if (valid) {
			//	Log::msg(std::to_string(startPos.x) +", "+ std::to_string(startPos.y));
			//	Log::msg(std::to_string(endPos.x) +", "+ std::to_string(endPos.y));
			//}
			return;
		}
	}

}
