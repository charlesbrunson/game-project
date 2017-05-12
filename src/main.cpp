
#if defined(_DEBUG) && defined(_WIN32) && 1
	//Visual Leak Detector
	#include <vld.h>
#endif

// Game class
#include "ResourceLoader.hpp"
//#include "Game.hpp"

int main() {
	
	// Start up the game loop
	{
		if (RL()->loadResources()) {
			//Game g;
			//g.run();
		}
		RL()->dumpResources();
	}

	return 0;
}
