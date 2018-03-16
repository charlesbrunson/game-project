#include "ui/UISlider.hpp"
#include "res/ResourceLoader.hpp"

#include "util/Log.hpp"
#include "Controls.hpp"

#include <assert.h>

const sf::Vector2f UISlider::sliderSize = sf::Vector2f(100.f, 10.f);
const int UISlider::sliderWidth = 100;

UISlider::UISlider(int min, int max, int step) {
	setSliderRange(min, max, step);

	capturesInput[Cardinal::EAST] = true;
	capturesInput[Cardinal::WEST] = true;

	capturesMouse = true;
	
	sliderBase.setTexture(RL()->getTexture("sprites/ui.png"));
	sliderBase.setTextureRect(sf::IntRect(sf::Vector2i(19, 51), sf::Vector2i(102, 10)));
	sliderBase.setOrigin(sf::Vector2f(1.f, 0.f));

	sliderCursor.setTexture(RL()->getTexture("sprites/ui.png"));
	sliderCursor.setTextureRect(sf::IntRect(sf::Vector2i(0, 48), sf::Vector2i(5, 16)));
	sliderCursor.setOrigin(2.f, 3.f);


	trackSpeedTier.push_back(SpeedTier(sf::seconds(1.f), 1));
	trackSpeedTier.push_back(SpeedTier(sf::seconds(0.1f), 0));
	scroller.curTier = trackSpeedTier.begin();

	interactive = true;
	delayedActivation = true;
}

void UISlider::setSliderRange(int min, int max, int step) {
	assert((max - min) % step == 0 && (max - min) <= 101);

	rangeMin = min;
	rangeMax = max;
	rangeStep = step;
	rangeCount = (rangeMax - rangeMin + 1) / rangeStep;
}

void UISlider::setActiveState(ActiveState st) {

	if (st == NOT_SELECTED) {
		trackDir = NO_DIR;
		//trackTimer = sf::Time::Zero;
	}

	UIElement::setActiveState(st);
}

void UISlider::update(sf::Time deltaTime) {

	if (state == ActiveState::SELECTED && trackDir != NO_DIR) {
		if (trackDir == EAST && Controls::isHeld(Controls::dirMap[EAST])) {

			scroller.timer -= deltaTime;
			if (scroller.timer <= sf::Time::Zero) {
				moveCursor(curIndex + 1);
				scroller.optionsCounted++;

				if (scroller.curTier + 1 != trackSpeedTier.end() 
					&& scroller.optionsCounted >= scroller.curTier->optionCount) {

					scroller.optionsCounted = 0;
					scroller.curTier++;
				}
				scroller.timer = scroller.curTier->timePerOption;
			}

		}
		else if (trackDir == WEST && Controls::isHeld(Controls::dirMap[WEST])) {

			scroller.timer -= deltaTime;
			if (scroller.timer <= sf::Time::Zero) {
				moveCursor(curIndex - 1);
				scroller.optionsCounted++;

				if (scroller.curTier + 1 != trackSpeedTier.end()
					&& scroller.optionsCounted >= scroller.curTier->optionCount) {

					scroller.optionsCounted = 0;
					scroller.curTier++;
				}
				scroller.timer = scroller.curTier->timePerOption;
			}

		}
		else {
			trackDir = NO_DIR;
		}
	}
}

void UISlider::setPosition(sf::Vector2f pos) {
	area = sf::FloatRect(pos.x, pos.y, sliderSize.x, sliderSize.y);
	sliderBase.setPosition(pos);
	updateCursorSprite();
}

void UISlider::captureDirInput(Cardinal d) {

	if (d == Cardinal::EAST) {
		curIndex++;
		curIndex = std::min(curIndex, rangeCount - 1);
		Log::msg(std::to_string(getValue()));
		trackDir = EAST;
	}
	else if (d == Cardinal::WEST) {
		curIndex--;
		curIndex = std::max(curIndex, 0);
		Log::msg(std::to_string(getValue()));
		trackDir = WEST;
	}

	scroller.curTier = trackSpeedTier.begin();
	scroller.timer = scroller.curTier->timePerOption;
	scroller.optionsCounted = 0;

	updateCursorSprite();
}

void UISlider::captureMouseMove(sf::Vector2f m_pos) {

	sf::Vector2f p = m_pos - Math::topleft(area);
	float space = sliderSize.x / rangeCount;
	p.x /= space;

	moveCursor( (int)ceilf(p.x - 0.5f) );
}

void UISlider::moveCursor(int p) {
	curIndex = std::max(0, std::min(p, rangeCount - 1));
	Log::msg(std::to_string(getValue()));
	updateCursorSprite();
}

void UISlider::updateCursorSprite() {
	float ratio = (float)curIndex / (float)rangeCount;
	sliderCursor.setPosition(sliderBase.getPosition() + sf::Vector2f(ratio * sliderSize.x, 0.f));
}

void UISlider::setSpeedTiers(std::vector<SpeedTier> speeds) {
	trackSpeedTier = speeds;

	scroller.curTier = trackSpeedTier.begin();
	scroller.optionsCounted = 0;
	scroller.timer = sf::Time::Zero;
};

void UISlider::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	UIElement::draw(target, states);
	target.draw(sliderBase, states);
	target.draw(sliderCursor, states);
}
