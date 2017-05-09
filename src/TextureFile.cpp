#include "TextureFile.hpp"

#include <fstream>

#include "TileProperty.hpp"
#include "Log.hpp"
#include "json.h"

#include <assert.h>

bool TextureFile::in_loadFromFile(std::string path) {
	std::ifstream reader;
	if (tex.loadFromFile(path)) {
		//check for associated files

		//animation file
		reader.open(path + ".anim");
		if (reader.is_open()) {

			//TODO
			Json::Value root;
			reader >> root;

			for (auto anim = root.begin(); anim != root.end(); anim++) {
				std::string animName = anim->asString();

				Animation a;
				bool err = false;

				// area
				Json::Value area = anim->get("area", Json::Value());
				if (area.isArray() && area.size() == 4 && 
					area[0].isInt() && area[1].isInt() && area[2].isInt() && area[3].isInt()) {
					a.area = area[0].asInt;
				}
				else if (area.isNull()) {
					a.area = sf::IntRect();
				}
				else {
					err = true;
				}

				// origin
				Json::Value origin = anim->get("origin", Json::Value());
				if (origin.isArray() && origin.size() == 2 &&
					origin[0].isConvertibleTo(Json::realValue) && origin[1].isConvertibleTo(Json::realValue)) {

					a.origin = sf::Vector2f(origin[0].asFloat(), origin[1].asFloat());
				}
				else if (origin.isNull()) {
					a.origin = sf::Vector2f();
				}
				else {
					err = true;
				}

				// frame count
				Json::Value count = anim->get("count", 0);
				int animCount;
				if (count.isInt())
					animCount = count.asInt();
				else
					err = true;

				// framerate
				Json::Value frameRateMS = anim->get("frameRateMS", Json::nullValue);
				if (frameRateMS.isConvertibleTo(Json::realValue) || frameRateMS.isArray()) {
					if (frameRateMS.isConvertibleTo(Json::realValue)) {

						a.frameTimes = std::vector<sf::Time>(animCount, sf::seconds(frameRateMS.asFloat() / 1000.f));

					}
					else if (frameRateMS.isArray()) {

						for (auto time = frameRateMS.begin(); time != frameRateMS.end(); time++) {
							a.frameTimes.push_back(sf::seconds(time->asFloat()) / 1000.f);
						}

					}
				}
				else {
					err = true;
				}

				// loop count
				Json::Value loop = anim->get("loop", -1);
				if (loop.isInt())
					a.loop = loop.asInt();
				else
					err = true;

				// chain animation name
				Json::Value chain = anim->get("chainTo", "");
				if (chain.isString())
					a.chainToName = chain.asString();
				else
					err = true;

				// chain start frame
				Json::Value chainToStartFrame = anim->get("chainToStartFrame", 0);
				if (chainToStartFrame.isInt())
					a.chainStartOnFrame = chainToStartFrame.asInt();
				else
					err = true;


				if (err) {
					Log::msg("TextureFile: in_loadFromFile(), " + animName + ", error: Incorrect arguments");
				}
				else {
					animations[animName] = a;
				}
			}
		}
		reader.close();

		//tile data file
		reader.open(path + ".tile");
		if (reader.is_open()) {

			std::map<GridVector, TileProperty::TileData> tileData;

			int currentProp = TileProperty::tileProps::TILE_DEFAULT;

			//parse json
			Json::Value root;
			reader >> root;

			for (auto prop = root.begin(); prop != root.end(); prop++) {
				std::string type = prop->get("type", "").asString();
				assert(type != "");

				int framerate = -1, framecount = -1;

				if (type == "nocollision") {
					currentProp = TileProperty::tileProps::TILE_NOCOLLISION;
				}
				else if (type == "oneway") {
					currentProp = TileProperty::tileProps::TILE_ONEWAY;
				}
				else if (type == "animated") {
					currentProp = TileProperty::tileProps::TILE_ANIMATED;

					framerate = prop->get("framerate", -1).asInt();
					framecount = prop->get("framecount", -1).asInt();
				}

				auto tileList = prop->get("tiles", 0);
				assert(tileList.isArray());

				//add tile data
				for (auto tileIndex = tileList.begin(); tileIndex != tileList.end(); tileIndex++) {
					TileProperty::TileData t;
					GridVector w;

					w.x = tileIndex->get("x", 0).asInt();
					w.y = tileIndex->get("y", 0).asInt();
					t.occluding = tileIndex->get("occluding", true).asBool();
					t.tileProperty = currentProp;

					if (currentProp == TileProperty::tileProps::TILE_ANIMATED) {
						assert(framerate > -1 && framecount > -1);
						t.animFrameRate = sf::milliseconds(framerate);
						t.animFrameCount = framecount;
					}
					tileData[w] = t;
				}
			}
			TileProperty::addTileMap(filePath, tileData);
		}
		reader.close();

		return true;
	}
	return false;
}

bool TextureFile::in_loadFromStream(std::ifstream& str) {


	return false;
}

void TextureFile::convertToData() {



}