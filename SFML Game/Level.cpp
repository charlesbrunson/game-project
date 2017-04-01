#include <SFML/Graphics.hpp>
#include "Level.hpp"

//#include "CamRef.hpp"
#include "PixelSnap.hpp"

#include "Math.hpp"

Level::Level(ResourceLoader *rloader) : 
	ResourceUser(rloader),
	decorativeLayer(rloader, &tilesets),
	foregroundLayer(rloader, &tilesets),
	backgroundLayer(rloader, &tilesets)
	{
	invisibleWalls[0] = false;
	invisibleWalls[1] = false;
	invisibleWalls[2] = false;
	invisibleWalls[3] = false;
};

std::vector<Level::Trigger> *Level::getTriggers() {
	return &triggers;
}

std::vector<Transition> *Level::getLevelTransitions() {
	return &levelTransitions;
}

bool Level::hasTransition(sf::String name) {
	for (auto i = levelTransitions.begin(); i != levelTransitions.end(); i++) {
		if (i->levelName == name)
			return true;
	}
	return false;
}

void Level::update(sf::Time deltaTime, sf::FloatRect displayArea, sf::Vector2f camCenter, bool reDraw) {

	sf::FloatRect intersection;
	displayArea.intersects(sf::FloatRect(levelArea), intersection);
	
	sf::IntRect gridArea;
	gridArea.left = floorf(intersection.left / (float)tileSpacing);
	gridArea.top = floorf(intersection.top / (float)tileSpacing);
	gridArea.width = ceilf((intersection.left + intersection.width) / (float)tileSpacing) - gridArea.left;
	gridArea.height = ceilf((intersection.top + intersection.height) / (float)tileSpacing) - gridArea.top;

	decorativeLayer.update(deltaTime, gridArea, intersection, camCenter, levelArea, reDraw);
	foregroundLayer.update(deltaTime, gridArea, intersection, camCenter, levelArea, reDraw);
	backgroundLayer.update(deltaTime, gridArea, intersection, camCenter, levelArea, reDraw);

	for (auto i = parallaxLayers.begin(); i != parallaxLayers.end(); i++)
		i->second.update(deltaTime, gridArea, intersection, camCenter, levelArea, reDraw);

	if (!bgSpriteFilename.empty() && !bgSprite.getTexture()) {
		bgSprite.setTexture(*rMan->getTexture("bg/" + bgSpriteFilename));
		bgSprite.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
	}
	bgSprite.setPosition(snapToPixel(camCenter - sf::Vector2f(GAMEWIDTH / 2.f, GAMEHEIGHT / 2.f)));

	lastFrameDisplay = intersection;
}

void Level::setArea(int left, int top, int w, int h) {
	levelArea = sf::IntRect(left, top, w, h);
}

Level::LevelCollision Level::getTileCollisionBox(sf::Vector2i gridPosition) {
	LevelCollision c;

	GridVector v;
	v.x = gridPosition.x;
	v.y = gridPosition.y;

	c.box = sf::FloatRect(gridPosition.x * tileSpacing, gridPosition.y * tileSpacing, tileSpacing, tileSpacing);
	auto *fTiles = &foregroundLayer.tiles;
	auto t = fTiles->find(GridVector(gridPosition.x, gridPosition.y));
		
	if (t != fTiles->end() && t->second.tileProperty == TileProperty::TILE_NOCOLLISION) {
		return c;
	}

	sf::IntRect gridArea(levelArea);
	gridArea.left /= tileSpacing;
	gridArea.top /= tileSpacing;
	gridArea.width /= tileSpacing;
	gridArea.height /= tileSpacing;

	auto neighborTester = [&gridArea, &fTiles](const GridVector &pos, bool &col) {
		if (gridArea.contains(pos)) {
			col = fTiles->find(pos) == fTiles->end();
			if (col == false) {
				int t = fTiles->find(pos)->second.tileProperty;
				col = col || t == TileProperty::TILE_ONEWAY || t == TileProperty::TILE_NOCOLLISION;
			}
		}
	};

	//check top
	neighborTester(GridVector(gridPosition.x, gridPosition.y - 1), c.upCol);
	if (t == fTiles->end() || t->second.tileProperty != TileProperty::TILE_ONEWAY) {
		//check right
		neighborTester(GridVector(gridPosition.x + 1, gridPosition.y), c.rightCol);

		//check down
		neighborTester(GridVector(gridPosition.x, gridPosition.y + 1), c.downCol);

		//check left
		neighborTester(GridVector(gridPosition.x - 1, gridPosition.y), c.leftCol);
	}
	return c;
}

void Level::generateCollisionMap() {

	//make floatrect from collidable tiles
	collisionMap.clear();

	upCollisions.clear();
	rightCollisions.clear();
	downCollisions.clear();
	leftCollisions.clear();

	auto *fTiles = &foregroundLayer.tiles;

	//get tile info
	LevelCollision c;
	for (std::map<GridVector, Tile>::const_iterator it = fTiles->begin(); it != fTiles->end(); ++it) {
		c = getTileCollisionBox(it->second.gridPos);
		if (c.upCol || c.rightCol || c.leftCol || c.downCol)
			collisionMap.push_back(c);
	}

	//invisible wall info
	auto transitions = getLevelTransitions();
	auto touchesTransition = [&transitions, &c](const Cardinal dir) {
		for (auto i = transitions->begin(); i != transitions->end(); i++) {
			if (i->dir == dir && i->box.intersects(c.box)) {
				return true;
			}
		}
		return false;
	};
	if (invisibleWalls[0]) {
		for (int i = 0; i < levelArea.width / 16; i++) {
			c = getTileCollisionBox(sf::Vector2i(i, -1));
			if (touchesTransition(Cardinal::NORTH)) continue;
			if (c.downCol) collisionMap.push_back(c);
		}
	}
	if (invisibleWalls[1]) {
		for (int i = 0; i < levelArea.height / 16; i++) {
			c = getTileCollisionBox(sf::Vector2i((levelArea.width / 16), i));
			if (touchesTransition(Cardinal::EAST)) continue;
			if (c.leftCol) collisionMap.push_back(c);
		}
	}
	if (invisibleWalls[2]) {
		for (int i = 0; i < levelArea.width / 16; i++) {
			c = getTileCollisionBox(sf::Vector2i(i, (levelArea.height / 16)));
			if (touchesTransition(Cardinal::SOUTH))
				continue;
			if (c.upCol) collisionMap.push_back(c);
		}
	}
	if (invisibleWalls[3]) {
		for (int i = 0; i < levelArea.height / 16; i++) {
			c = getTileCollisionBox(sf::Vector2i(-1, i));
			if (touchesTransition(Cardinal::WEST)) continue;
			if (c.rightCol) collisionMap.push_back(c);
		}
	}

	//fill collisions 
	for (std::vector<LevelCollision>::const_iterator it = collisionMap.begin(); it != collisionMap.end(); it++) {

		//top --------------------------------------------
		if (it->upCol) {
			bool inserted = false;
			//see if we can add it to already existing box
			if (!upCollisions.empty()) {
				for (std::vector<sf::FloatRect>::iterator it2 = upCollisions.begin(); it2 != upCollisions.end(); it2++) {
					if (it2->top == it->box.top) {
						//check right side
						if (it2->left + it2->width == it->box.left) {
							//add it to right side
							it2->width += it->box.width;
							inserted = true;
							break;
						}
						//check left side
						else if (it2->left == it->box.left + it->box.width) {
							//add it to left side
							it2->left -= it->box.width;
							it2->width += it->box.width;
							inserted = true;
							break;
						}
					}
				}
			}
			if (!inserted) {
				//else insert new one
				upCollisions.push_back(it->box);
			}
		}

		//left --------------------------------------------
		if (it->leftCol) {
			bool inserted = false;
			//see if we can add it to already existing box
			if (!leftCollisions.empty()) {
				for (std::vector<sf::FloatRect>::iterator it2 = leftCollisions.begin(); it2 != leftCollisions.end(); it2++) {
					if (it2->left == it->box.left) {
						//check bottom
						if (it2->top + it2->height == it->box.height) {
							//add to bottom
							it2->height += it->box.height;
							inserted = true;
							break;
						}
						//check top
						else if (it2->top == it->box.top + it->box.height) {
							//add it top
							it2->top -= it->box.height;
							it2->height += it->box.height;
							inserted = true;
							break;
						}
					}
				}
			}
			if (!inserted) {
				//else insert new one
				leftCollisions.push_back(it->box);
			}
		}

		//down --------------------------------------------
		if (it->downCol) {
			bool inserted = false;
			//see if we can add it to already existing box
			if (!downCollisions.empty()) {
				for (std::vector<sf::FloatRect>::iterator it2 = downCollisions.begin(); it2 != downCollisions.end(); it2++) {
					if (it2->top == it->box.top) {
						//check right side
						if (it2->left + it2->width == it->box.left) {
							//add it to right side
							it2->width += it->box.width;
							inserted = true;
							break;
						}
						//check left side
						else if (it2->left == it->box.left + it->box.width) {
							//add it to left side
							it2->left -= it->box.width;
							it2->width += it->box.width;
							inserted = true;
							break;
						}
					}
				}
			}
			if (!inserted) {
				//else insert new one
				downCollisions.push_back(it->box);
			}
		}

		//right --------------------------------------------
		if (it->rightCol) {
			bool inserted = false;
			//see if we can add it to already existing box
			if (!rightCollisions.empty()) {
				for (std::vector<sf::FloatRect>::iterator it2 = rightCollisions.begin(); it2 != rightCollisions.end(); it2++) {
					if (it2->left == it->box.left) {
						//check bottom
						if (it2->top + it2->height == it->box.height) {
							//add to bottom
							it2->height += it->box.height;
							inserted = true;
							break;
						}
						//check top
						else if (it2->top == it->box.top + it->box.height) {
							//add it top
							it2->top -= it->box.height;
							it2->height += it->box.height;
							inserted = true;
							break;
						}
					}
				}
			}
			if (!inserted) {
				//else insert new one
				rightCollisions.push_back(it->box);
			}
		}
	}
}

void Level::syncLayerTimers(sf::Time zoneTimer) {
	decorativeLayer.syncTimersWithZone(zoneTimer);
	foregroundLayer.syncTimersWithZone(zoneTimer);
	backgroundLayer.syncTimersWithZone(zoneTimer);
	for (auto i = parallaxLayers.begin(); i != parallaxLayers.end(); i++)
		i->second.syncTimersWithZone(zoneTimer);
}

void Level::setInvisibleWall(int index, bool prop) {
	if (index < 0 || index > 3) {
		Log::msg("ERROR: Can't set invisible wall, index out of bounds\n");
		return;
	}

	invisibleWalls[index] = prop;
}