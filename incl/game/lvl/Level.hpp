#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include <istream>
#include <string>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "util/Log.hpp"

#include "res/ResourceLoader.hpp"

#include "Globals.hpp"
#include "util/GridVector.hpp"
#include "util/StandardSize.hpp"
#include "game/lvl/TileProperty.hpp"

#include "game/lvl/Tile.hpp"
#include "game/lvl/TileLayer.hpp"

#include "util/Math.hpp"
#include "phys/SurfaceMap.hpp"

struct Transition {

	sf::FloatRect box;

	int offset;
	Cardinal dir = NO_DIR;

	std::string levelName;
};

class Level {
	friend class LevelSerializer;
public:
		
	Level();

	void initializeTileProperties();

	std::vector<Transition> *getLevelTransitions();
	bool hasTransition(std::string name);


	void update(sf::Time deltaTime, sf::FloatRect displayArea, sf::Vector2f camCenter, bool reDraw = false);
	
	void buildSurfaceMap();

	void setArea(int left, int top, int w, int h);

	inline void clearSpriteTiles() { 
		lastFrameGrid = sf::IntRect(); 
		lastFrameDisplay = sf::FloatRect();
		unloadSprites(); 
	}
		
	//COLFIX
	inline SurfaceMap* getSurfaceMap() { return &surfaces; };	

	inline TileLayer* getDecorativeLayer()					{ return &decorativeLayer; };
	inline TileLayer* getForegroundLayer()					{ return &foregroundLayer; };
	inline TileLayer* getBackgroundLayer()					{ return &backgroundLayer; };

	inline std::map<GridVector, Tile> *getDecorativeTiles()	{ return &decorativeLayer.tiles; };
	inline std::map<GridVector, Tile> *getForegroundTiles()	{ return &foregroundLayer.tiles; };
	inline std::map<GridVector, Tile> *getBackgroundTiles()	{ return &backgroundLayer.tiles; };

	inline std::map<GridVector, sf::Sprite> *getDecorativeSprites() { return &decorativeLayer.sprites; };
	inline std::map<GridVector, sf::Sprite> *getForegroundSprites() { return &foregroundLayer.sprites; };
	inline std::map<GridVector, sf::Sprite> *getBackgroundSprites() { return &backgroundLayer.sprites; };
	
	//gameplay data
	sf::IntRect levelArea;
	sf::Vector2f playerStartPos;

	//background data
	inline void setBGColor(sf::Color c) { bgColor = c; }
	inline sf::Color getBGColor() {	return bgColor; }

	//parallax layers
	std::map<int, TileLayer>* getParallaxLayers() {	return &parallaxLayers;	}
	bool initParallaxSprites = false;

	//invisible walls
	void setInvisibleWall(int index, bool prop);

	void syncLayerTimers(sf::Time zoneTimer);

	void setLevelName(std::string n) { levelName = n; }
	std::string getLevelName() { return levelName; }
	std::map<int, std::string> *getTilesetList() { 	return &tilesets; }

	void setBGSpriteName(std::string spriteName) {
		bgSpriteFilename = spriteName;
	}

	std::string getBGSpriteName() {
		return bgSpriteFilename;
	}
	sf::Sprite getBGSprite() {
		//sprite not init, init before returning
		if (!bgSpriteFilename.empty() && !bgSprite.getTexture()) {
			bgSprite.setTexture(RL()->getTexture("sprites/bg/" + bgSpriteFilename));
			bgSprite.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
		}
		return bgSprite;
	}

private:

	sf::Vector2i position;

	std::string levelName;

	//0 = north, 1 = east, etc
	bool invisibleWalls[4];

	//clears background and foreground sprites
	void unloadSprites() {
		decorativeLayer.sprites.clear();
		foregroundLayer.sprites.clear();
		backgroundLayer.sprites.clear();
		for (auto &i : parallaxLayers)
			i.second.sprites.clear();
	}

	//corresponds to layer enum
	int drawLayer = 0;

	bool levelLoaded = false;
	
	sf::Vector2f lastCamPos;

	sf::FloatRect lastFrameDisplay;
	sf::IntRect lastFrameGrid;

	std::string bgSpriteFilename = "";
	sf::Sprite bgSprite;
	sf::Color bgColor = sf::Color::Black;

	std::map<int, std::string> tilesets;

	TileLayer decorativeLayer;
	TileLayer foregroundLayer;
	TileLayer backgroundLayer;
	std::map<int, TileLayer> parallaxLayers;

	std::vector<Transition> levelTransitions;
	
	SurfaceMap surfaces;

};

#endif
