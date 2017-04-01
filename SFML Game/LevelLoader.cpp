#include <assert.h>
#include <sstream>

#include "LevelLoader.hpp"
#include "rapidxml.hpp"

#include "Math.hpp"

void LevelLoader::loadLevel(sf::String name, Zone::LevelArea &area, ResourceLoader *loader, Zone *zone) {
	/*
	#ifdef _DEBUG
	//load from .lvl file

	//if (Gameplay_Globals::Debug::forceCompile) {
	//	Log::msg("Forcing recompile...\n");
	//	compileTMXFile(name, loader);
	//}


	std::ifstream stream((loader->fileDir + name).toAnsiString() + ".lvl", std::ios_base::binary);
	if (stream.is_open()) {
	Zone::readLevel(stream, area, loader, zone);
	}
	stream.close();

	#else */

	//load from pack archive
	std::ifstream stream(loader->packName, std::ios_base::binary);
	if (stream.is_open()) {
		Log::msg("Opening " + name + ".lvl\n");

		int seek = loader->getLevelOffset(name + ".lvl");

		assert(seek > -1);

		stream.seekg(loader->packHeaderSize + seek, std::ios_base::beg);
		LevelLoader::readLevel(stream, area, loader, zone);
	}
	else {
		Log::msg("Could not open " + name + ".lvl\n");
	}
	stream.close();

	//#endif
}

void LevelLoader::readLevel(std::ifstream &reader, Zone::LevelArea &area, ResourceLoader *loader, Zone *zone) {

	if (reader.is_open()) {

		//LEVEL---------------------------------
		if (area.level != nullptr)
			delete area.level;
		area.level = new Level(loader);
		LevelSerializer::readLevel(reader, *(area.level));
		Log::msg("Reading level: " + area.level->getLevelName() + ".lvl\n");

		//OBJECTS---------------------------------

		int objCount;
		reader.read((char*)&objCount, StdSizes::intSize);

		area.objects.clear();

		for (int i = 0; i < objCount; i++) {

			objNode obj;

			//name
			int nameLength;
			reader.read((char*)&nameLength, StdSizes::intSize);
			for (int j = 0; j < nameLength; j++) {
				char c;
				reader.read(&c, StdSizes::charSize);
				obj.name += c;
			}

			int typeLength;
			reader.read((char*)&typeLength, StdSizes::intSize);
			for (int j = 0; j < typeLength; j++) {
				char c;
				reader.read(&c, StdSizes::charSize);
				obj.type += c;
			}

			reader.read((char*)&obj.x, StdSizes::intSize);
			reader.read((char*)&obj.y, StdSizes::intSize);
			reader.read((char*)&obj.width, StdSizes::intSize);
			reader.read((char*)&obj.height, StdSizes::intSize);
			reader.read((char*)&obj.faceLeft, StdSizes::boolSize);
			reader.read((char*)&obj.isTemplate, StdSizes::boolSize);

			//args
			int argCount;
			reader.read((char*)&argCount, StdSizes::intSize);
			for (int j = 0; j < argCount; j++) {
				sf::String name;
				int nameLength;
				reader.read((char*)&nameLength, StdSizes::intSize);
				for (int k = 0; k < nameLength; k++) {
					char c;
					reader.read(&c, StdSizes::charSize);
					name += c;
				}

				sf::String arg;
				int argLength;
				reader.read((char*)&argLength, StdSizes::intSize);
				for (int k = 0; k < argLength; k++) {
					char c;
					reader.read(&c, StdSizes::charSize);
					arg += c;
				}

				obj.args.push_back(std::pair<sf::String, sf::String>(name, arg));
			}

			if (obj.type == "PLAYER") {
				area.level->playerStartPos = sf::Vector2f(obj.x, obj.y);
			}

			area.objects.push_back(obj);
		}
		area.level->setResources(loader);

		//Log::msg(name + ".lvl successfully loaded" + "\n\n");

	}
	else {

		//if can't find try to compile .tmx tile of same name
		//Log::msg("Couldn't open " + name + ".lvl\n");
		Log::msg("Attemping to compile " + area.level->getLevelName() + ".tmx\n");

		bool attempt = compileTMXFile(area.level->getLevelName(), loader);

		if (!attempt) {
			Log::msg("Failed to load level " + area.level->getLevelName() + ".lvl\n\n");
		}
	}
	reader.close();


	int i = zone->getTimesActive(area.level->getLevelName());
	if (i == -1) {
		zone->addActiveCounter(area.level->getLevelName());
	}
	zone->incrementActiveCount(area.level->getLevelName());
	
	area.levelName = area.level->getLevelName();
	//return area;
};

//------------------------------------------------------------------
//Writes LevelArea objects as .lvl files
//------------------------------------------------------------------

void LevelLoader::saveLevel(sf::String name, Zone::LevelArea &area) {

	Log::msg("Writing level to native format: " + name + ".lvl\n");

	ResourceLoader *temp = area.level->getResources();
	area.level->setResources(nullptr);

	std::ofstream writer;
	writer.open((std::string)area.level->getResources()->fileDir + (std::string)name + ".lvl", std::ios::binary | std::ios::trunc);

	if (writer.is_open()) {
		//LEVEL---------------------
		//write level to mem
		LevelSerializer::writeLevel(writer, *(area.level));

		//OBJECTS---------------------
		//write obj nodes to mem
		int size = (int)area.objects.size();
		writer.write((char*)&size, StdSizes::intSize);
		for (objNode n : area.objects) {

			//name
			int nameLength = n.name.getSize();
			writer.write((char*)&nameLength, StdSizes::intSize);

			for (char c : n.name) {
				writer.write(&c, StdSizes::charSize);

			}

			//type
			int typeLength = n.type.getSize();
			writer.write((char*)&typeLength, StdSizes::intSize);

			for (char c : n.type) {
				writer.write(&c, StdSizes::charSize);

			}

			writer.write((char*)&n.x, StdSizes::intSize);
			writer.write((char*)&n.y, StdSizes::intSize);
			writer.write((char*)&n.width, StdSizes::intSize);
			writer.write((char*)&n.height, StdSizes::intSize);
			writer.write((char*)&n.faceLeft, StdSizes::boolSize);
			writer.write((char*)&n.isTemplate, StdSizes::boolSize);

			//args
			int size = n.args.size();
			writer.write((char*)&size, StdSizes::intSize);
			for (std::pair<sf::String, sf::String> arg : n.args) {

				int nameLength = arg.first.getSize();
				writer.write((char*)&nameLength, StdSizes::intSize);
				for (char c : arg.first) {
					writer.write(&c, StdSizes::charSize);

				}

				int argLength = arg.second.getSize();
				writer.write((char*)&argLength, StdSizes::intSize);
				for (char c : arg.second) {
					writer.write(&c, StdSizes::charSize);

				}
			}
		}

	}

	area.level->setResources(temp);

	Log::msg("Writing complete\n\n");

	//yay!
	writer.close();
};

//------------------------------------------------------------------
//Compiles TMX files for use
//------------------------------------------------------------------

bool LevelLoader::compileTMXFile(sf::String name, ResourceLoader *loader) {

	Zone::LevelArea area;

	//open tmx file
	using namespace rapidxml;

	std::ifstream *file = new std::ifstream((std::string)loader->fileDir + (std::string)name + ".tmx");
	std::stringstream *buffer = new std::stringstream;
	xml_document<> *doc = new xml_document<>();

	if (!file->is_open()) {
		return false;
	}

	*buffer << file->rdbuf();
	file->close();
	delete file;

	std::string content(buffer->str());
	delete buffer;

	doc->parse<0>(&content[0]);

	//start reading it
	if (area.level != nullptr)
		delete area.level;
	area.level = new Level(loader);

	Log::msg("Reading TMX file: " + name + "\n");

	area.level->setLevelName(name);

	xml_node<> * rootNode = doc->first_node();

	//get background color
	sf::Color bg = sf::Color::Black;
	xml_attribute<> * bgNode = rootNode->first_attribute("backgroundcolor");
	if (bgNode != NULL) {
		std::string str = bgNode->value();

		bg.r = std::stoul("0x" + str.substr(1, 2), nullptr, 16);
		bg.g = std::stoul("0x" + str.substr(3, 2), nullptr, 16);
		bg.b = std::stoul("0x" + str.substr(5, 2), nullptr, 16);
	}
	area.level->setBGColor(bg);

	//determine bounds by height and width attributes
	int gridWidth = std::stoi(rootNode->first_attribute("width")->value());
	int gridHeight = std::stoi(rootNode->first_attribute("height")->value());

	area.level->setArea(0, 0, gridWidth * tileSpacing, gridHeight * tileSpacing);

	//custom properties
	xml_node<> * propertiesNode = rootNode->first_node("properties");
	if (propertiesNode != NULL) {
		for (xml_node<> * prop = propertiesNode->first_node("property"); prop; prop = prop->next_sibling("property")) {
			std::string name = prop->first_attribute("name")->value();
			std::string value = prop->first_attribute("value")->value();
			if (name == "INVISIBLEWALLS") {
				area.level->setInvisibleWall(0, std::stoi(value.substr(0, 1)) != 0);
				area.level->setInvisibleWall(1, std::stoi(value.substr(2, 3)) != 0);
				area.level->setInvisibleWall(2, std::stoi(value.substr(4, 5)) != 0);
				area.level->setInvisibleWall(3, std::stoi(value.substr(6, 7)) != 0);
			}
			//override autogenerated levelsize
			else if (name == "LEVELSIZE") {
				int space = value.find(' ');
				int x = std::stoi(value.substr(0, space));
				int y = std::stoi(value.substr(space + 1, value.size()));
				area.level->setArea(0, 0, x * tileSpacing, y * tileSpacing);
			}
			else if (name == "BACKGROUND") {
				area.level->setBGSpriteName(value);
			}
		}
	}
	Log::msg("Level dimensions: " + std::to_string(area.level->levelArea.width) + " by " + std::to_string(area.level->levelArea.height) + "\n");

	//get tilesets used and store them temporarily

	struct tileset {
		int firstgid;
		int lastgid;
		sf::String imageName;
		int sprite;
		int width;
		int height;
	};
	std::list<tileset> tilesetList;

	Log::msg("\n Required tilesets:\n");

	tileset t;

	for (xml_node<> * tilesetNode = rootNode->first_node("tileset"); tilesetNode; tilesetNode = tilesetNode->next_sibling("tileset")) {

		t.firstgid = std::stoi(tilesetNode->first_attribute("firstgid")->value());


		xml_node<> * image = tilesetNode->first_node("image");

		t.imageName = image->first_attribute("source")->value();
		t.imageName.erase(0, std::string("tilesets/").length());

		t.width = std::stoi(image->first_attribute("width")->value());
		t.height = std::stoi(image->first_attribute("height")->value());

		t.lastgid = t.firstgid + (floor(t.width / tileSpacing) * floor(t.height / tileSpacing)) - 1;
		
		tilesetList.push_front(t);
		Log::msg(t.imageName + "\n");

	}
	Log::msg("\n");

	//populate layers
	area.level->getParallaxLayers()->clear();
	area.level->getBackgroundTiles()->clear();
	area.level->getForegroundTiles()->clear();
	area.level->getDecorativeTiles()->clear();

	//for each layer
	for (xml_node<> * layer = rootNode->first_node("layer"); layer; layer = layer->next_sibling("layer")) {

		enum Layer : int {
			NONE,
			FOREGROUND,
			BACKGROUND,
			PARALLAX,
			DECORATIVE
		};

		Layer cLayer = Layer::NONE;
		int parallax = -1;

		std::string name = layer->first_attribute("name")->value();
		Log::msg("Building Layer: " + name + "\n");

		if (name == "Foreground") {
			cLayer = Layer::FOREGROUND;
		}
		else if (name == "Background") {
			cLayer = Layer::BACKGROUND;
		}
		else if (name == "Parallax") {
			cLayer = Layer::PARALLAX;
			sf::Vector2i size;
			std::string name = layer->first_attribute("name")->value();
			sf::Vector2f scroll;
			sf::Vector2i scrollSize;
			sf::Vector2f parallaxRate;

			for (xml_node<> * prop = layer->first_node("properties")->first_node("property"); prop; prop = prop->next_sibling("property")) {
				std::string name = prop->first_attribute("name")->value();
				std::string value = prop->first_attribute("value")->value();
				if (name == "SIZE") {
					int space = value.find(' ');
					size.x = stoi(value.substr(0, space));
					size.y = stoi(value.substr(space + 1, value.size()));
				}
				else if (name == "LAYER") {
					parallax = std::stoi(value);
				}
				else if (name == "AUTOSCROLL") {
					int space = value.find(' ');
					scroll = sf::Vector2f(stoi(value.substr(0, space)), stoi(value.substr(space + 1, value.size())));
				}
				else if (name == "SCROLLSIZE") {
					int space = value.find(' ');
					scrollSize = sf::Vector2i(stoi(value.substr(0, space)), stoi(value.substr(space + 1, value.size())));
				}
				else if (name == "RATE") {
					int space = value.find(' ');
					parallaxRate = sf::Vector2f(stof(value.substr(0, space)), stof(value.substr(space + 1, value.size())));
				}
			}
			TileLayer p(loader, area.level->getTilesetList());
			area.level->getParallaxLayers()->insert(std::pair<int, TileLayer>(parallax, p));
			TileLayer *ip = &area.level->getParallaxLayers()->at(parallax);

			ip->parallax.isParallax = size != sf::Vector2i(area.level->levelArea.width / tileSpacing, area.level->levelArea.height / tileSpacing);
			ip->parallax.area = sf::FloatRect(0.f, 0.f, size.x, size.y);
			ip->parallax.scrollSpeed = scroll;
			ip->parallax.scrollSize = scrollSize == sf::Vector2i() ? size : scrollSize;
			ip->parallax.parallaxRate = parallaxRate;

		}
		else if (name == "Decorative") {
			cLayer = Layer::DECORATIVE;
		}

		xml_node<> * layerData = layer->first_node("data");

		sf::Vector2i pos(0, 0);

		//for each tile in layer data
		for (xml_node<> * tileData = layerData->first_node("tile"); tileData; tileData = tileData->next_sibling("tile")) {
			int gid = std::stoi(tileData->first_attribute("gid")->value());

			if (gid > 0) {
				int targetTileset = -1;
				sf::Vector2i sprPos(0, 0);

				for (std::list<tileset>::iterator ts = tilesetList.begin(); ts != tilesetList.end(); ts++) {
					if (ts->firstgid <= gid && gid <= ts->lastgid) {
						int localgid = gid - ts->firstgid;


						int h = localgid;
						while (h >= (ts->width / tileSpacing)) {
							h -= (ts->width / tileSpacing);
							sprPos.y++;
						}
						sprPos.x = localgid % (ts->width / tileSpacing);

						bool f = false;
						for (auto z = area.level->getTilesetList()->begin(); z != area.level->getTilesetList()->end(); z++) {
							if (z->second == ts->imageName) {
								ts->sprite = z->first;
								f = true;
								break;
							}
						}
						if (!f) {
							ts->sprite = area.level->getTilesetList()->size();
							area.level->getTilesetList()->insert(std::make_pair(ts->sprite, ts->imageName));
						}

						targetTileset = ts->sprite;
						break;
					}
				}
				
				switch (cLayer) {
				case Layer::DECORATIVE: area.level->getDecorativeLayer()->setTile(pos, targetTileset, sprPos); break;
				case Layer::FOREGROUND: area.level->getForegroundLayer()->setTile(pos, targetTileset, sprPos); break;
				case Layer::BACKGROUND: area.level->getBackgroundLayer()->setTile(pos, targetTileset, sprPos); break;
				case Layer::PARALLAX: area.level->getParallaxLayers()->at(parallax).setTile(pos, targetTileset, sprPos); break;
				}
			}
			pos.x += 1;
			if (pos.x >= (gridWidth)){
				pos.x = 0;
				pos.y += 1;
			}
		}
	}

	Log::msg("\nReading object data...\n");

	//read and generate objects
	xml_node<> * objData = rootNode->first_node("objectgroup");
	std::vector<objNode> objList;
	do {

		std::stringstream ss(objData->first_attribute(0)->value());
		std::string groupName;
		ss >> groupName;

		Log::msg("Reading object group: " + groupName + "\n");

		if (groupName == "Entities") {

			//read objects
			for (xml_node<> * object = objData->first_node("object"); object; object = object->next_sibling("object")) {
				objNode objN;

				if (object->first_attribute("name") != 0)
					objN.name = object->first_attribute("name")->value();

				objN.type = object->first_attribute("type")->value();

				Log::msg(objN.type + "\n");

				objN.width = std::stoi(object->first_attribute("width")->value());
				objN.height = std::stoi(object->first_attribute("height")->value());
				objN.x = std::stoi(object->first_attribute("x")->value()) + objN.width / 2;
				objN.y = std::stoi(object->first_attribute("y")->value()) + objN.height;

				bool hasArgs = object->first_node("properties") != 0;
				if (hasArgs) {
					for (xml_node<> * prop = object->first_node("properties")->first_node("property"); prop; prop = prop->next_sibling("property")) {
						std::string name = prop->first_attribute("name")->value();
						Log::msg("\t" + name + " " + prop->first_attribute("value")->value() + "\n");

						if (name == "FACELEFT") {
							objN.faceLeft = std::stoi(prop->first_attribute("value")->value()) != 0;
						}
						else if (name == "TEMPLATE") {
							objN.isTemplate = true;
						}
						else {
							objN.args.push_back(std::pair<sf::String, sf::String>(name, prop->first_attribute("value")->value()));
						}

					}
				}

				area.objects.push_back(objN);
			}
		}
		else if (groupName == "Transitions") {
			for (xml_node<> * t = objData->first_node("object"); t; t = t->next_sibling("object")) {
				Transition trans;
				trans.box.left = std::stoi(t->first_attribute("x")->value());
				trans.box.top = std::stoi(t->first_attribute("y")->value());

				trans.box.width = std::stoi(t->first_attribute("width")->value());
				trans.box.height = std::stoi(t->first_attribute("height")->value());

				//get levelname
				trans.levelName = t->first_attribute("type")->value();

				//get side, offset
				for (xml_node<> * prop = t->first_node("properties")->first_node("property"); prop; prop = prop->next_sibling("property")) {
					std::string name = prop->first_attribute("name")->value();
					
					if (name == "DIRECTION") {
						std::string d = prop->first_attribute("value")->value();
						if (d == "NORTH") {
							trans.dir = Cardinal::NORTH;
						}
						else if (d == "EAST") {
							trans.dir = Cardinal::EAST;
						}
						else if (d == "SOUTH") {
							trans.dir = Cardinal::SOUTH;
						}
						else if (d == "WEST") {
							trans.dir = Cardinal::WEST;
						}
					}
					else if (name == "OFFSET") {
						trans.offset = std::stoi(prop->first_attribute("value")->value());
					}
				}
				assert(trans.dir != Cardinal::NO_DIR);
				area.level->getLevelTransitions()->push_back(trans);
			}
		}
		else if (groupName == "Triggers") {
			for (xml_node<> * t = objData->first_node("object"); t; t = t->next_sibling("object")) {
				Level::Trigger trigger;

				trigger.area.left = std::stoi(t->first_attribute("x")->value());
				trigger.area.top = std::stoi(t->first_attribute("y")->value());

				trigger.area.width = std::stoi(t->first_attribute("width")->value());
				trigger.area.height = std::stoi(t->first_attribute("height")->value());

				//bool correct = false;
				sf::String type = t->first_attribute("type")->value();
				if (type == "DAMAGE") {
					trigger.type = Level::TriggerType::DAMAGE;
				}
				else if (type == "PUSH_LEFT") {
					trigger.type = Level::TriggerType::PUSH_LEFT;
				}
				else if (type == "PUSH_RIGHT") {
					trigger.type = Level::TriggerType::PUSH_RIGHT;
				}

				if (trigger.type != Level::TriggerType::NULL_TRIGGER) {
					area.level->getTriggers()->push_back(trigger);
				}
			}
		}
		objData = objData->next_sibling("objectgroup");

	} while (objData != 0);

	doc->clear();
	delete doc;

	//generate collision map
	Log::msg("Building collision map\n");
	area.level->generateCollisionMap();

	Log::msg("Level successfully compiled\n\n");

	LevelLoader::saveLevel(name, area);

	area.objects.clear();

	delete area.level;

	return true;
};