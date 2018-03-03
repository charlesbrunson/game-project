
#if defined(_DEBUG) && defined(_WIN32) && 0
	//Visual Leak Detector
	#include <vld.h>
#endif

#include <string>

// Game class
#include "ResourceLoader.hpp"
#include "Game.hpp"
#include "Log.hpp"

int main(int /*argc*/, char** /*argv*/) {
	
	// Start up the game loop
	{
		if (RL()->loadResources()) {
			Game g;
			g.run();
		}
		RL()->dumpResources();
	}

	return EXIT_SUCCESS;
}
