#ifndef TILE_H
#define TILE_H

#include <SFML/System.hpp>
#include "game/lvl/TileProperty.hpp"

const float tileSpacing = 16.f;

struct TileShape {

	Point vertexes[4] = {
		Point(0.f, 0.f),
		Point(0.f, 0.f),
		Point(0.f, 0.f),
		Point(0.f, 0.f)
	};
	int shapeNumCorners = 4;
	static const int shapeMaxCorners = 4;

	bool operator== (const TileShape& t) const {
	   return t.shapeNumCorners == shapeNumCorners &&
			   t.vertexes[0] == vertexes[0] &&
			   t.vertexes[1] == vertexes[1] &&
			   t.vertexes[2] == vertexes[2] &&
			   t.vertexes[3] == vertexes[3];
	};
};

struct Tile {
	// The sprite to be used for this tile
	// Use tilesets map to translate to filename
	int tileSprite = -1;

	// Grid position of this tile in level, beware putting two tiles in same spot on same layer
	sf::Vector2i gridPos = sf::Vector2i(0, 0);

	// Position of target sprite on spritesheet
	sf::Vector2i spritePos = sf::Vector2i(0, 0);

	// Used for animated tiles
	sf::Time anim_rate = sf::Time::Zero;
	int anim_framecount = 0;

	int tileProperty = TileProperty::TILE_DEFAULT;

	TileShape shape;

	bool occluding = true;

	bool isSimilar(const Tile& t) const {
		return t.tileSprite == tileSprite &&
			   t.tileProperty == tileProperty &&
			   t.spritePos == spritePos &&
			   t.occluding == occluding &&
			   t.shape == shape;
	};

	bool operator== (const Tile& t) const {
		return isSimilar(t) && gridPos == t.gridPos;
	};
};

#endif
