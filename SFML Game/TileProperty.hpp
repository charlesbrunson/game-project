#ifndef TILEPROPERTY_H
#define TILEPROPERTY_H

#include <SFML/System.hpp>
#include "ResourceLoader.hpp"
#include "GridVector.hpp"

#include <map>

class TileProperty {
public:
	enum tileProps : int {
		TILE_DEFAULT = 0,
		TILE_ONEWAY,
		TILE_ANIMATED,
		TILE_NOCOLLISION
	};

	struct TileData {
		int tileProperty = TILE_DEFAULT;
		bool occluding = true;
		sf::Time animFrameRate = sf::Time::Zero;
		int animFrameCount = 0;
	};

	static TileData getTileData(std::string sheet, GridVector tilePos);

	static void initTileData(ResourceLoader *rm);

private:
	static std::map<std::string, std::map<GridVector, TileData>> tileDataMap;
	static ResourceLoader *resLoader;

};


#endif