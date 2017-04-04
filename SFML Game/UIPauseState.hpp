#ifndef UIPAUSESTATE_H
#define UIPAUSESTATE_H

#include "UIGraph.hpp"

class UIPauseState : public State {
public:
	UIPauseState(State* prevState, ResourceLoader* r);
	
	void update(sf::Time deltaTime);

private:
	State* prev;
	sf::Text pauseText;

	UIGraph uiGraph;

	void resume();
	void quitToMenu();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif