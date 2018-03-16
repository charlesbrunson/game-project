#ifndef UIBUTTON_H
#define UIBUTTON_H

#include "ui/UIElement.hpp"
#include "ui/UIText.hpp"
#include "anim/AnimSprite.hpp"

class UIButton : public UIElement {
public:
	UIButton(std::string buttonLabel);

	void setActiveState(ActiveState st);

	//void setArea(sf::FloatRect a);

	void update(sf::Time deltaTime);

	static const sf::Vector2f buttonSize;

	void setPosition(sf::Vector2f pos);

	void activate() { onActivate(); };
	std::function<void(void)> onActivate;
private:

	void setArea(sf::FloatRect) {};

	//static Animation anim_button_notselected;
	//static Animation anim_button_selected;
	//static Animation anim_button_activated;

	AnimSprite spr;

	UIText bText;
	
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
