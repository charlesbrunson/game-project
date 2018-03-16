#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "res/ResourceLoader.hpp"
#include "game/Zone.hpp"

class LevelLoader {
public:
	static bool compileTMXFile(std::string location);
	static void loadLevel(std::string name, Zone::LevelArea &area, Zone *zone);
	static void readLevel(std::ifstream &reader, Zone::LevelArea &area, Zone *zone);
	static void saveLevel(std::string name, Zone::LevelArea &area);
};

#endif
