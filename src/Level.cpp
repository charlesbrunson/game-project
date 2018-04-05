#include <SFML/Graphics.hpp>
#include "game/lvl/Level.hpp"

//#include "CamRef.hpp"
#include "util/PixelSnap.hpp"
#include "util/Math.hpp"

Level::Level() :
	decorativeLayer(&tilesets),
	foregroundLayer(&tilesets),
	backgroundLayer(&tilesets)
	{
	invisibleWalls[0] = false;
	invisibleWalls[1] = false;
	invisibleWalls[2] = false;
	invisibleWalls[3] = false;
};

/* std::vector<Level::Trigger> *Level::getTriggers() {
	return &triggers;
} */

std::vector<Transition> *Level::getLevelTransitions() {
	return &levelTransitions;
}

bool Level::hasTransition(std::string name) {
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

	decorativeLayer.update(deltaTime, gridArea, intersection, camCenter, reDraw);
	foregroundLayer.update(deltaTime, gridArea, intersection, camCenter, reDraw);
	backgroundLayer.update(deltaTime, gridArea, intersection, camCenter, reDraw);

	for (auto i = parallaxLayers.begin(); i != parallaxLayers.end(); i++)
		i->second.update(deltaTime, gridArea, intersection, camCenter, reDraw);

	if (!bgSpriteFilename.empty() && !bgSprite.getTexture()) {
		bgSprite.setTexture(RL()->getTexture("sprites/bg/" + bgSpriteFilename));
		bgSprite.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
	}
	bgSprite.setPosition(snapToPixel(camCenter - sf::Vector2f(GAMEWIDTH / 2.f, GAMEHEIGHT / 2.f)));

	lastFrameDisplay = intersection;
}

void Level::setArea(int left, int top, int w, int h) {
	levelArea = sf::IntRect(left, top, w, h);
}

void Level::generateSurfaceMap() {

	//make level border
	sf::FloatRect tileArea(0.f, 0.f, tileSpacing, tileSpacing);
	auto transitions = getLevelTransitions();
	TileShape wall;
	wall.shapeNumCorners = 2;

	auto touchesTransition = [&transitions](const Cardinal dir, const sf::FloatRect& area) {
		for (auto i = transitions->begin(); i != transitions->end(); i++) {
			if (i->dir == dir && i->box.intersects(area)) {
				return true;
			}
		}
		return false;
	};

	if (invisibleWalls[Cardinal::NORTH]) {
		tileArea.left = 0.f;
		tileArea.top = -tileSpacing;

		for (int x = 0; x < levelArea.width / tileSpacing; x++) {
			tileArea.left = x * tileSpacing;

			if (touchesTransition(Cardinal::NORTH, tileArea))
				continue;

			wall.vertexes[0] = Point(1.f, 1.f);
			wall.vertexes[1] = Point(0.f, 1.f);
			surfaces.addShape(wall, GridVec2(x, -1), true);
		}
	}
	if (invisibleWalls[Cardinal::EAST]) {
		tileArea.left = levelArea.width;
		tileArea.top = 0.f;

		for (int y = 0; y < levelArea.height / tileSpacing; y++) {
			tileArea.top = y * tileSpacing;

			if (touchesTransition(Cardinal::EAST, tileArea))
				continue;

			wall.vertexes[0] = Point(0.f, 1.f);
			wall.vertexes[1] = Point(0.f, 0.f);
			surfaces.addShape(wall, GridVec2(levelArea.width / tileSpacing, y), true);
		}
	}
	if (invisibleWalls[Cardinal::SOUTH]) {
		tileArea.left = 0.f;
		tileArea.top = levelArea.height;

		for (int x = 0; x < levelArea.width / tileSpacing; x++) {
			tileArea.left = x * tileSpacing;

			if (touchesTransition(Cardinal::SOUTH, tileArea))
				continue;

			wall.vertexes[0] = Point(0.f, 0.f);
			wall.vertexes[1] = Point(1.f, 0.f);
			surfaces.addShape(wall, GridVec2(x, levelArea.height / tileSpacing), true);
		}
	}
	if (invisibleWalls[Cardinal::EAST]) {
		tileArea.left = -tileSpacing;
		tileArea.top = 0.f;

		for (int y = 0; y < levelArea.height / tileSpacing; y++) {
			tileArea.top = y * tileSpacing;

			if (touchesTransition(Cardinal::WEST, tileArea))
				continue;

			wall.vertexes[0] = Point(1.f, 0.f);
			wall.vertexes[1] = Point(1.f, 1.f);
			surfaces.addShape(wall, GridVec2(-1, y), true);
		}
	}
	

	//add level collision
	auto tiles = &foregroundLayer.tiles;
	for (auto t = tiles->begin(); t != tiles->end(); t++) {
		surfaces.addTile(t->second);
	}

}

/*
std::vector<Surface> Level::getTileCollisionBox(sf::Vector2i gridPosition) {

	std::vector<Surface> collisionArr;

	//TODO

	return collisionArr;
}
*/

void Level::syncLayerTimers(sf::Time zoneTimer) {
	decorativeLayer.syncTimersWithZone(zoneTimer);
	foregroundLayer.syncTimersWithZone(zoneTimer);
	backgroundLayer.syncTimersWithZone(zoneTimer);
	for (auto i = parallaxLayers.begin(); i != parallaxLayers.end(); i++)
		i->second.syncTimersWithZone(zoneTimer);
}

void Level::setInvisibleWall(int index, bool prop) {
	if (index < 0 || index > 3) {
		Log::msg("ERROR: Can't set invisible wall, index out of bounds");
		return;
	}

	invisibleWalls[index] = prop;
}
