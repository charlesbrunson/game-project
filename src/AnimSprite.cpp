#include "AnimSprite.hpp"


AnimSprite::AnimSprite() {

};

bool AnimSprite::isPlaying(Animation &a, bool includeChainTo) {
	return activeAnim == &a || (includeChainTo && activeAnim != nullptr && activeAnim->chainTo == &a);
}

void AnimSprite::setAnimation(Animation &anim, float timeScale) {
	activeAnim = &anim;
	currentFrame = 0;
	completedCurrentAnim = false;
	loopCount = 0;
	acc = sf::Time::Zero;
	animTimeScale = timeScale;
};
void AnimSprite::setTimeScale(float tScale) {
	animTimeScale = tScale;
}

void AnimSprite::swapAnimation(Animation &anim) {
	if (activeAnim == nullptr) {
		setAnimation(anim, animTimeScale);
		return;
	}

	if (activeAnim->numOfFrames > anim.numOfFrames) {
		currentFrame = (currentFrame % anim.numOfFrames);
	}

	activeAnim = &anim;
	updateSpritePos(position);
}

void AnimSprite::update(sf::Time t) {
	t *= animTimeScale;

	if (t <= sf::Time::Zero || activeAnim == nullptr) 
		return;

	if (activeAnim->numOfFrames > 1 || activeAnim->loop != -1)
		acc += t;

	if (activeAnim->frameTimes[currentFrame] > sf::Time::Zero) {
		while (acc >= activeAnim->frameTimes[currentFrame]) {

			acc -= activeAnim->frameTimes[currentFrame];

			if (onLastFrame()) {
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

bool AnimSprite::onLastFrame() {
	if (activeAnim == nullptr) return false;
	return currentFrame >= activeAnim->numOfFrames - 1;
};

bool AnimSprite::isNextFrameWithin(sf::Time deltaTime) {
	if (activeAnim == nullptr) return false;
	return acc + (deltaTime * animTimeScale) >= activeAnim->frameTimes[currentFrame];
}

bool AnimSprite::isComplete() {
	return completedCurrentAnim;
};

//sprite ref
sf::Sprite* AnimSprite::getSprite() {
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

Animation* AnimSprite::getAnimation() {
	return activeAnim;
};

int AnimSprite::getLoopCount() {
	return loopCount;
}

float AnimSprite::getAnimProgress() {
	if (activeAnim == nullptr) return 0.f;
	return (acc + activeAnim->duration(currentFrame)) / activeAnim->duration();
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

void AnimSprite::updateSpritePos(sf::Vector2f pos) {
	sf::Vector2f ori = getAnimation() != nullptr ? getAnimation()->origin : sf::Vector2f();

	float flippedOrigin = getHFlip() ? ori.x : -ori.x;

	getSprite()->setPosition(snapToPixel(sf::Vector2f(pos.x + flippedOrigin, pos.y - ori.y)));

	updateFrame();
	position = pos;
}