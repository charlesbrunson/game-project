
#include <SFML/Graphics.hpp>

#include "phys/SurfaceCollision.hpp"
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


	struct colFlags {
		bool collisionUp    = false;
		bool collisionDown  = false;
		bool collisionLeft  = false;
		bool collisionRight = false;
	};

	std::vector<std::pair<Surface*, colFlags>> curCollisions;

	bool clingGround = false;
	bool clingCeil   = false;

	//collision flags
	colFlags curFlags;
};

