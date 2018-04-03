#include "game/lvl/TileProperty.hpp"

#include "json.h"

#include "util/Log.hpp"

//#include <fstream>
//#include <string>
#include <assert.h>

std::map<std::string, std::map<GridVector, TileProperty::TileData>> TileProperty::tileDataMap;

void TileProperty::TileData::setShape(std::string shapeType, bool hFlip, int rotate) {
	shape.clear();

	if (shapeType == "square") {
		shape = {
			Point(0.f, 0.f),
			Point(1.f, 0.f),
			Point(1.f, 1.f),
			Point(0.f, 1.f)
		};
	}
	else if (shapeType == "slope") {
		shape = {
			Point(0.f, 1.f),
			Point(1.f, 0.f),
			Point(1.f, 1.f)
		};
	}
	else if (shapeType == "slopelow") {
		shape = {
			Point(0.f, 1.f),
			Point(1.f, 0.5f),
			Point(1.f, 1.f)
		};
	}
	else if (shapeType == "slopehigh") {
		shape = {
			Point(0.f, 0.5f),
			Point(1.f, 0.f),
			Point(1.f, 1.f),
			Point(0.f, 1.f)
		};
	}
	else if (shapeType == "short") {
		shape = {
			Point(0.f, 0.5f),
			Point(1.f, 0.5f),
			Point(1.f, 1.f),
			Point(0.f, 1.f)
		};
	}
	else {
		Log::msg("ERROR: Unknown tile shape: " + shapeType);
		shape = {
			Point(0.f, 0.f),
			Point(1.f, 0.f),
			Point(1.f, 1.f),
			Point(0.f, 1.f)
		};
	}

	if (hFlip) {
		for (auto i = shape.begin(); i != shape.end(); i++) {
			i->x *= -1.f;
			i->x += 1.f;
		}
		std::reverse(shape.begin(), shape.end());
	}

	if (rotate > 0) {
		for (auto i = shape.begin(); i != shape.end(); i++) {
			for (int r = 0; r < rotate; r++) {
				Point p;
				p.x = (i->y) * -1.f;
				p.y = i->x;
				*i = p + Vec2(1.f, 0);
			}
		}
	}

}

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
	
