#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>

#include "ResourceLoader.hpp"
#include "Globals.hpp"

//generic class for eventual state machine
class State : public sf::Drawable {

public:

	State(ResourceLoader *r) {
		res = r;
	}
	virtual ~State() {};
		
	virtual void update(sf::Time deltaTime) = 0; 
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const = 0;
	
	//camera position
	sf::Vector2f cameraPos = sf::Vector2f(GAMEWIDTH / 2u, GAMEHEIGHT / 2u);

	//delete this state upon state change
	bool removeStateOnChange = true;
	
	void setNextState(State* st) {
		nextState = st;
	}
	State* getNextState() {
		return nextState;
	}
	bool hasNextState() {
		return nextState != nullptr;
	}

	//message flags to Game class
	bool toNextState = false;
	bool quitGame = false;

	//color to use when clearing screen
	sf::Color clearColor = sf::Color::Black;
	
protected:
	ResourceLoader *res;
	State* nextState = nullptr;
};

#endif