
#if defined(_DEBUG) && defined(_WIN32) && 0
	//Visual Leak Detector
	#include <vld.h>
#endif

#include <string>

// Game class
#include "ResourceLoader.hpp"
#include "Engine.hpp"

#include "CollisionManager.hpp"

int main() {
	
	// Start up the game loop
	{
		if (RL()->loadResources()) {
			Engine e;
			e.run();
		}
		RL()->dumpResources();
	}

//	CollisionManager man(sf::Vector2i(16,16));
//	man.createStatic(sf::FloatRect(0,16,16,16), Static::CollisionType::FLOOR);
//	man.createStatic(sf::FloatRect(0,16,16,16), Static::CollisionType::RIGHTWALL);
//	Solid* solid = man.createSolid(sf::FloatRect(0,0,16,16));
//	solid->setArea(sf::FloatRect(0, 5, 16, 16));
//	man.update(sf::Time::Zero);
	
	return EXIT_SUCCESS;
}
