#include "TextureFile.hpp"

#include <fstream>

#include "TileProperty.hpp"
#include "Log.hpp"
#include "json.h"

#include "StandardSize.hpp"

#include <assert.h>
#include <array>
#include <string.h>


TextureFile::TextureFile(std::string path, FileStream* str) : GameFile(path, str) {
	fileType = FileType::TEXTURE;
	load(path, str);
};

const sf::Texture& TextureFile::get() const {
	return tex;
}

Animation* TextureFile::getAnimation(std::string animName) {
	return &animations.at(animName);
}

bool TextureFile::loadFromFile(std::string path) {
	data.clear();
	animations.clear();

	if (tex.loadFromFile(RL()->fileDir + path)) {
		// check for associated files

		// animation file
		std::ifstream reader(RL()->fileDir + path + ".anim");
		if (reader.is_open()) {

			Json::Value root;
			reader >> root;

			animations.clear();

			for (auto anim = root.begin(); anim != root.end(); anim++) {


				//std::cout << anim.key().asString() << std::endl << a.write(*anim) << std::endl;
				std::string animName = anim.key().asString();

				Animation a;
				a.name = animName;
				bool err = false;

				// area
				Json::Value area = anim->get("area", Json::Value());
				if (area.isArray() && area.size() == 4 &&
					area[0].isInt() && area[1].isInt() && area[2].isInt() && area[3].isInt()) {
					a.area = sf::IntRect(area[0].asInt(), area[1].asInt(), area[2].asInt(), area[3].asInt());
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
				//int animCount;
				if (count.isInt())
					a.numOfFrames = count.asInt();
				else
					err = true;

				// framerate
				Json::Value frameRateMS = anim->get("framerateMS", Json::nullValue);
				if (frameRateMS.isNumeric() || frameRateMS.isArray()) {
					if (frameRateMS.isNumeric()) {

						float f = frameRateMS.asFloat();
						a.frameTimes.insert(a.frameTimes.begin(), a.numOfFrames, sf::seconds(f / 1000.f));
						//a.frameTimes = std::vector<sf::Time>(animCount, sf::seconds(frameRateMS.asFloat() / 1000.f));

					}
					else if (frameRateMS.isArray()) {

						for (auto time = frameRateMS.begin(); time != frameRateMS.end(); time++) {
							a.frameTimes.push_back(sf::seconds(time->asFloat()) / 1000.f);
						}
						a.numOfFrames = a.frameTimes.size();
					}
				}
				else {
					err = true;
				}

				// loop count
				Json::Value loop = anim->get("loop", 0);
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
					animations.insert(std::make_pair(animName, a));
				}
			}
			Log::msg("Created animation: " + filePath);
		}
		reader.close();

		//std::cout << animations.size() << std::endl;

		//tile data file
		reader.open(RL()->fileDir + path + ".tile");
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
			Log::msg("Created tile map: " + filePath);
			TileProperty::addTileMap(filePath, &tileData);
		}
		reader.close();

		return true;
	}
	return false;
}

bool TextureFile::loadFromStream(FileStream* str) {

	int texSize = 0;
	str->read(&texSize, StdSizes::intSize);

	std::ifstream* fstr = str->getSource();
	FileStream tStr(fstr, fstr->tellg(), (int)fstr->tellg() + texSize);

	if (tex.loadFromStream(tStr)) {
		animations.clear();

		str->seek(StdSizes::intSize + texSize);

		//load remaining data (animations)
		int animCount;
		str->read(&animCount, StdSizes::intSize);

		animations.clear();
		for (int i = 0; i < animCount; i++) {

			std::pair<std::string, Animation> a;

			//name
			int animNameSize;
			str->read(&animNameSize, StdSizes::intSize);
			char c;
			for (int j = 0; j < animNameSize; j++) {
				str->read(&c, StdSizes::charSize);
				a.first += c;
			}
			a.second.name = a.first;

			//area
			str->read(&a.second.area.left, StdSizes::intSize);
			str->read(&a.second.area.top, StdSizes::intSize);
			str->read(&a.second.area.width, StdSizes::intSize);
			str->read(&a.second.area.height, StdSizes::intSize);

			//origin
			str->read(&a.second.origin.x, StdSizes::floatSize);
			str->read(&a.second.origin.y, StdSizes::floatSize);

			//frames
			int frameCount;
			str->read(&frameCount, StdSizes::intSize);
			for (int j = 0; j < frameCount; j++) {
				float t;
				str->read(&t, StdSizes::floatSize);
				a.second.frameTimes.push_back(sf::seconds(t));
			}
			a.second.numOfFrames = a.second.frameTimes.size();

			//loop
			str->read(&a.second.loop, StdSizes::floatSize);

			//chain name
			int chainNameSize;
			str->read(&chainNameSize, StdSizes::intSize);
			//char c;
			for (int j = 0; j < chainNameSize; j++) {
				str->read(&c, StdSizes::charSize);
				a.second.chainToName += c;
			}

			//chain start frame
			str->read(&a.second.chainStartOnFrame, StdSizes::intSize);

			animations.insert(a);
		}
		linkAnimations();
		return true;
	}
	return false;
}

void TextureFile::convertToData() {

	std::stringstream out;

	// copy file to stream
	std::ifstream file(RL()->fileDir + filePath, std::ios::binary | std::ios::ate);

	if (file) {
		int size = file.tellg();
		out.write((char*)&size, StdSizes::intSize);
		file.seekg(0);

		out << file.rdbuf();
		file.close();
	}
	else {
		return;
	}

	// write animations
	int animCount = animations.size();
	out.write((char*)&animCount, StdSizes::intSize);
	for (const auto& a : animations) {
		const Animation& ani = a.second;

		//name
		int animNameSize = a.first.size();
		out.write((char*)&animNameSize, StdSizes::intSize);
		for (const char& c : a.first) {
			out.write((char*)&c, StdSizes::charSize);
		}

		//area
		out.write((char*)&ani.area.left, StdSizes::intSize);
		out.write((char*)&ani.area.top, StdSizes::intSize);
		out.write((char*)&ani.area.width, StdSizes::intSize);
		out.write((char*)&ani.area.height, StdSizes::intSize);

		//origin
		out.write((char*)&ani.origin.x, StdSizes::floatSize);
		out.write((char*)&ani.origin.y, StdSizes::floatSize);

		//frames
		int frameCount = ani.frameTimes.size();
		out.write((char*)&frameCount, StdSizes::intSize);
		for (int i = 0; i < frameCount; i++) {
			float t = ani.frameTimes.at(i).asSeconds();
			out.write((char*)&t, StdSizes::floatSize);
		}

		//loop
		out.write((char*)&ani.loop, StdSizes::floatSize);

		//chain name
		int chainSize = ani.chainToName.size();
		out.write((char*)&chainSize, StdSizes::intSize);
		for (const char& c : ani.chainToName) {
			out.write((char*)&c, StdSizes::charSize);
		}
		//chain start frame
		out.write((char*)&ani.chainStartOnFrame, StdSizes::intSize);
	}
	linkAnimations();

	// dont need to write tiledata

	//copy stringstream to data
	std::string dstr = out.str();
	data.append(dstr.c_str(), dstr.size());

}

void TextureFile::linkAnimations() {
	if (animations.size() < 2)
		return;

	for (auto a = animations.begin(); a != animations.end(); a++) {
		if (a->second.chainToName.empty() || a->second.chainTo)
			continue;

		for (auto b = animations.begin(); b != animations.end(); b++) {
			if (a == b)
				continue;

			if (a->second.chainToName == b->first) {
				a->second.chainTo = &b->second;
				break;
			}
		}
	}
}
