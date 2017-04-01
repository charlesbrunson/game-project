#ifndef UISTATE_H
#define UISTATE_H

#include <SFML/Graphics.hpp>

#include "State.hpp"
#include "Controls.hpp"
#include "UIElement.hpp"

#include "Math.hpp"

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

	bool mousePressedLastFrame = false;
	bool inputPressedLastFrame = false;
	UIElement* lastPressed = nullptr;

	std::vector<UIElement*> uiElements;
	
	void UIState::activateElement();

	void changeSelection(UIElement* to);

	UIElement* findElementUnderMouse();

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif