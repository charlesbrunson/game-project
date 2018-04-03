#ifndef TILEPROPERTY_H
#define TILEPROPERTY_H

#include <SFML/System.hpp>
#include "res/ResourceLoader.hpp"
#include "util/GridVector.hpp"
#include "util/Math.hpp"

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

		void setShape(std::string shapeType, bool hFlip, int rotate);

		std::vector<Point> shape = {
			Point(0.f, 0.f),
			Point(1.f, 0.f),
			Point(1.f, 1.f),
			Point(0.f, 1.f)
		};
	};

	static const TileData getTileData(std::string sheet, GridVector tilePos);

	static void addTileMap(std::string texName, const std::map<GridVector, TileData>& data);

private:
	static std::map<std::string, std::map<GridVector, TileData>> tileDataMap;
	//static ResourceLoader *resLoader;

};


#endif

