#ifndef UISLIDER_H
#define UISLIDER_H

#include "ui/UIElement.hpp"
#include "anim/AnimSprite.hpp"

class UISlider : public UIElement {
public:

	struct SpeedTier {
		SpeedTier(sf::Time t, int option) {
			timePerOption = t;
			optionCount = option;
		}

		sf::Time timePerOption;

		//this is ignored if this tier is the last one
		int optionCount;
	};

	UISlider(int min, int max, int step);

	void setActiveState(ActiveState st);
	
	void update(sf::Time deltaTime);

	static const sf::Vector2f sliderSize;

	void setPosition(sf::Vector2f pos);

	void captureDirInput(Cardinal d);
	void captureMouseMove(sf::Vector2f m_pos);

	int getIndex() { return curIndex; };
	int getValue() { return rangeMin + (curIndex * rangeStep); };
	void setIndex(int i) { curIndex = i; };

	void setSliderRange(int min, int max, int step);
	void setSpeedTiers(std::vector<SpeedTier> speeds);

private:

	Cardinal trackDir = Cardinal::NO_DIR;

	struct AutoScroller {
		std::vector<SpeedTier>::iterator curTier;
		int optionsCounted = 0;
		sf::Time timer = sf::Time::Zero;
	};
	std::vector<SpeedTier> trackSpeedTier;
	AutoScroller scroller;


	static const int sliderWidth;

	void moveCursor(int p);
	void updateCursorSprite();

	sf::Sprite sliderBase;
	sf::Sprite sliderCursor;

	int curIndex = 0;

	int rangeMin = 0;
	int rangeMax = 0;
	int rangeStep = 0;
	int rangeCount = 1;

	void setArea(sf::FloatRect) {};

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
