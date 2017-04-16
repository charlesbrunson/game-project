#include "UIMainMenu.hpp"
#include "UIOptions.hpp"
#include "CamRef.hpp"

UIOptions::UIOptions() {

	cameraPos = GAMEDIMENSIONS / 2.f;
	gCameraPos = cameraPos;

	sf::Text text;
	text.setFont(RL()->getFont("pixelated.ttf"));
	text.setCharacterSize(8);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);

	UIButton* back = uiGraph.createUIButton("BACK");
	back->setPosition(sf::Vector2f(25.f, Math::bottomleft(getCameraArea()).y - 60.f));
	back->onActivate = std::bind(&UIOptions::backToMain, this);

	UIToggle* test = uiGraph.createUIToggle();
	test->setPosition(sf::Vector2f(25.f, Math::bottomleft(getCameraArea()).y - 100.f));

	back->connections[Cardinal::NORTH] = test;
	test->connections[Cardinal::SOUTH] = back;

	uiGraph.setSelected(back);
}

UIOptions::~UIOptions() {
	// save all options
}

void UIOptions::update(sf::Time deltaTime) {
	uiGraph.update(deltaTime);
}

void UIOptions::backToMain() {
	removeStateOnChange = true;
	toNextState = true;
	nextState = new UIMainMenu();
}

void UIOptions::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(uiGraph, states);
}