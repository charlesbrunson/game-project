#ifndef UIPAUSESTATE_H
#define UIPAUSESTATE_H

#include "UIState.hpp"
#include "Controls.hpp"

class UIPauseState : public UIState {
public:
	UIPauseState(State* prevState, ResourceLoader *r) : UIState(r) {
		//StateName = STATE_UI;
		prev = prevState;
		cameraPos = prev->cameraPos;

		pauseText.setFont(*r->getFont("pixelated.ttf"));
		pauseText.setString("PAUSED");
		pauseText.setCharacterSize(8);
		pauseText.setFillColor(sf::Color::White);
		pauseText.setOutlineColor(sf::Color::Black);
		pauseText.setOutlineThickness(1.f);
	}

	void update(sf::Time deltaTime) {

		pauseText.setPosition(cameraPos 
			- sf::Vector2f(pauseText.getLocalBounds().width, pauseText.getLocalBounds().height)/2.f);

		if (Controls::isPressed(Controls::Input::START)) {
			//pause menu
			Controls::confirmedPress(Controls::Input::START);

			removeStateOnChange = true;
			toNextState = true;
			nextState = prev;

			return;
		}
	};
	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		prev->draw(target, states);
		target.draw(pauseText, res->getShader("noalpha.frag"));
	};

private:
	State* prev;
	sf::Text pauseText;
};

#endif