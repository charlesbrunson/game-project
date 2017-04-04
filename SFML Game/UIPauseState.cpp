
#include "UIPauseState.hpp"
#include "CamRef.hpp"

#include "UIMainMenu.hpp"

UIPauseState::UIPauseState(State* prevState) {

	prev = prevState;
	cameraPos = prev->cameraPos;

	pauseText.setFont(*RL()->getFont("pixelated.ttf"));
	pauseText.setCharacterSize(8);
	pauseText.setFillColor(sf::Color::White);
	pauseText.setOutlineColor(sf::Color::Black);
	pauseText.setOutlineThickness(1.f);

	sf::FloatRect area;
	area.width = 64.f;
	area.height = 16.f;
	area.left = Math::center(getCameraArea()).x - area.width / 2.f;
	area.top = Math::center(getCameraArea()).y - 16.f - area.height / 2.f;

	UIText* t1 = uiGraph.createUIText();
	t1->setArea(area);
	t1->copyTextOptions(pauseText);
	t1->setAlignment(UIText::CENTER, UIText::CENTER);
	t1->setString("RESUME");
	t1->updateText();
	t1->onActivate = std::bind(&UIPauseState::resume, this);

	area.top += 32.f;
	UIText* t2 = uiGraph.createUIText();
	t2->setArea(area);
	t2->copy(*t1);
	t2->setArea(area);
	t2->setString("QUIT TO MENU");
	t2->updateText();
	t2->onActivate = std::bind(&UIPauseState::quitToMenu, this);

	t1->connections[Cardinal::SOUTH] = t2;
	t2->connections[Cardinal::NORTH] = t1;

	uiGraph.setSelected(t1);

}

void UIPauseState::update(sf::Time deltaTime) {
	uiGraph.update(deltaTime);
}

void UIPauseState::resume() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = prev;
}

void UIPauseState::quitToMenu() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = new UIMainMenu();
}

void UIPauseState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(*prev, states);
	target.draw(uiGraph, states);
};