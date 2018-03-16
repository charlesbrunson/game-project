#include "util/PixelSnap.hpp"
#include "Globals.hpp"

sf::Vector2f snapToPixel(sf::Vector2f p) {
	return sf::Vector2f(snapToPixel(p.x), snapToPixel(p.y));
}

float snapToPixel(float i) {
	if (pixelSnapping) {
		return nearbyintf(i);
	}
	else {
		return nearbyintf(i * (float)Gameplay_Globals::gameScale) / (float)Gameplay_Globals::gameScale;
	}
}

