#include "ui/UIToggle.hpp"
#include "res/ResourceLoader.hpp"

#include "util/Log.hpp"

const sf::Vector2f UIToggle::toggleSize = sf::Vector2f(32.f, 32.f);

UIToggle::UIToggle() {
	//capturesInput[EAST] = true;
	//capturesInput[WEST] = true;

	interactive = true;
	delayedActivation = true;
	//capturesMouse = true;
}

void UIToggle::setActiveState(UIElement::ActiveState st) {
	state = st;
}

void UIToggle::update(sf::Time) {

}

void UIToggle::setPosition(sf::Vector2f pos) {
	area = sf::FloatRect(pos.x, pos.y, toggleSize.x, toggleSize.y);
}

void UIToggle::activate() {
	toggleState = !toggleState;
	Log::msg(std::to_string(toggleState));
}

void UIToggle::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	UIElement::draw(target, states);


}

/*
void captureDirInput(Cardinal d) {

}

void captureMouseMove(sf::Vector2f m_pos) {

}
*/
