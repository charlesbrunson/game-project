
#if defined(_DEBUG) && defined(_WIN32) && 0
	//Visual Leak Detector
	#include <vld.h>
#endif

// Game class
//#include "Game.hpp"
#include "ResourceLoader.hpp"

int main() {
	
	// Start up the game loop
	{
		RL()->loadResources();
		//Game g;
		//g.run();
		RL()->dumpResources();
	}

	return 0;
}
