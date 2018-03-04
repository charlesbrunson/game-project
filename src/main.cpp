
#if defined(_DEBUG) && defined(_WIN32) && 0
	//Visual Leak Detector
	#include <vld.h>
#endif

#include <string>

// Game class
#include "ResourceLoader.hpp"
#include "Engine.hpp"

int main() {
	
	// Start up the game loop
	{
		if (RL()->loadResources()) {
			Engine e;
			e.run();
		}
		RL()->dumpResources();
	}

	return EXIT_SUCCESS;
}
