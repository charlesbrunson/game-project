#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <functional>

#include <SFML/Graphics.hpp>
#include "Globals.hpp"
#include "util/Math.hpp"

class UIElement : public sf::Drawable {
public:
	enum ActiveState {
		NOT_SELECTED,
		SELECTED,
		ACTIVATED
	};

	UIElement();

	UIElement* connections[Cardinal::DIR_COUNT];
	bool hasConnections();
	bool canCaptureDir(Cardinal d);
	bool canCaptureMouse();

	bool isInteractive();
	bool isActivationDelayed();
	
	virtual void update(sf::Time) {};

	virtual void setArea(sf::FloatRect a) = 0;
	virtual void setPosition(sf::Vector2f pos) = 0;
	const sf::FloatRect getArea();

	ActiveState getActiveState();
	virtual void setActiveState(ActiveState st);
	
	virtual void captureDirInput(Cardinal) {};
	virtual void captureMouseMove(sf::Vector2f) {};
	
	virtual void activate() {};

protected:
	
	sf::FloatRect area;
	ActiveState state = NOT_SELECTED;

	bool interactive = false;

	//false = activate on press, true = activate on release (and is still selected)
	//necessary for buttons
	bool delayedActivation = false;


	bool capturesInput[Cardinal::DIR_COUNT];
	bool capturesMouse = false;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
