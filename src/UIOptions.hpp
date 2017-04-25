#ifndef UIOPTIONS_H
#define UIOPTIONS_H

#include "SFML/Graphics.hpp"

#include "State.hpp"
#include "UIGraph.hpp"

class UIOptions : public State {
public:
	UIOptions();

	// Saves all options to settings
	~UIOptions();

	void update(sf::Time deltaTime);

private:
	UIGraph uiGraph;

	void backToMain();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif