
#include "UIPauseState.hpp"
#include "CamRef.hpp"

UIPauseState::UIPauseState(State* prevState, ResourceLoader *r) : State(r), uiGraph(r) {

	prev = prevState;
	cameraPos = prev->cameraPos;

	pauseText.setFont(*r->getFont("pixelated.ttf"));
	pauseText.setCharacterSize(8);
	pauseText.setFillColor(sf::Color::White);
	pauseText.setOutlineColor(sf::Color::Black);
	pauseText.setOutlineThickness(1.f);

	UIText* t = uiGraph.createUIText();
	t->setArea(getCameraArea());
	t->copyTextOptions(pauseText);
	t->setAlignment(UIText::CENTER, UIText::CENTER);
	t->setString("PAUSED");
	t->updateText();
}

void UIPauseState::update(sf::Time deltaTime) {
	uiGraph.update(deltaTime);

	if (!toNextState && Controls::isPressed(Controls::Input::START)) {
		Controls::confirmPress(Controls::Input::START);
		resume();
	}
}

void UIPauseState::resume() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = prev;
}

void UIPauseState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(*prev, states);
	target.draw(uiGraph, states);
};