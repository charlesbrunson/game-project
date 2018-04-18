
#include <SFML/Graphics.hpp>

#include "phys/Collision.hpp"
#include "util/Log.hpp"

class Solid : public sf::Rect<float> {
public:

	Solid& operator= (const sf::FloatRect& ff) {
		left = ff.left;
		top = ff.top;
		width = ff.width;
		height = ff.height;
		return *this;
	}

	Vec2 getFloorUnit();
	Vec2 getCeilUnit();
	void clearFlags();


	struct ColFlags {
		bool collisionUp    = false;
		bool collisionDown  = false;
		bool collisionLeft  = false;
		bool collisionRight = false;
	};

	struct SolidCollision {
		//collision in question
		std::unique_ptr<Collision> collision;
		// collision direction flags tripped by this collision
		ColFlags flags;
	};

	std::vector<SolidCollision> curCollisions;

	bool clingGround = false;
	bool clingCeil   = false;

	//collision flags
	ColFlags curFlags;
};

