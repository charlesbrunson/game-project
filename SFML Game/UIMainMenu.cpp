
#include "UIMainMenu.hpp"
#include "CamRef.hpp"
#include "Log.hpp"

#include "GameplayState.hpp"

UIMainMenu::UIMainMenu() {
	cameraPos = GAMEDIMENSIONS / 2.f;
	gCameraPos = cameraPos;

	sf::Text text;
	text.setFont(*RL()->getFont("pixelated.ttf"));
	text.setCharacterSize(8);
	text.setFillColor(sf::Color::White);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(1.f);

	UIText* t1 = uiGraph.createUIText();
	t1->setArea(sf::FloatRect(25.f, 25.f, 120.f, 35.f));
	t1->copyTextOptions(text);
	t1->setString("START THE\nGAAAAAAAAME");
	t1->setAlignment(UIText::CENTER, UIText::CENTER);
	t1->updateText();

	UIText* t2 = uiGraph.createUIText();
	t2->copy(*t1);
	t2->setArea(sf::FloatRect(25.f, 80.f, 120.f, 35.f));
	t2->setAlignment(UIText::START, UIText::END);
	t2->setString("ANOTHER BUTTON\nDONT WORRY\nABOUT\nIT");
	t2->updateText();

	UIText* t3 = uiGraph.createUIText();
	t3->copy(*t1);
	t3->setArea(sf::FloatRect(25.f, 135.f, 120.f, 35.f));
	t3->setAlignment(UIText::END, UIText::START);
	t3->setString("HEY\nGUESS WHAT\nTHATS RIGHT\nITS A BUTTON");
	t3->updateText();

	t1->connections[Cardinal::SOUTH] = t2;
	t2->connections[Cardinal::SOUTH] = t3;

	t2->connections[Cardinal::NORTH] = t1;
	t3->connections[Cardinal::NORTH] = t2;

	t1->onActivate = std::bind(&UIMainMenu::startGame, this);
	t2->onActivate = std::bind(&UIMainMenu::message, this, "BUTTON 2\n");
	t3->onActivate = std::bind(&UIMainMenu::message, this, "BUTTON 3\n");

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

void UIMainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(uiGraph, states);
};