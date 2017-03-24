#ifndef LEVELLIST_H
#define LEVELLIST_H

#include <SFML/System.hpp>
#include <vector>


namespace LevelDef {

	const static int levelCount = 1;

	//each vector in the array is a level
	//first element in vector is the start point of level
	//subsequent elements are checkpoint levels
	static std::vector<sf::String> checkpoints[levelCount];
	static bool levelDefsInitialized = false;

	static void initLevelDefs() {
		if (LevelDef::levelDefsInitialized)
			return;

		//checkpoints
		LevelDef::checkpoints[0].push_back("test2");
		LevelDef::levelDefsInitialized = true;
	}

}

#endif