
#if _DEBUG && _WIN32 && 1
	//Visual Leak Detector
	#include <vld.h>
#endif

// Game class
#include "Game.hpp"

int main() {
	
	// Start up the game loop
	{
		Game g;
		g.run();
	}

	return 0;
}