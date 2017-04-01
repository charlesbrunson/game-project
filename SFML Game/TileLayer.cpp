#include <math.h>

#include "TileLayer.hpp"
#include "TileProperty.hpp"
#include "Globals.hpp"

#include "PixelSnap.hpp"
#include "Log.hpp"

//#include "CamRef.hpp"

void TileLayer::update(sf::Time deltaTime, sf::IntRect gridArea, sf::FloatRect displayArea, sf::Vector2f camCenter, bool drawAll) {
	
	if (isParallax()) {
		parallax.offset = updateParallax(camCenter);
	}

	//parallax scroll area
	sf::FloatRect pArea
		(0.f,
		0.f,
		parallax.scrollSize.x * tileSpacing,
		parallax.scrollSize.y * tileSpacing);

	if (isScrolling()) {
		updateScroll(deltaTime, pArea);
	}
	if (isParallax() || isScrolling()) {
		//move display relative to parallax layer offsets
		displayArea.left -= (parallax.offset.x + parallax.scrollOffset.x);
		displayArea.top -= (parallax.offset.y + parallax.scrollOffset.y);


		//recalculate grid area
		sf::IntRect nGrid;
		nGrid.left = (int)floorf(displayArea.left / (float)tileSpacing);
		nGrid.top = (int)floorf(displayArea.top / (float)tileSpacing);
		nGrid.width = (int)ceilf((displayArea.left + displayArea.width) / (float)tileSpacing) - nGrid.left;
		nGrid.height = (int)ceilf((displayArea.top + displayArea.height) / (float)tileSpacing) - nGrid.top;
		gridArea = sf::IntRect(nGrid);
	}

	//add and remove sprites
	if (gridArea != lastFrameGridArea) {
		sf::IntRect totalEffectedArea;

		sf::IntRect intersection;
		bool intersects = gridArea.intersects(lastFrameGridArea, intersection);
		
		if (drawAll) {
			totalEffectedArea = gridArea;
		}
		else {
			totalEffectedArea = sf::IntRect(
				std::min(gridArea.left, lastFrameGridArea.left),
				std::min(gridArea.top, lastFrameGridArea.top),
				std::max(gridArea.left + gridArea.width, lastFrameGridArea.left + lastFrameGridArea.width),
				std::max(gridArea.top + gridArea.height, lastFrameGridArea.top + lastFrameGridArea.height)
				);
			totalEffectedArea.width -= totalEffectedArea.left;
			totalEffectedArea.height -= totalEffectedArea.top;
		}

		GridVector position;
		bool hasSprite;

		bool inCurrent;
		bool inPrev;
		std::map<GridVector, Tile>::iterator tileAt;
		for (position.x = totalEffectedArea.left; position.x <= totalEffectedArea.left + totalEffectedArea.width; position.x++) {
			for (position.y = totalEffectedArea.top; position.y <= totalEffectedArea.top + totalEffectedArea.height; position.y++) {
				
				GridVector wrapPos(position);
				if (isScrolling()) {
					if (wrapPos.x < 0)
						wrapPos.x += parallax.scrollSize.x;
					if (wrapPos.y < 0)
						wrapPos.y += parallax.scrollSize.y;
				}
				
				if (!drawAll) {
					if (intersection.contains(position)) {
						//skip past intersection
						position.y = intersection.top + intersection.height - 1;
						continue;
					}

					inPrev = lastFrameGridArea.contains(position);
					hasSprite = sprites.find(position) != sprites.end();
				}
				else {
					inPrev = false;
					hasSprite = false;
				}
				
				inCurrent = gridArea.contains(position);
				bool hasTile = tiles.find(wrapPos) != tiles.end();

				if (inPrev == inCurrent) {
					continue;
				}
				else if (!inCurrent) {
					if (hasSprite)
						sprites.erase(position);
				}
				else if ((tileAt = tiles.find(wrapPos)) != tiles.end()) {
					addTileSprite(&tileAt->second, position);

					if (tileAt->second.tileProperty == TileProperty::TILE_ANIMATED) {

						//add to timers
						std::set<GridVector> *tilesAff = &tileTimers.at(std::make_pair(tileAt->second.anim_rate, tileAt->second.anim_framecount)).tilesAffected;
						if (tilesAff->find(position) == tilesAff->end()) {
							tilesAff->insert(position);
						}
					}

					
					//TODO: tile layering occlusion?
				}
			}
		}

		dtotalEffectedArea = totalEffectedArea;
		dintersection = intersection;
	}
	lastFrameGridArea = gridArea;
	
	//update tile animation
	updateSpriteAnimation(deltaTime);
	
	//update parallax & scrolling
	if ((isParallax() && camPrevPosition != camCenter) || isScrolling()) {
		updateSpritePosition(parallax.offset, parallax.scrollOffset, pArea);
	}
	lastFrameDisplayArea = displayArea;
	camPrevPosition = camCenter;
}

sf::Vector2f TileLayer::updateParallax(sf::Vector2f camCenter) {
	
	//top left of the camera
	camCenter = snapToPixel(camCenter);
	sf::Vector2f camOffset = camCenter - sf::Vector2f((float)GAMEWIDTH / 2.f, (float)GAMEHEIGHT / 2.f);

	sf::Vector2f _offset;
	_offset.x = camOffset.x * parallax.parallaxRate.x;
	_offset.y = camOffset.y * parallax.parallaxRate.y;

	sf::Vector2f snapped = snapToPixel(_offset);

	sf::FloatRect parallaxArea(0.f, 0.f, (parallax.area.width * tileSpacing) - GAMEWIDTH, (parallax.area.height * tileSpacing) - GAMEHEIGHT);
	_offset.x = parallaxArea.width > 0.f ? snapped.x : _offset.x;
	_offset.y = parallaxArea.height > 0.f ? snapped.y : _offset.y;

	return _offset;
}

void TileLayer::updateScroll(sf::Time t, sf::FloatRect area, bool isZoneTimer) {

	//sf::Vector2f snappedOffset;
	sf::Vector2f lastOffset = parallax.scrollOffset;

	if (!isZoneTimer) {
		parallax.scrollOffset += parallax.scrollSpeed * t.asSeconds();
	}
	else {
		parallax.scrollOffset = parallax.scrollSpeed * t.asSeconds();
	}
	
	//normalize offset
	while (parallax.scrollOffset.x < 0)
		parallax.scrollOffset.x += area.width;
	while (parallax.scrollOffset.x >= area.width)
		parallax.scrollOffset.x -= area.width;

	while (parallax.scrollOffset.y < 0)
		parallax.scrollOffset.y += area.height;
	while (parallax.scrollOffset.y >= area.height)
		parallax.scrollOffset.y -= area.height;
	
}

//update tile animation
void TileLayer::updateSpriteAnimation(sf::Time t, bool isZoneTimer) {

	if (!isZoneTimer) {

		//treat as deltatime
		for (auto i = tileTimers.begin(); i != tileTimers.end(); i++) {
			i->second.timeAccumulator += t;

			if (i->second.timeAccumulator >= i->first.first) {
				i->second.timeAccumulator -= i->first.first;

				int oldFrame = i->second.currentFrame;
				i->second.currentFrame = (i->second.currentFrame + 1) % i->first.second;

				if (oldFrame != i->second.currentFrame) {
					int offset = (i->second.currentFrame - oldFrame) * tileSpacing;

					for (auto s = i->second.tilesAffected.begin(); s != i->second.tilesAffected.end(); s++) {
						
						auto spriteIt = sprites.find(*s);
						if (spriteIt == sprites.end())
							continue;

						sf::Sprite *spr = &spriteIt->second;
						sf::IntRect rect = spr->getTextureRect();
						rect.left += offset;
						spr->setTextureRect(rect);

					}
				}
			}
		}
	}
	else {

		//treat as zone global time
		for (auto i = tileTimers.begin(); i != tileTimers.end(); i++) {

			//the frame this should be on
			int nframe = (t % (i->first.first * (float)i->first.second)) / i->first.first;
			i->second.timeAccumulator = t % i->first.first;

			if (nframe != i->second.currentFrame) {
				int offset = (nframe - i->second.currentFrame) * tileSpacing;

				for (auto s = i->second.tilesAffected.begin(); s != i->second.tilesAffected.end(); s++) {
					
					auto spriteIt = sprites.find(*s);
					if (spriteIt == sprites.end())
						continue;

					sf::Sprite *spr = &spriteIt->second;
					sf::IntRect rect = spr->getTextureRect();
					rect.left += offset;
					spr->setTextureRect(rect);

				}
				i->second.currentFrame = nframe;
			}
		}
	}
}

void TileLayer::updateSpritePosition(sf::Vector2f pOffset, sf::Vector2f sOffset, sf::FloatRect pArea) {
	if (!isParallax() && !isScrolling())
		return;
	
	for (auto j = sprites.begin(); j != sprites.end(); j++) {
		sf::Vector2f tilePos(sf::Vector2f(j->first) * (float)tileSpacing);
		j->second.setPosition(snapToPixel(tilePos + pOffset + sOffset));
	}
}

void TileLayer::copy(const TileLayer &t) {
	sprites.clear();
	tiles.clear();
	tileTimers.clear();

	tiles = t.tiles;
	tileTimers = t.tileTimers;

	//parallax.mirrors.clear();
	parallax.area = t.parallax.area;
	parallax.isParallax = t.parallax.isParallax;
	parallax.offset = t.parallax.offset;
	parallax.parallaxRate = t.parallax.parallaxRate;
	parallax.scrollOffset = t.parallax.scrollOffset;
	parallax.scrollSize = t.parallax.scrollSize;
	parallax.scrollSpeed = t.parallax.scrollSpeed;
}

//generate sprite according to tile
void TileLayer::addTileSprite(Tile *tile, GridVector pos) {
	sf::Sprite spr;

	spr.setTexture(*rMan->getTexture(tilesetNames->at(tile->tileSprite)));
	spr.setPosition(floor(tile->gridPos.x * tileSpacing), floor(tile->gridPos.y * tileSpacing));

	sf::IntRect rect(tile->spritePos * (int)tileSpacing, sf::Vector2i(tileSpacing, tileSpacing));

	if (tile->tileProperty != TileProperty::TILE_ANIMATED) {
		spr.setTextureRect(rect);
	}
	else if (tile->anim_rate != sf::Time::Zero && tile->anim_framecount > 1) {
		//get tiletimer
		TileAnimTimer *timer = &tileTimers.at(std::make_pair(tile->anim_rate, tile->anim_framecount));
		rect.left += timer->currentFrame * tileSpacing;
		spr.setTextureRect(rect);
	}
	sprites.insert(std::pair<GridVector, sf::Sprite>(pos, spr));
}

//insert tile data into 
void TileLayer::setTile(sf::Vector2i gridPosition, int tileSpr, sf::Vector2i offset) {
	
	GridVector v;
	v.x = gridPosition.x;
	v.y = gridPosition.y;

	Tile t;
	t.gridPos = gridPosition;
	t.tileSprite = tileSpr;
	t.spritePos = offset;

	TileProperty::TileData tileData = TileProperty::getTileData(tilesetNames->at(t.tileSprite), GridVector(t.spritePos.x, t.spritePos.y));

	t.tileProperty = tileData.tileProperty;
	t.occluding = tileData.occluding;

	if (t.tileProperty == TileProperty::TILE_ANIMATED && tileData.animFrameRate != sf::Time::Zero && tileData.animFrameCount > 1) {
		std::pair<sf::Time, int> pair(tileData.animFrameRate, tileData.animFrameCount);

		auto at = tileTimers.find(pair);

		if (at == tileTimers.end()) {
			at = tileTimers.insert(std::make_pair(pair, TileAnimTimer())).first;
		}

		at->second.tilesAffected.insert(v);
		
		t.anim_rate = pair.first;
		t.anim_framecount = pair.second;
	}

	tiles[v] = t;
}

int TileLayer::getTileSpriteName(GridVector gridpos, int layer) const {
	auto i = tiles.find(gridpos);
	if (i != tiles.end())
		return tiles.at(gridpos).tileSprite;
	else
		return -1;
}

void TileLayer::syncTimersWithZone(sf::Time zoneTimer) {
	
	//update tile animation
	updateSpriteAnimation(zoneTimer, true);

	//update parallax
	if (isParallax()) {
		
		sf::Vector2f cOff(lastFrameDisplayArea.left, lastFrameDisplayArea.top);

		sf::Vector2f offset;
		offset.x = cOff.x * parallax.parallaxRate.x;
		offset.y = cOff.y * parallax.parallaxRate.y;

		sf::Vector2f snapped = snapToPixel(offset);

		sf::FloatRect parallaxArea(0.f, 0.f, (parallax.area.width * tileSpacing) - GAMEWIDTH, (parallax.area.height * tileSpacing) - GAMEHEIGHT);
		offset.x = parallaxArea.width > 0.f ? snapped.x : cOff.x;
		offset.y = parallaxArea.height > 0.f ? snapped.y : cOff.y;

		parallax.offset = offset;
		for (auto j = sprites.begin(); j != sprites.end(); j++) {
			sf::Vector2f pos;
			pos.x = (float)j->first.x * (float)tileSpacing + offset.x;
			pos.y = (float)j->first.y * (float)tileSpacing + offset.y;

			sf::Vector2f snappedOffset = snapToPixel(parallax.scrollOffset);
			j->second.setPosition(pos + snappedOffset);
		}
	}

	//update scrolling
	if (isScrolling()) {
		sf::Vector2f lastOffset = parallax.scrollOffset;
		parallax.scrollOffset = (parallax.scrollSpeed * zoneTimer.asSeconds());

		sf::FloatRect pArea = sf::FloatRect(0.f, 0.f, parallax.scrollSize.x, parallax.scrollSize.y);
		pArea.width *= tileSpacing;
		pArea.height *= tileSpacing;

		while (parallax.scrollOffset.x < 0)
			parallax.scrollOffset.x += pArea.width;
		while (parallax.scrollOffset.x >= pArea.width)
			parallax.scrollOffset.x -= pArea.width;

		while (parallax.scrollOffset.y < 0)
			parallax.scrollOffset.y += pArea.height;
		while (parallax.scrollOffset.y >= pArea.height)
			parallax.scrollOffset.y -= pArea.height;

		sf::Vector2f snappedOffset = snapToPixel(parallax.scrollOffset);
		sf::Vector2f snappedLastOffset = snapToPixel(lastOffset);
		if (floorf(snappedLastOffset.x / (float)tileSpacing) != floorf((float)snappedOffset.x / (float)tileSpacing) ||
			floorf(snappedLastOffset.y / (float)tileSpacing) != floorf((float)snappedOffset.y / (float)tileSpacing)) {
			
			for (auto j = sprites.begin(); j != sprites.end(); j++) {

				sf::Vector2f p = (sf::Vector2f(j->first.x, j->first.y) * (float)tileSpacing) + snappedOffset;
				p.x = p.x < 0 - tileSpacing ? p.x + pArea.width : p.x;
				p.x = p.x >= pArea.width ? p.x - pArea.width : p.x;

				p.y = p.y < 0 - tileSpacing ? p.y + pArea.height : p.y;
				p.y = p.y >= pArea.height ? p.y - pArea.height : p.y;

				j->second.setPosition(p + parallax.offset);
			}
		}
	}
}

void TileLayer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto i = sprites.begin(); i != sprites.end(); i++) {
		target.draw(i->second, states);
	}

#ifdef _DEBUG
	
	if (Gameplay_Globals::Debug::scrollingParallax && isScrolling()) {
		//scroll - red
		sf::RectangleShape test;
		test.setSize(sf::Vector2f(parallax.scrollSize.x, parallax.scrollSize.y) * (float)tileSpacing);
		test.setPosition(sf::Vector2f(parallax.offset + parallax.scrollOffset));
		test.setFillColor(sf::Color::Transparent);
		test.setOutlineColor(sf::Color(255, 0, 0, 255));
		test.setOutlineThickness(1.f);
		target.draw(test, states);
		
		//intersection - blue
		sf::RectangleShape testIntersection;
		testIntersection.setSize(sf::Vector2f(dintersection.width, dintersection.height) * (float)tileSpacing);
		testIntersection.setPosition(sf::Vector2f(dintersection.left, dintersection.top) * (float)tileSpacing + sf::Vector2f(parallax.offset + parallax.scrollOffset));
		testIntersection.setFillColor(sf::Color::Transparent);
		testIntersection.setOutlineColor(sf::Color(0, 0, 255, 255));
		testIntersection.setOutlineThickness(1.f);
		target.draw(testIntersection, states);

		//total affected area - green
		sf::RectangleShape testTotal;
		testTotal.setSize(sf::Vector2f(dtotalEffectedArea.width, dtotalEffectedArea.height) * (float)tileSpacing);
		testTotal.setPosition(sf::Vector2f(dtotalEffectedArea.left, dtotalEffectedArea.top) * (float)tileSpacing + sf::Vector2f(parallax.offset + parallax.scrollOffset));
		testTotal.setFillColor(sf::Color::Transparent);
		testTotal.setOutlineColor(sf::Color(0, 255, 0, 255));
		testTotal.setOutlineThickness(1.f);
		target.draw(testTotal, states);
	}
#endif // DEBUG
	
}