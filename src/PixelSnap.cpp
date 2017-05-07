#include "PixelSnap.hpp"

sf::Vector2f snapToPixel(sf::Vector2f p) {
	return pixelSnapping ? sf::Vector2f(nearbyintf(p.x), nearbyintf(p.y)) : p;
}

float snapToPixel(float i) {
	return pixelSnapping ? nearbyintf(i) : i;
}

