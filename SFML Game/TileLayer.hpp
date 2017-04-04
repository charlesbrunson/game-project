#ifndef TILELAYER_H
#define TILELAYER_H

#include "Tile.hpp"

#include <unordered_set>

class TileLayer : public sf::Drawable {
public:
	//parallax info
	/*
	struct ParallaxMirror {
		ParallaxMirror(GridVector p, bool x, bool y) : tilePos(p), mirrorX(x), mirrorY(y) {};

		GridVector tilePos;
		bool mirrorX;
		bool mirrorY;
	};
	*/
	struct Parallax {
		bool isParallax = false;

		sf::FloatRect area;

		// rate of movement compared to camera
		sf::Vector2f parallaxRate;

		sf::Vector2f offset; //modified per frame
		//std::vector<ParallaxMirror> mirrors; //modified per frame

		sf::Vector2f scrollSpeed;
		sf::Vector2i scrollSize;
		sf::Vector2f scrollOffset; //modified per frame
	};
	//animation tile info
	struct TileAnimTimer {

		TileAnimTimer(int currFrame = 0) : currentFrame(currFrame) {};

		sf::Time timeAccumulator = sf::Time::Zero;
		int currentFrame = 0;

		//tiles effected
		std::set<GridVector> tilesAffected;
	};

	TileLayer(std::map<int, std::string>* tileNames) 
		: tilesetNames(tileNames) {
	
	}
	
	//pointer tileset filenames used
	const std::map<int, std::string>* tilesetNames;

	//tiles and associated onscreen sprites
	std::map<GridVector, Tile> tiles;
	std::map<GridVector, sf::Sprite> sprites;

	//sf::Time = time per frame, int = numOfFrames
	std::map<std::pair<sf::Time, int>, TileAnimTimer> tileTimers;

	//parallax info
	Parallax parallax;

	// copies the tile & parallax data other another tilelayer
	void copy(const TileLayer &t);

	void update(sf::Time deltaTime, sf::IntRect gridArea, sf::FloatRect displayArea, sf::Vector2f camCenter, bool DrawAll = false);

	sf::Vector2f updateParallax(sf::Vector2f camCenter);
	void updateScroll(sf::Time t, sf::FloatRect area, bool isZoneTimer = false);
	void updateSpriteAnimation(sf::Time t, bool isZoneTimer = false);
	void updateSpritePosition(sf::Vector2f pOffset, sf::Vector2f sOffset, sf::FloatRect pArea);

	//updates scrolling offset, returns whether mirror tiles need to be updated
	//bool updateScroll(sf::Time t, sf::FloatRect area, bool isZoneTimer = false);
	//void updateSpritePosition(sf::Vector2f pOffset, sf::Vector2f sOffset, sf::FloatRect pArea, bool updateMirrors);

	//generate sprite according to tile
	void addTileSprite(Tile *tile, GridVector pos);

	//insert tile data into
	void setTile(sf::Vector2i gridPosition, int tileSpr, sf::Vector2i offset);

	int getTileSpriteName(GridVector gridpos, int layer) const;

	void syncTimersWithZone(sf::Time zoneTimer);

	Parallax* getParallax();

	inline bool isParallax() const {
		return parallax.parallaxRate != sf::Vector2f();
	}
	inline bool isScrolling() const {
		return parallax.scrollSpeed != sf::Vector2f();
	}
	
protected:
	sf::IntRect lastFrameGridArea;
	sf::FloatRect lastFrameDisplayArea;

	//the tile layer above this one, if any
	TileLayer* aboveLayer = nullptr;
		
private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	//debug stuff
	sf::IntRect dtotalEffectedArea;
	sf::IntRect dintersection;

	sf::Vector2f camPrevPosition;
};

#endif