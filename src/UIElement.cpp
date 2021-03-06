#include "ui/UIElement.hpp"
#include "res/ResourceLoader.hpp"


UIElement::UIElement() {
	connections[NORTH] = nullptr;
	connections[EAST] = nullptr;
	connections[SOUTH] = nullptr;
	connections[WEST] = nullptr;

	capturesInput[NORTH] = false;
	capturesInput[EAST] = false;
	capturesInput[SOUTH] = false;
	capturesInput[WEST] = false;
};

bool UIElement::canCaptureDir(Cardinal d) {
	return capturesInput[d]; 
};
bool UIElement::canCaptureMouse() {
	return capturesMouse;
};
bool UIElement::isInteractive() {
	return interactive; 
};
bool UIElement::isActivationDelayed() {
	return delayedActivation;
}
bool UIElement::hasConnections() {
	return connections[NORTH]
		|| connections[EAST]
		|| connections[SOUTH]
		|| connections[WEST];
};

const sf::FloatRect UIElement::getArea() { 
	return area; 
};

UIElement::ActiveState UIElement::getActiveState() { 
	return state; 
};
void UIElement::setActiveState(ActiveState st) { 
	state = st; 
};

void UIElement::draw(sf::RenderTarget &target, sf::RenderStates states) const {
#if _DEBUG
	if (UI_Globals::Debug::UIDebug) {
		sf::RectangleShape rect(sf::Vector2f(area.width, area.height));
		rect.setPosition(sf::Vector2f(area.left, area.top));
		rect.setOutlineThickness(-1.f);
		switch (state) {
		case NOT_SELECTED: 
			rect.setFillColor(sf::Color(0, 0, 255, 128)); 
			rect.setOutlineColor(sf::Color::Black);
			break;
		case SELECTED:
			rect.setFillColor(sf::Color(255, 255, 0, 128));
			rect.setOutlineColor(sf::Color::Black);
			break;
		case ACTIVATED: rect.setFillColor(sf::Color(255, 0, 0, 128));
			rect.setOutlineColor(sf::Color(255, 255, 0));
			break;
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
			line.setRotation(Math::toDeg(Math::angle(end, start)));
			line.setPosition(start);
			target.draw(line, states);
		}
	}
#endif
}
