#ifndef LEVEL_H
#define LEVEL_H

#include <list>
#include <istream>
#include <string>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Log.hpp"

#include "ResourceLoader.hpp"
//#include "ResourceUser.hpp"
#include "Globals.hpp"
#include "GridVector.hpp"
#include "StandardSize.hpp"
#include "TileProperty.hpp"

#include "Tile.hpp"
#include "TileLayer.hpp"

#include "Math.hpp"

struct Transition {

	sf::FloatRect box;

	int offset;
	Cardinal dir = NO_DIR;

	std::string levelName;
};


class Level /*, public sf::Drawable */{
	friend class LevelSerializer;
public:
	
	//what layer to draw in draw call
	//int layerToDraw;
		
	//CONSTRUCTOR -----------------------------------------------------------------------------
	Level();
	~Level() {
		Log::msg("destroying " + getLevelName() + "\n");
	};

	void initializeTileProperties();

	//ENUMS -----------------------------------------------------------------------------
	/*
	enum layers : int {
		BGSPRITE,
		PARALLAX,
		BACKGROUND,
		FOREGROUND,
		DECORATIVE
	};*/
	enum TriggerType : int {
		NULL_TRIGGER = 0,
		DAMAGE,
		PUSH_LEFT,
		PUSH_RIGHT
	};

	//STRUCTS -----------------------------------------------------------------------------
	struct LevelCollision {
		//grid position of this tile in level, beware putting two tiles in same spot on same layer
		sf::FloatRect box;

		bool upCol = false;
		bool rightCol = false;
		bool downCol = false;
		bool leftCol = false;
	};
	struct Trigger {
		int type = NULL_TRIGGER;
		sf::FloatRect area;
	};

	//------------------------------------

	std::vector<Trigger> *getTriggers();

	std::vector<Transition> *getLevelTransitions();
	bool hasTransition(std::string name);


	void update(sf::Time deltaTime, sf::FloatRect displayArea, sf::Vector2f camCenter, bool reDraw = false);
	
	void generateCollisionMap();

	void setArea(int left, int top, int w, int h);

	inline void clearSpriteTiles() { 
		lastFrameGrid = sf::IntRect(); 
		lastFrameDisplay = sf::FloatRect();
		unloadSprites(); 
	}
		
	//collision data
	inline std::vector<LevelCollision> *getCollisionMap()	{ return &collisionMap; };

	inline std::vector<sf::FloatRect> *getUpCol()			{ return &upCollisions; };
	inline std::vector<sf::FloatRect> *getRightCol()		{ return &rightCollisions; };
	inline std::vector<sf::FloatRect> *getDownCol()			{ return &downCollisions; };
	inline std::vector<sf::FloatRect> *getLeftCol()			{ return &leftCollisions; };

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
	//const static int spikeDamage = 4;
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
			bgSprite.setTexture(*RL()->getTexture("bg/" + bgSpriteFilename));
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

	std::vector<Trigger> triggers;
	
	//collision data for each collidable tile, possibly effects too (like sliding/pushing)
	std::vector<LevelCollision> collisionMap;

	//directional collision boxes we actually use
	std::vector<sf::FloatRect> upCollisions;
	std::vector<sf::FloatRect> rightCollisions;
	std::vector<sf::FloatRect> downCollisions;
	std::vector<sf::FloatRect> leftCollisions;
	
	Level::LevelCollision getTileCollisionBox(sf::Vector2i gridPosition);

	//void draw(sf::RenderTarget &target, sf::RenderStates states) const;

};

#endif