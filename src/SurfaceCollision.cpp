#include "phys/SurfaceCollision.hpp"

#include "util/Log.hpp"

#include "obj/GameObject.hpp"
#include <cfloat>

SurfaceCollision::SurfaceCollision(Surface* ss, GameObject* object, GridVec2 gridpos) : sur(ss), obj(object), place(gridpos) {
	eval();
};

void SurfaceCollision::eval() {
	valid = false;
	//offset = Vec2();

	if (sur == nullptr || obj == nullptr)
		return;

	Vec2 normal = sur->line.getLeftHandUnit();
	sf::FloatRect rect = obj->getCollision();
	sf::FloatRect prev = obj->getPrevFrameCollision();

	Vec2 oPos = obj->getPosition();

	bool goingUp    = rect.top < prev.top;
	bool goingLeft  = rect.left < prev.left;
	bool goingDown  = rect.top + rect.height > prev.top + prev.height;
	bool goingRight = rect.left + rect.width > prev.left + prev.width;

	bool isDiag = !sur->line.isVertical() && !sur->line.isHorizontal();

	//surface is orthogonal
	if (sur->line.isVertical() /*|| sur->line.isHorizontal()*/) {
		if (Math::intersects(sur->line, rect) && !Math::intersects(sur->line, prev, false)) {
			
			if (sur->line.isVertical()) {
				if (normal.x > 0.f && goingLeft) {
					newPos = obj->getPosition() + (normal * (sur->line.start.x - rect.left));
					valid = true;
					return;
				}
				else if (normal.x < 0.f && goingRight) {
					newPos = obj->getPosition() + (normal * (rect.left + rect.width - sur->line.start.x));
					valid = true;
					return;
				}
			}
			else if (sur->line.isHorizontal()) {
				if (normal.y > 0.f && goingUp) {
					newPos = obj->getPosition() + (normal * (sur->line.start.y - rect.top));
					valid = true;
					return;
				}
				else if (normal.y < 0.f && goingDown) {
					newPos = obj->getPosition() + (normal * (rect.top + rect.height - sur->line.start.y));
					valid = true;
					return;
				}
			}
		}
	}
	//surface is diagonal
	//we can assume normal.x != 0 and normal y != 0
	else {
		bool extendLeft = normal.y < 0.f ? sur->startConn == nullptr : sur->endConn == nullptr;
		bool extendRight = normal.y > 0.f ? sur->startConn == nullptr : sur->endConn == nullptr;

		float topB = std::min(sur->line.start.y, sur->line.end.y);
		float botB = std::max(sur->line.start.y, sur->line.end.y);
		float leftB = std::min(sur->line.start.x, sur->line.end.x);
		float rightB = std::max(sur->line.start.x, sur->line.end.x);
		
		Line centerVert(Point(oPos.x, rect.top), Point(oPos.x, rect.top + rect.height));
		Line prevVert(  Point(Math::center(prev).x, prev.top), Point(Math::center(prev).x, prev.top + prev.height));
	
		if (centerVert.start.x < leftB - (extendLeft ? rect.width : 0.f) ||
			centerVert.start.x > rightB + (extendRight ? rect.width : 0.f))
			return;


		//check box movement from side facing surface
		Vec2 moveVec;
		moveVec.x = normal.x > 0.f ? rect.left - prev.left : (rect.left + rect.width) - (prev.left + prev.width);
		moveVec.y = normal.y > 0.f ? rect.top - prev.top : (rect.top + rect.height) - (prev.top + prev.height);
		
		//check if movement is facing normal
		//if (Math::dotProd(normal, moveVec) < 0.f) {
			//collision is possible, time for the nitty gritty

			bool prevNoIntersect = false;
			bool hasIntersect = false;

			bool useFlat = false;
			float flatY = 0.f;

			if (isDiag) {
				if (extendLeft && centerVert.start.x < leftB) {
					if (normal.x > 0.f) {
						useFlat = true;
						flatY = normal.y < 0.f ? topB : botB;
					}
				}
				else if (extendRight && centerVert.start.x > rightB) {
					if (normal.x < 0.f) {
						useFlat = true;
						flatY = normal.y < 0.f ? topB : botB;
					}
				}
			}

			if (!useFlat) {
				//check prev is on the right side, add some leniance when dealing with slopes
				prevNoIntersect = 
					(Math::pointOnLine(sur->line, prevVert.start + Vec2(0.f, normal.y > 0.f ?  1.f : 0.f)) >= 0.f ==
					 Math::pointOnLine(sur->line, prevVert.end   + Vec2(0.f, normal.y < 0.f ? -1.f : 0.f)) >= 0.f);

				//check current is intersecting
				hasIntersect = 
					(Math::pointOnLine(sur->line, centerVert.start) >= 0.f !=
					 Math::pointOnLine(sur->line, centerVert.end)   >= 0.f );

				if (prevNoIntersect && hasIntersect) {
					//its a hit

					valid = Math::dotProd(normal, moveVec) < 0.f;
					if (!valid)
						return;

					//going straight down/up means vertical response
					/*if (obj->getVelocity().x == 0.f && centerVert.start.x == prevVert.start.x) {

						float perc  = (centerVert.start.x - leftB) / (rightB - leftB);
						if (normal.y > 0.f)
							perc = 1.f - perc;

						newPos = (perc * sur->line.getVector()) + sur->line.start;

						if (normal.y > 0.f) {
							newPos.y += rect.height;
						}

					}
					else {*/
						//use projection
						newPos = Math::projection((normal.y > 0.f ? centerVert.start : centerVert.end) - sur->line.start, sur->line.getUnit(), true) + sur->line.start;		
						if (normal.y > 0.f) {
							newPos += Vec2(0.f, rect.height);
						}
					//}
					//cancel out if the vector is in the wrong direction due to slope leniance
					if (newPos - obj->getPosition() == Vec2(0.f, 0.f) || Math::dotProd(newPos - obj->getPosition(), normal) <= 0.f)
						valid = false;

				}
			}
			else {
				prevNoIntersect = (prevVert.start.y + (normal.y > 0.f ?  1.f : 0.f) <= flatY) ==
								  (prevVert.end.y   + (normal.y < 0.f ? -1.f : 0.f) <= flatY);

				hasIntersect = (centerVert.start.y <= flatY) != (centerVert.end.y <= flatY);

				if (prevNoIntersect && hasIntersect) {
					valid = Math::dotProd(normal.y < 0.f ? Vec2(0.f, -1.f) : Vec2(0.f, 1.f), moveVec) < 0.f;
					newPos = obj->getPosition();
					newPos.y = flatY;
					if (normal.y > 0.f) {
						newPos += Vec2(0.f, rect.height);
					}
				}
			}
		//}
	}
}







