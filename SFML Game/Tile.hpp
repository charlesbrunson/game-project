#ifndef TILE_H
#define TILE_H

#include <SFML\System.hpp>
#include "TileProperty.hpp"

const int tileSpacing = 16;

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

	bool occluding = true;
};

#endif