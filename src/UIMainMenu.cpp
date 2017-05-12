
#include "CamRef.hpp"
#include "Log.hpp"

#include "GameplayState.hpp"
#include "UIMainMenu.hpp"
#include "UIOptions.hpp"

UIMainMenu::UIMainMenu() {
	cameraPos = GAMEDIMENSIONS / 2.f;
	gCameraPos = cameraPos;

	sf::Text text("", RL()->getFont("fonts/pixelated.ttf"), 8);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);

	UIButton* t1 = uiGraph.createUIButton("START");
	t1->setPosition(sf::Vector2f(25.f, 25.f));

	UIButton* t2 = uiGraph.createUIButton("OPTIONS");
	t2->setPosition(sf::Vector2f(25.f, 25.f + 32.f));

	UIButton* t3 = uiGraph.createUIButton("QUIT");
	t3->setPosition(sf::Vector2f(25.f, 25.f + 64.f));

	t1->connections[Cardinal::SOUTH] = t2;
	t2->connections[Cardinal::SOUTH] = t3;

	t2->connections[Cardinal::NORTH] = t1;
	t3->connections[Cardinal::NORTH] = t2;

	t1->onActivate = std::bind(&UIMainMenu::startGame, this);
	t2->onActivate = std::bind(&UIMainMenu::toOptions, this);
	t3->onActivate = std::bind(&UIMainMenu::quit, this);

	uiGraph.setSelected(t1);
}

void UIMainMenu::update(sf::Time deltaTime) {
	uiGraph.update(deltaTime);
}

void UIMainMenu::message(std::string s) {
	Log::msg(s);
}

void UIMainMenu::startGame() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = new GameplayState();
}

void UIMainMenu::toOptions() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = new UIOptions();
}

void UIMainMenu::quit() {
	removeStateOnChange = true;
	quitGame = true;
}

void UIMainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(uiGraph, states);
};