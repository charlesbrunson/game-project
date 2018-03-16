#include "game/lvl/TileProperty.hpp"

#include "json.h"

#include "util/Log.hpp"

//#include <fstream>
//#include <string>
#include <assert.h>

std::map<std::string, std::map<GridVector, TileProperty::TileData>> TileProperty::tileDataMap;

const TileProperty::TileData TileProperty::getTileData(std::string sheet, GridVector tilePos) {
	auto i = tileDataMap.find(sheet);
	if (i != tileDataMap.end()) {
		auto j = i->second.find(tilePos);
		if (j != i->second.end()) {
			return j->second;
		}
	}

	return TileData();
}

void TileProperty::addTileMap(std::string texName, const std::map<GridVector, TileData>& data) {
	tileDataMap[texName] = data;
}
	
