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

	UIElement() {
		connections[NORTH] = nullptr;
		connections[EAST] = nullptr;
		connections[SOUTH] = nullptr;
		connections[WEST] = nullptr;

		capturesInput[NORTH] = false;
		capturesInput[EAST] = false;
		capturesInput[SOUTH] = false;
		capturesInput[WEST] = false;
	};

	UIElement* connections[DIR_COUNT];
	bool capturesDir(Direction d) {
		return capturesInput[d];
	}


	virtual void setArea(sf::FloatRect a) {
		area = a;
	}

	bool isInteractive() { return interactive; };
	bool hasConnections() {
		return connections[NORTH] 
			|| connections[EAST] 
			|| connections[SOUTH] 
			|| connections[WEST];
	}


	const sf::FloatRect getArea()	{ return area; };
	ActiveState getActiveState() { return state; };
	void setActiveState(ActiveState st) { state = st; };
	
	//std::function<void()> onActivate;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
#if _DEBUG
		if (UI_Globals::Debug::UIDebug) {
			sf::RectangleShape rect(sf::Vector2f(area.width, area.height));
			rect.setPosition(sf::Vector2f(area.left, area.top));
			switch (state) {
			case NOT_SELECTED: rect.setFillColor(sf::Color(0, 0, 255, 128)); break;
			case SELECTED: rect.setFillColor(sf::Color(255, 255, 0, 128)); break;
			case ACTIVATED: rect.setFillColor(sf::Color(255, 255, 255, 128)); break;
			}
			target.draw(rect, states);
		}
		if (UI_Globals::Debug::UIConnectors && state == SELECTED) {
			for (int i = 0; i < DIR_COUNT; i++) {
				if (!connections[i])
					continue;

				sf::FloatRect conArea = connections[i]->getArea();

				sf::Vector2f start = Math::center(area);
				sf::Vector2f end = Math::center(conArea);

				sf::RectangleShape line(sf::Vector2f(Math::dist(start, end), 1.f));
				line.setOrigin(sf::Vector2f(0.f, 0.5f));
				line.setFillColor(sf::Color::Green);
				line.setRotation(Math::toDeg(-Math::angle(start, end)));
				line.setPosition(start);
				target.draw(line, states);
			}
		}
#endif
	}

protected:
	
	bool interactive = false;
	sf::FloatRect area;
	ActiveState state = NOT_SELECTED;

	//for slider widgets, where left and right movements are captured
	bool capturesInput[DIR_COUNT];
};

#endif