#ifndef PIXELSNAP_H
#define PIXELSNAP_H

#include <SFML/System.hpp>
#include "Globals.hpp"

static sf::Vector2f snapToPixel(sf::Vector2f p) {
	//return pixelSnapping ? sf::Vector2f(std::floorf(p.x + 0.5f), std::floorf(p.y + 0.5f)) : p;
	return pixelSnapping ? sf::Vector2f(nearbyintf(p.x), nearbyintf(p.y)) : p;
}

static float snapToPixel(float i) {
	return pixelSnapping ? nearbyintf(i) : i;
}

#endif