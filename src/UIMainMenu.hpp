#ifndef UIMAINMENU_H
#define UIMAINMENU_H


#include <SFML/Graphics.hpp>

#include "UIGraph.hpp"

class UIMainMenu : public State {
public:
	UIMainMenu();

	void update(sf::Time deltaTime);

protected:

	void message(std::string s);

	UIGraph uiGraph;

	void startGame();

	void toOptions();

	void quit();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif