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
