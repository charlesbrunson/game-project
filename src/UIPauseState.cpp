
#include "UIPauseState.hpp"
#include "CamRef.hpp"

#include "UIMainMenu.hpp"

UIPauseState::UIPauseState(State* prevState) {

	prev = prevState;
	cameraPos = prev->cameraPos;

	pauseText.setFont(RL()->getFont("pixelated.ttf"));
	pauseText.setCharacterSize(8);
	pauseText.setFillColor(sf::Color::White);
	pauseText.setOutlineColor(sf::Color::Black);
	pauseText.setOutlineThickness(1.f);

	sf::Vector2f pos;
	pos.x = Math::center(getCameraArea()).x - 64.f / 2.f;
	pos.y = Math::center(getCameraArea()).y - 16.f - 16.f / 2.f;

	UIButton* t1 = uiGraph.createUIButton("RESUME");
	t1->setPosition(pos);
	t1->onActivate = std::bind(&UIPauseState::resume, this);

	pos.y += 32.f;
	UIButton* t2 = uiGraph.createUIButton("QUIT TO MENU");
	t2->setPosition(pos);
	t2->onActivate = std::bind(&UIPauseState::quitToMenu, this);

	t1->connections[Cardinal::SOUTH] = t2;
	t2->connections[Cardinal::NORTH] = t1;

	uiGraph.setSelected(t1);

	backPanel.setFillColor(sf::Color(60, 166, 244));
	backPanel.setSize(sf::Vector2f(80.f, 64.f));
	backPanel.setPosition(Math::center(getCameraArea()) - backPanel.getSize() / 2.f);

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
	delete prev;
	nextState = new UIMainMenu();
}

void UIPauseState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(*prev, states);
	target.draw(backPanel, states);
	target.draw(uiGraph, states);
};