#include "game/CamRef.hpp"
#include "util/PixelSnap.hpp"

//position references
sf::Vector2f gCameraPos = sf::Vector2f(0.f, 0.f);
//sf::Vector2f gPlayerPos = sf::Vector2f(0.f, 0.f);

//GameObject* gPlayer = nullptr;

const sf::FloatRect getCameraArea() {
	sf::FloatRect camArea;
	sf::Vector2f lefttop(gCameraPos - (GAMEDIMENSIONS / 2.f));

	camArea.left = lefttop.x;
	camArea.top = lefttop.y;
	camArea.width = GAMEDIMENSIONS.x;
	camArea.height = GAMEDIMENSIONS.y;

	return camArea;
}
