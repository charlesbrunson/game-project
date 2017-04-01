#include "TransitionVerticalState.hpp"

TransitionVerticalState::TransitionVerticalState(GameplayState *game, Transition *activatedTransition, ResourceLoader *r) :
TransitionState(game, activatedTransition, r) {

	toUp = (transition.dir == Cardinal::NORTH);

	offsetIntoLevel = (float)tileSpacing;

	Level* curLvl = zone->getCurrentLevel();
	if (plr != nullptr) {
		plrEnd = sf::Vector2f(
			plrStart.x,
			!toUp ? curLvl->levelArea.height + (offsetIntoLevel * 2.f) + 0.f : -2.f);
	}

	sf::Vector2i currentLevelSize(curLvl->levelArea.width, curLvl->levelArea.height);
	sf::Vector2i nextLevelSize(toLevelArea->level->levelArea.width, toLevelArea->level->levelArea.height);
	toLvlOffset = sf::Vector2f(transition.offset, toUp ? nextLevelSize.y : -currentLevelSize.y);

	sf::FloatRect offsetArea(toLevelArea->level->levelArea);
	offsetArea.left -= toLvlOffset.x;
	offsetArea.top -= toLvlOffset.y;

	//cam end point
	{
		camEnd = camStart + sf::Vector2f(-transition.offset, GAMEHEIGHT * (toUp ? -1 : 1));
		camEnd.x = std::min(std::max(offsetArea.left + GAMEWIDTH / 2, camEnd.x), offsetArea.left + offsetArea.width - (GAMEWIDTH / 2));
		camEnd.y = std::min(std::max(offsetArea.top + GAMEHEIGHT / 2, camEnd.y), offsetArea.top + offsetArea.height - (GAMEHEIGHT / 2));

		if (offsetArea.width < GAMEWIDTH)
			camEnd.x = offsetArea.left + (offsetArea.width / 2);
		if (offsetArea.height < GAMEHEIGHT)
			camEnd.y = offsetArea.top + (offsetArea.height / 2);
	}

	initCovers(offsetArea);

	bgUp = toUp ? toLevelArea->level->getBGSprite() : curLvl->getBGSprite();
	bgDown = !toUp ? toLevelArea->level->getBGSprite() : curLvl->getBGSprite();
	updateBGSprites();

	initNextObjects();
};

void TransitionVerticalState::updateBGSprites() {
	if (fract < 1.f) {
		bgUp.setPosition(snapToPixel(gCameraPos - GAMEDIMENSIONS / 2.f));
		bgUp.setTextureRect(
			sf::IntRect(
			0,
			0,
			GAMEWIDTH,
			(float)GAMEHEIGHT * (toUp ? fract : 1 - fract)));
		bgDown.setPosition(snapToPixel(bgUp.getPosition() + sf::Vector2f(0.f, bgUp.getTextureRect().height)));
		bgDown.setTextureRect(
			sf::IntRect(
			0,
			bgUp.getTextureRect().height,
			GAMEWIDTH,
			GAMEHEIGHT - bgUp.getTextureRect().height));
	}
	else {
		if (toUp) {
			bgUp.setPosition(gCameraPos - GAMEDIMENSIONS / 2.f);
			bgUp.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
		}
		else {
			bgDown.setPosition(gCameraPos - GAMEDIMENSIONS / 2.f);
			bgDown.setTextureRect(sf::IntRect(0, 0, GAMEWIDTH, GAMEHEIGHT));
		}
	}
}


void TransitionVerticalState::initCovers(const sf::FloatRect &offsetArea) {
	const sf::FloatRect lvlArea = sf::FloatRect(zone->getCurrentLevel()->levelArea);
	const sf::FloatRect up(toUp ? offsetArea : lvlArea);
	const sf::FloatRect down(!toUp ? offsetArea : lvlArea);

	sf::FloatRect totalArea;
	totalArea.left = std::min(offsetArea.left, lvlArea.left);
	totalArea.top = std::min(offsetArea.top, lvlArea.top);
	totalArea.width = std::max(offsetArea.left + offsetArea.width, lvlArea.left + lvlArea.width) - totalArea.left;
	totalArea.height = std::max(offsetArea.top + offsetArea.height, lvlArea.top + lvlArea.height) - totalArea.top;

	sf::FloatRect upRegion = totalArea;
	upRegion.top = up.top;
	upRegion.height = up.height;

	sf::FloatRect downRegion = totalArea;
	downRegion.top = down.top;
	downRegion.height = down.height;

	//top covers
	if (up.left > upRegion.left) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(upRegion.left, upRegion.top));
		covers.back().setSize(sf::Vector2f(up.left - upRegion.left, upRegion.height));
	}
	if (up.left + up.width < upRegion.left + upRegion.width) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(up.left + up.width, up.top));
		covers.back().setSize(sf::Vector2f((upRegion.left + upRegion.width) - (up.left + up.width), upRegion.height));
	}

	//bottom covers
	if (down.left > downRegion.left) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(downRegion.left, downRegion.top));
		covers.back().setSize(sf::Vector2f(down.left - downRegion.left, downRegion.height));
	}
	if (down.left + down.width < downRegion.left + downRegion.width) {
		covers.push_back(sf::RectangleShape());
		covers.back().setFillColor(sf::Color::Black);
		covers.back().setPosition(sf::Vector2f(down.left + down.width, down.top));
		covers.back().setSize(sf::Vector2f((downRegion.left + downRegion.width) - (down.left + down.width), downRegion.height));
	}
}

void TransitionVerticalState::drawBG(sf::RenderTarget &target, sf::RenderStates states) const {
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
		if (bgUp.getTexture())
			target.draw(bgUp, states);

		if (bgDown.getTexture())
			target.draw(bgDown, states);
	}
	else if ((toUp ? bgUp : bgDown).getTexture()) {
		target.draw(toUp ? bgUp : bgDown, states);
	}
}