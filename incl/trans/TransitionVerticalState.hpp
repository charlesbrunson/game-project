#ifndef TRANSITIONVERTICALSTATE_H
#define TRANSITIONVERTICALSTATE_H


#include <SFML/Graphics.hpp>

#include "trans/TransitionState.hpp"

class TransitionVerticalState : public TransitionState {
public:
	TransitionVerticalState(GameplayState *game, Transition *activatedTransition);

protected:

	bool toUp = false;

	//background sprite for left and right levels
	sf::Sprite bgUp;
	sf::Sprite bgDown;

	void updateBGSprites();
	void initCovers(const sf::FloatRect &offsetArea);

	void drawBG(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
