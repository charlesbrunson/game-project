#ifndef GLOBALS_H
#define GLOBALS_H

#include <SFML/System.hpp>
#include <iostream>

//#define GAMEWIDTH 240
//#define GAMEHEIGHT 180

extern const int GAMEWIDTH;
extern const int GAMEHEIGHT;
extern const sf::Vector2f GAMEDIMENSIONS;


extern int volumeGame;
extern int volumeMusic; 
extern float volumeMusicPausedScale;

static sf::Mutex drawLock;

//extern bool forcePackResources;
extern bool pixelSnapping;

namespace Gameplay_Globals {
	extern sf::Time minSoundTime;
	extern sf::Time soundTimeout;
	extern int gameScale;

	namespace Debug {
		extern bool objectCollision;
		extern bool levelCollision;
		extern bool platformPathing;
		extern bool forceCompile;
		extern bool playerInvulnerable;
		extern bool scrollingParallax;
		extern bool noZoom;
	}
}
namespace Resource_Globals {
	extern bool ignorePackFile;
}
namespace UI_Globals {
	namespace Debug {
		extern bool UIDebug;
		extern bool UIConnectors;
	}
}

#endif

