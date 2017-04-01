#include "TransitionHorizontalState.hpp"

#include "Math.hpp"

TransitionHorizontalState::TransitionHorizontalState(GameplayState *game, Transition *activatedTransition, ResourceLoader *r) :
TransitionState(game, activatedTransition, r) {

	toLeft = (transition.dir == Cardinal::WEST);
	offsetIntoLevel = (float)tileSpacing / 2.f;

	Level* curLvl = zone->getCurrentLevel();
	if (plr != nullptr) {
		plrEnd = sf::Vector2f(
			!toLeft ? curLvl->levelArea.width + offsetIntoLevel : -offsetIntoLevel,
			plrStart.y);
	}

	sf::Vector2i currentLevelSize(curLvl->levelArea.width, curLvl->levelArea.height);
	sf::Vector2i nextLevelSize(toLevelArea->level->levelArea.width, toLevelArea->level->levelArea.height);
	toLvlOffset = sf::Vector2f(toLeft ? nextLevelSize.x : -currentLevelSize.x, transition.offset);

	sf::FloatRect offsetArea(toLevelArea->level->levelArea);
	offsetArea.left -= toLvlOffset.x;
	offsetArea.top -= toLvlOffset.y;

	//cam end point
	{
		camEnd = camStart + sf::Vector2f(GAMEWIDTH * (toLeft ? -1 : 1), -transition.offset);
		camEnd.x = std::min(std::max(offsetArea.left + GAMEWIDTH / 2, camEnd.x), offsetArea.left + offsetArea.width - (GAMEWIDTH / 2));
		camEnd.y = std::min(std::max(offsetArea.top + GAMEHEIGHT / 2, camEnd.y), offsetArea.top + offsetArea.height - (GAMEHEIGHT / 2));

		if (offsetArea.width < GAMEWIDTH)
			camEnd.x = offsetArea.left + (offsetArea.width / 2);
		if (offsetArea.height < GAMEHEIGHT)
			camEnd.y = offsetArea.top + (offsetArea.height / 2);
	}

	initCovers(offsetArea);

	bgLeft = toLeft ? toLevelArea->level->getBGSprite() : curLvl->getBGSprite();
	bgRight = !toLeft ? toLevelArea->level->getBGSprite() : curLvl->getBGSprite();
	updateBGSprites();

	initNextObjects();
};

void TransitionHorizontalState::updateBGSprites() {
	if (fract < 1.f) {
		bgLeft.setPosition(snapToPixel(gCameraPos - GAMEDIMENSIONS / 2.f));
		bgLeft.setTextureRect(
			sf::IntRect(
			0,
			0,
			(float)GAMEWIDTH * (toLeft ? fract : 1 - fract),
			GAMEHEIGHT));
		bgRight.setPosition(snapToPixel(bgLeft.getPosition() + sf::Vector2f(bgLeft.getTextureRect().width, 0.f)));
		bgRight.setTextureRect(
			sf::IntRect(
			bgLeft.getTextureRect().width,
			0,
			GAMEWIDTH - bgLeft.getTextureRect().width,
			GAMEHEIGHT));
	}
	else {
		if (toLeft) {
			bgLeft.setPosition(gCameraPos - GAMEDIMENSIONS / 2.f);
			bgLeft.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
		}
		else {
			bgRight.setPosition(gCameraPos - GAMEDIMENSIONS / 2.f);
			bgRight.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
		}
	}
}


void TransitionHorizontalState::initCovers(const sf::FloatRect &offsetArea) {
	const sf::FloatRect lvlArea = sf::FloatRect(zone->getCurrentLevel()->levelArea);
	const sf::FloatRect left(toLeft ? offsetArea : lvlArea);
	const sf::FloatRect right(!toLeft ? offsetArea : lvlArea);
	
	sf::FloatRect totalArea;
	totalArea.left = std::min(offsetArea.left, lvlArea.left);
	totalArea.top = std::min(offsetArea.top, lvlArea.top);
	totalArea.width = std::max(offsetArea.left + offsetArea.width, lvlArea.left + lvlArea.width) - totalArea.left;
	totalArea.height = std::max(offsetArea.top + offsetArea.height, lvlArea.top + lvlArea.height) - totalArea.top;

	sf::FloatRect leftRegion = totalArea;
	leftRegion.left = left.left;
	leftRegion.width = left.width;

	sf::FloatRect rightRegion = totalArea;
	rightRegion.left = right.left;
	rightRegion.width = right.width;

	//left covers
	if (left.top > leftRegion.top) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(leftRegion.left, leftRegion.top));
		covers.back().setSize(sf::Vector2f(leftRegion.width, left.top - leftRegion.top));
	}
	if (left.top + left.height < leftRegion.top + leftRegion.height) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(leftRegion.left, left.top + left.height));
		covers.back().setSize(sf::Vector2f(leftRegion.width, (leftRegion.top + leftRegion.height) - (left.top + left.height)));
	}

	//right covers
	if (right.top > rightRegion.top) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(rightRegion.left, rightRegion.top));
		covers.back().setSize(sf::Vector2f(rightRegion.width, right.top - rightRegion.top));
	}
	if (right.top + right.height < rightRegion.top + rightRegion.height) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(rightRegion.left, right.top + right.height));
		covers.back().setSize(sf::Vector2f(rightRegion.width, (rightRegion.top + rightRegion.height) - (right.top + right.height)));
	}
}

void TransitionHorizontalState::drawBG(sf::RenderTarget &target, sf::RenderStates states) const {
	Level* lvl = zone->getCurrentLevel();
	Level* nextLevel = toLevelArea->level;

	//current level
	{
		//draw currentlevel bg color, only where tolevel is not
		sf::FloatRect area = getCameraArea();

		area.intersects(sf::FloatRect(lvl->levelArea), area);

		sf::RectangleShape bgColor;
		bgColor.setFillColor(lvl->getBGColor());
		bgColor.setSize(sf::Vector2f(area.width, area.height));
		bgColor.setPosition(sf::Vector2f(area.left, area.top));
		target.draw(bgColor, states);
	}

	//to level
	{
		sf::FloatRect area = getCameraArea();
		sf::FloatRect toArea(nextLevel->levelArea);
		toArea.left -= toLvlOffset.x;
		toArea.top -= toLvlOffset.y;

		area.intersects(toArea, area);

		sf::RectangleShape bgColor;
		bgColor.setFillColor(nextLevel->getBGColor());
		bgColor.setSize(sf::Vector2f(area.width, area.height));
		bgColor.setPosition(sf::Vector2f(area.left, area.top));
		target.draw(bgColor, states);
	}

	if (fract < 1.f) {
		if (bgLeft.getTexture()) 
			target.draw(bgLeft, states);

		if (bgRight.getTexture()) 
			target.draw(bgRight, states);
	}
	else if ((toLeft ? bgLeft : bgRight).getTexture()) {
		target.draw(toLeft ? bgLeft : bgRight, states);
	}
}