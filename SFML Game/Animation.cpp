#include "Animation.hpp"


Animation::Animation() {

}

Animation::Animation(
	sf::IntRect spritesheetPosition,
	sf::Vector2f ori,
	int frameCount,
	sf::Time frameRate,
	int looping,
	Animation *chain,
	int chainStartFrame)
{
	area = spritesheetPosition;
	origin = ori;
	numOfFrames = frameCount;
	loop = looping;
	chainTo = chain;
	chainStartOnFrame = chainStartFrame;

	frameTimes.insert(frameTimes.begin(), frameCount, frameRate);

}

Animation::Animation(
	sf::IntRect spritesheetPosition,
	sf::Vector2f ori,
	std::vector<int> framesAsMS,
	int looping,
	Animation *chain,
	int chainStartFrame)
{
	area = spritesheetPosition;
	origin = ori;
	numOfFrames = framesAsMS.size();
	loop = looping;
	chainTo = chain;
	chainStartOnFrame = chainStartFrame;

	std::transform(framesAsMS.begin(), framesAsMS.end(), frameTimes.begin(),
		[](int c) { return sf::milliseconds(c); });

}

void Animation::setFrameSpeed(sf::Time rate) {
	for (sf::Time& t : frameTimes) {
		t = rate;
	}
}