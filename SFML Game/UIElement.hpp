#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <functional>

#include <SFML/Graphics.hpp>
#include "Globals.hpp"

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

	};

	UIElement* getConnected(Direction dir) {
		return connections[dir];
	};

	const sf::FloatRect getArea()	{ return area; };
	ActiveState getActiveState() { return state; };
	
	std::function<void()> onActivate;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
#if _DEBUG
		if (UI_Globals::Debug::UIDebug) {
			sf::RectangleShape rect(sf::Vector2f(area.width, area.height));
			rect.setPosition(sf::Vector2f(area.left, area.top));
			switch (state) {
			case NOT_SELECTED: rect.setFillColor(sf::Color::Blue); break;
			case SELECTED: rect.setFillColor(sf::Color::Yellow); break;
			case ACTIVATED: rect.setFillColor(sf::Color::White); break;
			}
			target.draw(rect, states);
		}
		if (UI_Globals::Debug::UIConnectors && state == SELECTED) {
			for (int i = 0; i < DIR_COUNT; i++) {
				if (connections[i] == nullptr)
					continue;

				sf::FloatRect conArea = connections[i]->getArea();

				sf::Vector2f start(area.left + area.width / 2.f, area.top + area.height / 2.f);
				sf::Vector2f end(conArea.left + conArea.width / 2.f, conArea.top + conArea.height / 2.f);
				sf::Vector2f diff(start.x - end.x, start.y - end.y);
				float dist = sqrtf(diff.x * diff.x + diff.y * diff.y);

				sf::RectangleShape line(sf::Vector2f(dist, 2.f));
				line.setOrigin(sf::Vector2f(0.f, 1.f));
				line.setFillColor(sf::Color::Green);
				line.setRotation(atanf(diff.y / diff.x));
				target.draw(line, states);
			}
		}
#endif
	}

protected:

	sf::FloatRect area;
	ActiveState state = NOT_SELECTED;
	UIElement* connections[DIR_COUNT] = { nullptr };
};

#endif