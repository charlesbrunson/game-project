#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "ResourceLoader.hpp"
#include "Zone.hpp"

class LevelLoader {
public:
	static bool compileTMXFile(sf::String location, ResourceLoader *loader);
	static void loadLevel(sf::String name, Zone::LevelArea &area, ResourceLoader *loader, Zone *zone);
	static void readLevel(std::ifstream &reader, Zone::LevelArea &area, ResourceLoader *loader, Zone *zone);
	static void saveLevel(sf::String name, Zone::LevelArea &area);
};

#endif