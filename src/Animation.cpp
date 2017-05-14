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


	for (const int& i : framesAsMS) {
		frameTimes.push_back(sf::milliseconds(i));
	}
}

void Animation::setFrameSpeed(sf::Time rate) {
	for (sf::Time& t : frameTimes) {
		t = rate;
	}
}

const sf::Time Animation::duration(int frames) {
	sf::Time sum;
	int stop = frames > -1 ? std::min(frames, numOfFrames) : numOfFrames;
	for (int i = 0; i < stop; i++)
		sum += frameTimes[i];

	return sum;
}