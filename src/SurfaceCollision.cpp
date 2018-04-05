#include "phys/SurfaceCollision.hpp"

#include "util/Log.hpp"

#include "obj/GameObject.hpp"

SurfaceCollision::SurfaceCollision(Surface* ss, GameObject* object, GridVec2 gridpos) : sur(ss), obj(object), place(gridpos) {
	eval();
};

void SurfaceCollision::eval() {
	valid = false;
	offset = Vec2();

	if (sur == nullptr || obj == nullptr)
		return;

	Vec2 normal = sur->line.getLeftHandUnit();
	sf::FloatRect rect = obj->getCollision();
	sf::FloatRect prev = obj->getPrevFrameCollision();

	if (Math::intersects(sur->line, rect) && !Math::intersects(sur->line, prev, false)) {
		
		if (sur->line.isVertical()) {
			if (normal.x > 0.f && rect.left < prev.left) {
				offset = normal * (sur->line.start.x - rect.left);				
				valid = true;
				return;
			}
			else if (normal.x < 0.f && rect.left + rect.width > prev.left + prev.width) {
				offset = normal * (rect.left + rect.width - sur->line.start.x);				
				valid = true;
				return;
			}
		}
		else if (sur->line.isHorizontal()) {
			if (normal.y > 0.f && rect.top < prev.top) {
				offset = normal * (sur->line.start.y - rect.top);				
				valid = true;
				return;
			}
			else if (normal.y < 0.f && rect.top + rect.height > prev.top + prev.height) {
				offset = normal * (rect.top + rect.height - sur->line.start.y);				
				valid = true;
				return;
			}
		}
		else {

			//TODO this shit

		}
	}


}







