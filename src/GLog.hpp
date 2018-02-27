#ifndef GLOG_H
#define GLOG_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Log.hpp"

//graphical representation of log contents
class GLog : public sf::Drawable, public LogListener {
public:

	GLog();

	void update();

	void fixMessage();
	void fixTracker();

private:

	void updateTextPosition(const sf::FloatRect& camArea);

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	sf::Text trackerBox;
	sf::Text messageBox;

	sf::FloatRect curArea;
};

#endif
