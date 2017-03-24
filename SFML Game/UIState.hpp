#ifndef UISTATE_H
#define UISTATE_H

#include <SFML/Graphics.hpp>

#include "State.hpp"

//incomplete

class UIState : public State {
public:
	UIState(ResourceLoader *r) : State(r) {
		//StateName = STATE_UI;
	}

private:

};

#endif