#include "AnimSprite.hpp"


AnimSprite::AnimSprite() {

};

bool AnimSprite::isPlaying(Animation &a, bool includeChainTo) {
	return activeAnim == &a || (includeChainTo && activeAnim->chainTo == &a);
}

void AnimSprite::setAnimation(Animation &anim) {
	activeAnim = &anim;
	currentFrame = 0;
	completedCurrentAnim = false;
	loopCount = 0;
	acc = sf::Time::Zero;
};

void AnimSprite::swapAnimation(Animation &anim) {

	if (activeAnim->numOfFrames > anim.numOfFrames) {
		currentFrame = (currentFrame % anim.numOfFrames);
	}

	activeAnim = &anim;
	updateSpritePos(position);
}

void AnimSprite::update(sf::Time t) {
	if (t <= sf::Time::Zero) return;
	if (activeAnim == nullptr) return;

	if (activeAnim->numOfFrames > 1 || activeAnim->loop != -1)
		acc += t;

	if (activeAnim->frameTimes[currentFrame] > sf::Time::Zero) {
		while (acc >= activeAnim->frameTimes[currentFrame]) {

			acc -= activeAnim->frameTimes[currentFrame];

			if (atLastFrame()) {
				completedCurrentAnim = true;
				if (activeAnim->loop != 0){
					loopCount++;
					currentFrame = 0;
					if (activeAnim->loop > 0 && loopCount <= activeAnim->loop) {
						completedCurrentAnim = false;
					}
				}
				if (completedCurrentAnim && activeAnim->chainTo != nullptr) {

					currentFrame = activeAnim->chainStartOnFrame;
					swapAnimation(*activeAnim->chainTo);

				}
			}
			else {
				currentFrame++;
			}
		}
	}
};

int AnimSprite::getFrame() {
	return currentFrame;
};

void AnimSprite::setFrame(int i) {
	currentFrame = i;
	acc = sf::Time::Zero;
};

bool AnimSprite::atLastFrame() {
	return currentFrame >= activeAnim->numOfFrames - 1;
};

bool AnimSprite::isSwitchingFramesNextStep(sf::Time deltaTime) {
	return acc + deltaTime >= activeAnim->frameTimes[currentFrame];
}

bool AnimSprite::completedCurrentAnimation() {
	return completedCurrentAnim;
};

//sprite ref
sf::Sprite *AnimSprite::getSprite() {
	return &sprite;
};

//horizontal flip
void AnimSprite::setHFlip(bool i) {
	bool u = hFlipped != i;

	hFlipped = i;

	if (u) 
		updateFrame();
};
bool AnimSprite::getHFlip() {
	return hFlipped;
};

Animation *AnimSprite::getActiveAnimation() {
	return activeAnim;
};

const int AnimSprite::getLoopCount() {
	return loopCount;
}

const float AnimSprite::getAnimProgress() {
	//float frame = 1.f / (activeAnim->numOfFrames);
	//float p = (frame * currentFrame) + ((acc.asSeconds() / activeAnim->frameTimes[currentFrame].asSeconds()) * frame);

	float p = (acc + getAnimationLength(currentFrame)) / getAnimationLength();

	return p;
}

const sf::Time AnimSprite::getAnimationLength(int frames) {
	sf::Time sum;
	int stop = frames != -1 ? frames : activeAnim->numOfFrames;
	for (int i = 0; i < stop; i++)
		sum += activeAnim->frameTimes[i];

	return sum;
}

//call this in update of entity
void AnimSprite::updateFrame() {

	if (activeAnim == nullptr)
		return;

	sf::IntRect a(activeAnim->area);

	if (currentFrame > 0)
		a.left += activeAnim->area.width * currentFrame;

	sprite.setTextureRect(a);

	//flip sprite
	float flip = 1.f;
	if (hFlipped) flip = -1.f;

	sprite.setScale(sf::Vector2f(1.f * flip, 1.f));
};