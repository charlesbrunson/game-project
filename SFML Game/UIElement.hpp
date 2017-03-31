#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <functional>

#include <SFML/Graphics.hpp>
#include "Globals.hpp"
#include "Math.hpp"

class UIElement : public sf::Drawable {
public:
	enum ActiveState {
		NOT_SELECTED,
		SELECTED,
		ACTIVATED
	};
	enum Direction : int {
		NORTH = 0,
		EAST,
		SOUTH,
		WEST,
		DIR_COUNT
	};

	UIElement();

	UIElement* connections[DIR_COUNT];
	bool hasConnections();
	bool capturesDir(Direction d);

	std::function<void(void)> onActivate;
	const bool isInteractive();
	const bool isActivationDelayed();
	
	virtual void setArea(sf::FloatRect a);
	const sf::FloatRect getArea();

	ActiveState getActiveState();
	void setActiveState(ActiveState st);
	
	virtual void captureDirInput() {};
	virtual void captureMouseMove(sf::Vector2f m_pos) {};
	
protected:
	
	sf::FloatRect area;
	ActiveState state = NOT_SELECTED;

	bool interactive = false;

	//false = activate on press, true = activate on release (and is still selected)
	//necessary for buttons
	bool delayedActivation = false;


	bool capturesInput[DIR_COUNT];
	bool capturesMouse = false;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
