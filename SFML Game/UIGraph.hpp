#ifndef UISTATE_H
#define UISTATE_H

#include <SFML/Graphics.hpp>

#include "State.hpp"
#include "Controls.hpp"
#include "UIElement.hpp"

#include "Math.hpp"

#include "UIElement.hpp"
#include "UIText.hpp"

//incomplete

class UIGraph : public sf::Drawable {
public:
	UIGraph();
	~UIGraph();

	void update(sf::Time deltaTime);

	UIText* createUIText();

	void addElement(UIElement* e);

	void setSelected(UIElement* e);

protected:

	// current selection
	UIElement* sElement = nullptr;
	// last valid element
	UIElement* lastElement = nullptr;
	// last element in activated state
	UIElement* lastPressed = nullptr;

	bool mousePressedLastFrame = false;
	bool inputPressedLastFrame = false;

	std::vector<UIElement*> uiInteractive;
	std::vector<UIElement*> uiStatic;
	
	void activateElement();

	void changeSelection(UIElement* to);
	
	UIElement* findElementUnderMouse();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif