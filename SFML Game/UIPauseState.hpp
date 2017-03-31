#ifndef UIPAUSESTATE_H
#define UIPAUSESTATE_H

#include "UIState.hpp"
#include "UIText.hpp"
#include "Controls.hpp"

class UIPauseState : public UIState {
public:
	UIPauseState(State* prevState, ResourceLoader *r) : UIState(r) {
		//StateName = STATE_UI;
		prev = prevState;
		cameraPos = prev->cameraPos;

		pauseText.setFont(*r->getFont("pixelated.ttf"));
		pauseText.setCharacterSize(8);
		pauseText.setFillColor(sf::Color::White);
		pauseText.setOutlineColor(sf::Color::Black);
		pauseText.setOutlineThickness(1.f);

		UIText* t = new UIText(r);
		t->setArea(getCameraArea());
		t->setAlignment(UIText::LEFT, UIText::BOTTOM);
		t->setString("PAUSED");
		t->updateText();
		uiElements.push_back(t);
	}

	void update(sf::Time deltaTime) {
		
		if (Controls::isPressed(Controls::Input::START)) {
			//pause menu
			Controls::confirmPress(Controls::Input::START);

			removeStateOnChange = true;
			toNextState = true;
			nextState = prev;

			return;
		}
	};

protected:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		prev->draw(target, states);
		UIState::draw(target, states);
	};

private:
	State* prev;
	sf::Text pauseText;
};

#endif