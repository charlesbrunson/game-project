#ifndef UIMAINMENU_H
#define UIMAINMENU_H


#include <SFML/Graphics.hpp>

#include "UIText.hpp"
#include "UIState.hpp"

#include "CamRef.hpp"

class UIMainMenu : public UIState {
public:
	UIMainMenu(ResourceLoader *r) : UIState(r) {
		cameraPos = GAMEDIMENSIONS / 2.f;
		gCameraPos = cameraPos;

		sf::Text text;
		text.setFont(*r->getFont("pixelated.ttf"));
		text.setString("TEST1");
		text.setCharacterSize(24);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1.f);

		UIText* t1 = new UIText(text, r);
		t1->setArea(sf::FloatRect(25.f, 25.f, 120.f, 35.f));
		uiElements.insert(std::make_pair("text1", t1));
		t1->onActivate = std::bind(message, "THIS IS TEXT1\n");


		text.setString("TEST2");
		UIText* t2 = new UIText(text, r);
		t2->setArea(sf::FloatRect(25.f, 80.f, 120.f, 35.f));
		uiElements.insert(std::make_pair("text2", t2));
		t2->onActivate = std::bind(message, "THIS IS TEXT2\n");

		text.setString("TEST3");
		UIText* t3 = new UIText(text, r);
		t3->setArea(sf::FloatRect(25.f, 135.f, 120.f, 35.f));
		uiElements.insert(std::make_pair("text3", t3));
		t3->onActivate = std::bind(message, "THIS IS TEXT3\n");
		
		t1->connections[UIElement::Direction::SOUTH] = t2;
		t2->connections[UIElement::Direction::NORTH] = t1;

		t2->connections[UIElement::Direction::SOUTH] = t3;
		t3->connections[UIElement::Direction::NORTH] = t2;

		lastElement = t1;
	}
protected:

	static void message(std::string s) {
		Log::msg(s);
	}

	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		UIState::draw(target, states);
	};

};

#endif