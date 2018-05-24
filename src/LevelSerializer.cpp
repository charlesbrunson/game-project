#include "game/lvl/LevelSerializer.hpp"

#include "util/Math.hpp"

#include "phys/SurfaceMap.hpp"

std::ostream& LevelSerializer::writeLevel(std::ostream& os, Level& lvl)
{

	int levelNameSize = lvl.getLevelName().size();
	write(levelNameSize, os);
	for (char c : lvl.getLevelName())
		write(c, os);

	// write out individual members of lvl
	//level bg color
	sf::Color bg = lvl.getBGColor();
	int c[] = { bg.r, bg.g, bg.b };
	write(c[0], os);
	write(c[1], os);
	write(c[2], os);

	//level area
	write(lvl.levelArea.left, os);
	write(lvl.levelArea.top, os);
	write(lvl.levelArea.width, os);
	write(lvl.levelArea.height, os);

	//invisible walls
	write(lvl.invisibleWalls[0], os);
	write(lvl.invisibleWalls[1], os);
	write(lvl.invisibleWalls[2], os);
	write(lvl.invisibleWalls[3], os);

	//BG sprite
	int lvlBGNameSize = lvl.getBGSpriteName().size();
	write(lvlBGNameSize, os);
	for (char c : lvl.getBGSpriteName())
		write(c, os);
	//now complex data sets
	//----------------------------
	
	//tileset image names
	int tilesetCount = lvl.getTilesetList()->size();
	write(tilesetCount, os);
	for (auto i = lvl.getTilesetList()->begin(); i != lvl.getTilesetList()->end(); i++) {
		write(i->first, os);
		int tilesetNameSize = i->second.size();
		write(tilesetNameSize, os);
		for (char c : i->second)
			write(c, os);

	}

	writeTileLayer(os, lvl.decorativeLayer);
	writeTileLayer(os, lvl.foregroundLayer);
	writeTileLayer(os, lvl.backgroundLayer);
	int parallaxCount = lvl.parallaxLayers.size();
	write(parallaxCount, os);
	for (auto& paraLayer : lvl.parallaxLayers) {
		write(paraLayer.first, os);
		writeTileLayer(os, paraLayer.second);
	}

	//COLFIX
	writeSurfaceMap(os, *lvl.getSurfaceMap());
	//up collisions
	//writeColVec(os, lvl.upCollisions);
	//right
	//writeColVec(os, lvl.rightCollisions);
	//down
	//writeColVec(os, lvl.downCollisions);
	//left
	//writeColVec(os, lvl.leftCollisions);
	//triggers
	//writeTriggerVec(os, lvl.triggers);

	//Transitions
	writeTransVec(os, lvl.levelTransitions);

	return os;
};

// Extraction operator
std::istream& LevelSerializer::readLevel(std::istream& is, Level& lvl)
{

	int levelNameSize;
	std::string levelName;
	read(levelNameSize, is);
	for (int z = 0; z < levelNameSize; z++) {
		char c;
		read(c, is);
		levelName += c;
	}
	lvl.setLevelName(levelName);


	//read in individual members of level
	//level bg color
	int c[] = { 0, 0, 0 };
	read(c[0], is);
	read(c[1], is);
	read(c[2], is);
	lvl.setBGColor(sf::Color(c[0], c[1], c[2]));
	
	//level area
	read(lvl.levelArea.left, is);
	read(lvl.levelArea.top, is);
	read(lvl.levelArea.width, is);
	read(lvl.levelArea.height, is);

	//invisible walls
	read(lvl.invisibleWalls[0], is);
	read(lvl.invisibleWalls[1], is);
	read(lvl.invisibleWalls[2], is);
	read(lvl.invisibleWalls[3], is);

	// BG Sprite
	int lvlBGSpriteNameSize;
	std::string lvlSpriteName;
	read(lvlBGSpriteNameSize, is);
	for (int z = 0; z < lvlBGSpriteNameSize; z++) {
		char c;
		read(c, is);
		lvlSpriteName += c;
	}
	lvl.setBGSpriteName(lvlSpriteName);
	//now complex data sets
	//----------------------------

	//tileset image names
	int tilesetCount;
	read(tilesetCount, is);
	for (int i = 0; i < tilesetCount; i++) {
		int index;
		read(index, is);
		int tilesetNameSize;
		read(tilesetNameSize, is);
		std::string tilesetName;
		for (int j = 0; j < tilesetNameSize; j++) {
			char c;
			read(c, is);
			tilesetName += c;
		}

		lvl.getTilesetList()->insert(std::make_pair(index, tilesetName));

	}

	readTileLayer(is, lvl.decorativeLayer);
	readTileLayer(is, lvl.foregroundLayer);
	readTileLayer(is, lvl.backgroundLayer);
	int parallaxCount;
	read(parallaxCount, is);
	for (int i = 0; i < parallaxCount; i++) {
		int paraDepth;
		read(paraDepth, is);
		lvl.getParallaxLayers()->insert(std::make_pair(paraDepth, TileLayer(&lvl.tilesets)));
		readTileLayer(is, (*lvl.getParallaxLayers()).at(paraDepth));
	}

	//COLFIX
	readSurfaceMap(is, *lvl.getSurfaceMap(), lvl.levelArea.width, lvl.levelArea.height);
	//up collisions
	//readColVec(is, lvl.upCollisions);
	//right
	//readColVec(is, lvl.rightCollisions);
	//down
	//readColVec(is, lvl.downCollisions);
	//left
	//readColVec(is, lvl.leftCollisions);
	//triggers
	//readTriggerVec(is, lvl.triggers);

	//Transitions
	readTransVec(is, lvl.levelTransitions);

	return is;
};

//----WRITING --------------------------------------------
void LevelSerializer::writeFloatRect(std::ostream& os, const sf::FloatRect rect) {
	write(rect.left, os);
	write(rect.top, os);
	write(rect.width, os);
	write(rect.height, os);
};
/*void LevelSerializer::writeTrigger(std::ostream& os, const Level::Trigger trigger) {
	write(trigger.type, os);
	writeFloatRect(os, trigger.area);
};
void LevelSerializer::writeLvlCol(std::ostream& os, const Level::LevelCollision col) {
	writeFloatRect(os, col.box);

	write(col.upCol, os);
	write(col.rightCol, os);
	write(col.downCol, os);
	write(col.leftCol, os);
}; */
void LevelSerializer::writeTileLayer(std::ostream& os, const TileLayer& layer) {
	//write tile map
	writeTileMap(os, layer.tiles);

	//write animtiles
	int animTimerCount = layer.tileTimers.size();
	write(animTimerCount, os);
	for (auto i = layer.tileTimers.begin(); i != layer.tileTimers.end(); i++) {
		write(i->first.first.asMilliseconds(), os);
		write(i->first.second, os);

		int tileCount = i->second.tilesAffected.size();
		write(tileCount, os);
		for (auto j = i->second.tilesAffected.begin(); j != i->second.tilesAffected.end(); j++) {
			write(j->x, os);
			write(j->y, os);
		}
	}

	//write parallax
	write(layer.parallax.isParallax, os);
	writeFloatRect(os, layer.parallax.area);
	write(layer.parallax.scrollSize.x, os);
	write(layer.parallax.scrollSize.y, os);
	write(layer.parallax.scrollSpeed.x, os);
	write(layer.parallax.scrollSpeed.y, os);
	write(layer.parallax.parallaxRate.x, os);
	write(layer.parallax.parallaxRate.y, os);
};
void LevelSerializer::writeTileMap(std::ostream& os, const std::map<GridVector, Tile> &tilemap) {
	
	
	std::vector<std::pair<Tile, std::vector<GridVector>>> tileData;
	typedef std::vector<std::pair<Tile, std::vector<GridVector>>>::iterator it;

	it isTile;
	Tile t1;
	Tile t2;
	bool inTileData = false;

	//fill tileData
	for (auto i = tilemap.begin(); i != tilemap.end(); i++) {

		inTileData = false;
		t1 = i->second;
		//check if tile is in tileData already
		for (auto j = tileData.begin(); j != tileData.end(); j++) {
			t2 = j->first;

			if (t1.isSimilar(t2)) {
				inTileData = true;
				isTile = j;
				break;
			}
		}
		if (inTileData) {
			//if yes, add pos
			isTile->second.push_back(i->first);
		}
		else {
			//if no, add tile and pos
			tileData.push_back(std::pair<Tile, std::vector<GridVector>>(t1, std::vector<GridVector>()));
			tileData.back().second.push_back(i->first);
		}
	}

	//write tileData
	int tileCount = tileData.size();
	int posCount = 0;
	write(tileCount, os);
	for (auto i = tileData.begin(); i != tileData.end(); i++) {

		write(i->first.tileSprite, os);
		write(i->first.tileProperty, os);
		if (i->first.tileProperty == TileProperty::TILE_ANIMATED) {
			write(i->first.anim_rate.asMilliseconds(), os);
			write(i->first.anim_framecount, os);
		}
		write(i->first.spritePos.x, os);
		write(i->first.spritePos.y, os);
		write(i->first.occluding, os);
		write(i->first.shape.shapeNumCorners, os);
		for (int j = 0; j < i->first.shape.shapeNumCorners; j++) {
			write(i->first.shape.vertexes[j].x, os);
			write(i->first.shape.vertexes[j].y, os);
		}

		posCount = i->second.size();
		write(posCount, os);
		for (auto j = i->second.begin(); j != i->second.end(); j++) {
			write(j->x, os);
			write(j->y, os);
		}
	}
};
/* void LevelSerializer::writeColVec(std::ostream& os, const std::vector<sf::FloatRect> &vec) {
	int size = vec.size();
	write(size, os);
	for (sf::FloatRect rect : vec) {
		writeFloatRect(os, rect);
	}
}; */
void LevelSerializer::writeTrans(std::ostream& os, const Transition t) {

	int sizeStr = t.levelName.size();
	write(sizeStr, os);

	for (char c : t.levelName) {
		write(c, os);
	}

	writeFloatRect(os, t.box);
	write(t.offset, os);
	write((int)t.dir, os);
};
void LevelSerializer::writeTransVec(std::ostream& os, const std::vector<Transition> &vec) {
	int size = vec.size();
	write(size, os);
	for (Transition trans : vec) {
		writeTrans(os, trans);
	}
};
/* void LevelSerializer::writeTriggerVec(std::ostream& os, const std::vector<Level::Trigger> &vec) {
	int size = vec.size();
	write(size, os);
	for (Level::Trigger trigger : vec) {
		writeTrigger(os, trigger);
	}
}; */


void LevelSerializer::writeSurfaceMap(std::ostream& os, SurfaceMap& smap) {

	int size = smap.getAllSurfaces()->size();
	write(size, os);
	for (const std::pair<GridVector, std::vector<Surface>>& vec : *smap.getAllSurfaces()) {
		
		write(vec.first.x, os);
		write(vec.first.y, os);

		write((int)vec.second.size(), os);
		for (const Surface& s : vec.second) {
			write(s.line.start.x, os);
			write(s.line.start.y, os);
			write(s.line.end.x, os);
			write(s.line.end.y, os);
		}
	}
	

}



void LevelSerializer::write(int i, std::ostream& os) {
	os.write((char*)&i, StdSizes::intSize);
}
void LevelSerializer::write(float i, std::ostream& os) {
	os.write((char*)&i, StdSizes::floatSize);
}
void LevelSerializer::write(bool i, std::ostream& os) {
	os.write((char*)&i, StdSizes::boolSize);
}
void LevelSerializer::write(char i, std::ostream& os) {
	os.write((char*)&i, StdSizes::charSize);
}

//----READING --------------------------------------------

void LevelSerializer::readFloatRect(std::istream& is, sf::FloatRect &rect) {
	read(rect.left, is);
	read(rect.top, is);
	read(rect.width, is);
	read(rect.height, is);
};
/* void LevelSerializer::readTrigger(std::istream& is, Level::Trigger &trigger) {
	read(trigger.type, is);
	readFloatRect(is, trigger.area);
};
void LevelSerializer::readLvlCol(std::istream& is, Level::LevelCollision &col) {
	sf::FloatRect rect;
	readFloatRect(is, rect);

	read(col.upCol, is);
	read(col.rightCol, is);
	read(col.downCol, is);
	read(col.leftCol, is);
}; */
void LevelSerializer::readTileLayer(std::istream& is, TileLayer& layer) {
	
	//read tiles map
	layer.tiles.clear();
	readTileMap(is, layer.tiles);

	//read animtiles
	int animTimerCount = 0;
	read(animTimerCount, is);
	for (int i = 0; i < animTimerCount; i++) {
		int framerateMS = 0;
		read(framerateMS, is);
		sf::Time frameRate = sf::milliseconds(framerateMS);

		int frameCount = 0;
		read(frameCount, is);


		auto pair = std::make_pair(frameRate, frameCount);
		layer.tileTimers.insert(std::make_pair(pair, TileLayer::TileAnimTimer()));

		int tileCount = 0;
		read(tileCount, is);
		for (int j = 0; j < tileCount; j++) {
			GridVector v;
			read(v.x, is);
			read(v.y, is);
			layer.tileTimers.at(pair).tilesAffected.insert(v);
		}
	}

	//read parallax
	read(layer.parallax.isParallax, is);
	readFloatRect(is, layer.parallax.area);
	read(layer.parallax.scrollSize.x, is);
	read(layer.parallax.scrollSize.y, is);
	read(layer.parallax.scrollSpeed.x, is);
	read(layer.parallax.scrollSpeed.y, is);
	read(layer.parallax.parallaxRate.x, is);
	read(layer.parallax.parallaxRate.y, is);
}
void LevelSerializer::readTileMap(std::istream& is, std::map<GridVector, Tile> &tilemap) {

	int tileCount = 0;
	read(tileCount, is);
	int posCount = 0;
	Tile t;
	GridVector v;
	for (int i = 0; i < tileCount; i++) {
		
		read(t.tileSprite, is);
		read(t.tileProperty, is);
		if (t.tileProperty == TileProperty::TILE_ANIMATED) {
			int animRateMS = 0;
			read(animRateMS, is);
			t.anim_rate = sf::milliseconds(animRateMS);
			read(t.anim_framecount, is);
		}
		read(t.spritePos.x, is);
		read(t.spritePos.y, is);
		read(t.occluding, is);
		read(t.shape.shapeNumCorners, is);
		for (int j = 0; j < t.shape.shapeNumCorners && j < 4; j++) {
			read(t.shape.vertexes[j].x, is);
			read(t.shape.vertexes[j].y, is);
		}


		read(posCount, is);
		for (int j = 0; j < posCount; j++) {
			read(t.gridPos.x, is);
			read(t.gridPos.y, is);
			v.x = t.gridPos.x;
			v.y = t.gridPos.y;
			tilemap.insert(std::pair<GridVector, Tile>(v, t));
		}
	}
};
/*
void LevelSerializer::readColVec(std::istream& is, std::vector<sf::FloatRect> &vec) {
	vec.clear();

	int size = 0;
	read(size, is);

	for (int i = 0; i < size; i++) {
		sf::FloatRect rect;
		readFloatRect(is, rect);
		vec.push_back(rect);
	}
};
*/
void LevelSerializer::readTrans(std::istream& is, Transition &t) {
	int strCount = 0;
	read(strCount, is);

	for (int j = 0; j < strCount; j++) {
		char c = 0;
		read(c, is);
		t.levelName += c;
	}

	readFloatRect(is, t.box);
	read(t.offset, is);
	int i;
	read(i, is);
	t.dir = (Cardinal)i;
};
void LevelSerializer::readTransVec(std::istream& is, std::vector<Transition> &vec) {
	vec.clear();
	int size = 0;
	read(size, is);
	for (int i = 0; i < size; i++) {
		Transition trans;
		readTrans(is, trans);
		vec.push_back(trans);
	}
};
/* void LevelSerializer::readTriggerVec(std::istream& is, std::vector<Level::Trigger> &vec) {
	vec.clear();
	int size = 0;
	read(size, is);
	for (int i = 0; i < size; i++) {
		Level::Trigger trig;
		readTrigger(is, trig);
		vec.push_back(trig);
	}
} */;

void LevelSerializer::readSurfaceMap(std::istream& is, SurfaceMap& smap, int lvlWidth, int lvlHeight) {
	smap.clear(lvlWidth, lvlHeight);

	int size = 0;
	read(size, is);
	for (int i = 0; i < size; i++) {

		GridVec2 p;
		read(p.x, is);
		read(p.y, is);

		//auto it = smap.getAllSurfaces()->insert(std::make_pair(p, std::vector<Surface>())).first;

		int vsize = 0;
		read(vsize, is);
		for (int j = 0; j < vsize; j++) {

			Vec2 st, en;
			read(st.x, is);
			read(st.y, is);
			read(en.x, is);
			read(en.y, is);
			//it->second.push_back(Surface(st,en));
			smap.addSurface(Surface(st, en), p, true);

		}

	}
	/*
	int size = smap.getAllSurfaces()->size();
	write(size, os);
	for (const std::pair<GridVector, std::vector<Surface>>& vec : *smap.getAllSurfaces()) {
		
		write(vec.first.x, os);
		write(vec.first.y, os);

		write((int)vec.second.size(), os);
		for (const Surface& s : vec.second) {
			write(s.line.start.x, os);
			write(s.line.start.y, os);
			write(s.line.end.x, os);
			write(s.line.end.y, os);
		}
	}
	 * */
}

void LevelSerializer::read(int &i, std::istream& is) {
	is.read((char*)&i, StdSizes::intSize);
}
void LevelSerializer::read(float &i, std::istream& is) {
	is.read((char*)&i, StdSizes::floatSize);
}
void LevelSerializer::read(bool &i, std::istream& is) {
	is.read((char*)&i, StdSizes::boolSize);
}
void LevelSerializer::read(char &i, std::istream& is) {
	is.read((char*)&i, StdSizes::charSize);
}
