#ifndef UITOGGLE_H
#define UITOGGLE_H

#include "anim/AnimSprite.hpp"

#include "ui/UIElement.hpp"

class UIToggle : public UIElement {
public:


	UIToggle();

	void setActiveState(ActiveState st);

	void update(sf::Time deltaTime);

	static const sf::Vector2f toggleSize;

	void setPosition(sf::Vector2f pos);

	void activate();

private:
	
	bool toggleState = false;

	void setArea(sf::FloatRect) {};

	AnimSprite spr;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

};

#endif
