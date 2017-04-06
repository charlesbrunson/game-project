
#if _DEBUG && _WIN32 && 1
	//Visual Leak Detector
	#include <vld.h>
#endif

// Game class
#include "Game.hpp"

int main() {
	
	// Start up the game loop
	{
		ResourceLoader::get()->loadResources();
		{
			Game g;
			g.run();
		}
		ResourceLoader::get()->dumpResources();
	}

	return 0;
}
