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
		text.setCharacterSize(8);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1.f);

		UIText* t1 = new UIText(r);
		t1->setArea(sf::FloatRect(25.f, 25.f, 120.f, 35.f));
		t1->copyTextOptions(text);
		t1->setString("JUST A BUTTON\nNO REALLY JUST A BUTTON\nWOW");
		t1->setAlignment(UIText::HCENTER, UIText::VCENTER);
		t1->updateText();
		uiElements.push_back(t1);


		UIText* t2 = new UIText(r);
		t2->copy(*t1);
		t2->setArea(sf::FloatRect(25.f, 80.f, 120.f, 35.f));
		t2->setAlignment(UIText::LEFT, UIText::BOTTOM);
		t2->setString("ANOTHER BUTTON\nDONT WORRY\nABOUT\nIT");
		t2->updateText();
		uiElements.push_back(t2);

		UIText* t3 = new UIText(r);
		t3->copy(*t1);
		t3->setArea(sf::FloatRect(25.f, 135.f, 120.f, 35.f));
		t3->setAlignment(UIText::RIGHT, UIText::TOP);
		t3->setString("HEY\nGUESS WHAT\nTHATS RIGHT\nITS A BUTTON");
		t3->updateText();
		uiElements.push_back(t3);

		t1->connections[UIElement::Direction::SOUTH] = t2;
		t2->connections[UIElement::Direction::SOUTH] = t3;

		t2->connections[UIElement::Direction::NORTH] = t1;
		t3->connections[UIElement::Direction::NORTH] = t2;

		t1->onActivate = std::bind(message, "BUTTON 1\n");
		t2->onActivate = std::bind(message, "BUTTON 2\n");
		t3->onActivate = std::bind(message, "BUTTON 3\n");
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