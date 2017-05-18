#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

class Animation {
public:
	Animation();

	Animation(
		sf::IntRect spritesheetPosition,
		sf::Vector2f ori,
		int frameCount,
		sf::Time frameRate,
		int looping = -1,
		Animation *chain = nullptr,
		int chainStartFrame = 0);

	Animation(
		sf::IntRect spritesheetPosition,
		sf::Vector2f ori,
		std::vector<int> framesAsMS,
		int looping = -1,
		Animation *chain = nullptr,
		int chainStartFrame = 0);

	std::string name = "";

	sf::IntRect area;
	sf::Vector2f origin;

	int numOfFrames = 1;
	std::vector<sf::Time> frameTimes;
	const sf::Time duration(int frames = -1) const;

	//sf::Time frameSpeed = sf::seconds(1.f);
	int loop = -1;

	//animation chains will automatically swap animations once this animation is complete
	std::string chainToName = "";
	Animation *chainTo = nullptr;
	int chainStartOnFrame = 0;

	void setFrameSpeed(sf::Time rate);
};

#endif
