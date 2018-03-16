#ifndef UISTATE_H
#define UISTATE_H

#include <SFML/Graphics.hpp>

#include "State.hpp"
#include "Controls.hpp"
#include "ui/UIElement.hpp"

#include "util/Math.hpp"

#include "ui/UIElement.hpp"
#include "ui/UIText.hpp"
#include "ui/UIButton.hpp"
#include "ui/UIToggle.hpp"
#include "ui/UISlider.hpp"

//incomplete

class UIGraph : public sf::Drawable {
public:
	UIGraph();
	~UIGraph();

	void update(sf::Time deltaTime);

	UIText* createUIText();
	UIButton* createUIButton(std::string label = "");
	UIToggle* createUIToggle();
	UISlider* createUISlider(int min, int max, int step);

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
