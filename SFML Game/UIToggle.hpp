#ifndef UITOGGLE_H
#define UITOGGLE_H

#include "AnimSprite.hpp"

#include "UIElement.hpp"

class UIToggle : public UIElement {
public:


	UIToggle();

	void setActiveState(ActiveState st);

	void update(sf::Time deltaTime);

	static const sf::Vector2f toggleSize;

	void setPosition(sf::Vector2f pos);

	//woops forgot which widget i was coding this is for UISlider
	//void captureDirInput(Cardinal d);
	//void captureMouseMove(sf::Vector2f m_pos);
	
	//inline unsigned int getIndex() { return curIndex; };
	//inline void setIndex(unsigned __int8 i) {};

	void activate();

private:

	//unsigned __int8 curIndex = 0;

	bool toggleState = false;

	void setArea(sf::FloatRect a) {};

	AnimSprite spr;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

};

#endif