#ifndef TRANSITIONHORIZONTALSTATE_H
#define TRANSITIONHORIZONTALSTATE_H

#include <SFML/Graphics.hpp>

#include "trans/TransitionState.hpp"

class TransitionHorizontalState : public TransitionState {
public:
	TransitionHorizontalState(GameplayState *game, Transition *activatedTransition);
	
protected:

	bool toLeft = false;

	//background sprite for left and right levels
	sf::Sprite bgLeft;
	sf::Sprite bgRight;

	void updateBGSprites();
	void initCovers(const sf::FloatRect &offsetArea);

	void drawBG(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif //TRANSITIONHORIZONTALSTATE_H
