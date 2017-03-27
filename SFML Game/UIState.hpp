#ifndef UISTATE_H
#define UISTATE_H

#include <SFML/Graphics.hpp>

#include "State.hpp"
#include "UIElement.hpp"
#include "Controls.hpp"

//incomplete

class UIState : public State {
public:
	UIState(ResourceLoader *r);

	void update(sf::Time deltaTime);

	virtual ~UIState();

protected:

	//current selection
	UIElement* sElement = nullptr;
	//last valid element
	UIElement* lastElement = nullptr;
	//default selection
	UIElement* defaultElement = nullptr;



	std::map<std::string, UIElement*> uiElements;

	
	static const std::map<Controls::Input, UIElement::Direction> dirMap;

	void changeSelection(UIElement* to);

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif