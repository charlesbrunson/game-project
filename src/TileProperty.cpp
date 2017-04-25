#include "TileProperty.hpp"
#include "Log.hpp"

#include <fstream>
#include <string>
#include <assert.h>

#include "json.h"


std::map<std::string, std::map<GridVector, TileProperty::TileData>> TileProperty::tileDataMap;

//ResourceLoader *TileProperty::resLoader = nullptr;

TileProperty::TileData TileProperty::getTileData(std::string sheet, GridVector tilePos) {
	//if (resLoader != nullptr && resLoader->getTexture(sheet) != nullptr && resLoader->isTextureTileset(sheet)) {
		auto i = tileDataMap.find(sheet);
		if (i != tileDataMap.end()) {
			auto j = i->second.find(tilePos);
			if (j != i->second.end()) {
				return j->second;
			}
		}
	//}

	return TileData();
};


void TileProperty::initTileData() {
	//TileProperty::resLoader = rm;

	std::vector<std::string> tilesetNames;


	Log::msg("Loading tileset data");

	//open index and find tileset list
	std::ifstream iReader(RL()->fileIndex);
	if (iReader.is_open()) {

		Json::Value root;
		iReader >> root;

		Json::Value tilesets = root.get(RL()->fileTypes[ResourceLoader::TILESET], Json::nullValue);

		if (!tilesets.isNull()) {
			Json::Value files = tilesets.get("files", Json::arrayValue);

			for (auto i = files.begin(); i != files.end(); i++) {
				//std::cout << i->asString() << std::endl;
				tilesetNames.push_back(i->asString());
			}
		}

	}
	iReader.close();

	//read tileset property files
	std::ifstream propReader;
	for (auto tileset = tilesetNames.begin(); tileset != tilesetNames.end(); tileset++) {

		std::string tilesetDataPath = "data/tilesets/" + *tileset + ".txt";

		propReader.open(tilesetDataPath);
		if (propReader.is_open()) {
			int currentProp = tileProps::TILE_DEFAULT;
			bool firstElement = true;

			//parse json
			Json::Value root;
			propReader >> root;
			
			for (auto prop = root.begin(); prop != root.end(); prop++) {
				std::string type = prop->get("type","").asString();
				assert(type != "");

				int framerate = -1, framecount = -1;

				if (type == "nocollision") {
					currentProp = tileProps::TILE_NOCOLLISION;
				}
				else if (type == "oneway") {
					currentProp = tileProps::TILE_ONEWAY;
				}
				else if (type == "animated") {
					currentProp = tileProps::TILE_ANIMATED;

					framerate = prop->get("framerate", -1).asInt();
					framecount = prop->get("framecount", -1).asInt();
				}
				
				auto tileList = prop->get("tiles", 0);
				assert(tileList.isArray());

				//add tile data
				for (auto tileIndex = tileList.begin(); tileIndex != tileList.end(); tileIndex++) {
					TileData t;
					GridVector w;

					w.x = tileIndex->get("x", 0).asInt();
					w.y = tileIndex->get("y", 0).asInt();
					t.occluding = tileIndex->get("occluding", true).asBool();
					t.tileProperty = currentProp;

					if (currentProp == tileProps::TILE_ANIMATED) {
						assert(framerate > -1 && framecount > -1);
						t.animFrameRate = sf::milliseconds(framerate);
						t.animFrameCount = framecount;
					}

					if (firstElement) {
						firstElement = false;
						tileDataMap.insert(std::pair<std::string, std::map<GridVector, TileData>>(*tileset, std::map<GridVector, TileData>()));
					}
					tileDataMap.at(*tileset).insert(std::pair<GridVector, TileData>(w, t));
				}
			}
		}

		Log::msg("\t... done\n");
		propReader.close();
	}
}