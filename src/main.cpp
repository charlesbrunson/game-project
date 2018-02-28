
#if defined(_DEBUG) && defined(_WIN32) && 0
	//Visual Leak Detector
	#include <vld.h>
#endif

#include <string>

// Game class
#include "ResourceLoader.hpp"
#include "Game.hpp"
#include "Configuration.hpp"
#include "Log.hpp"

int main(int argc, char* argv[]) {
	

	if (argc >= 1) {
		Config::setExecPath(argv[0]);
		Log::msg("Exec location: " + std::string(argv[0]));
	}
	

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
