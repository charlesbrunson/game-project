#include "AnimSprite.hpp"

#include <set>

AnimSprite::AnimSprite() {

};

void AnimSprite::setPosition(sf::Vector2f pos) {
	sprite.setPosition(pos);
}

void AnimSprite::setTexFile(TextureFile& texFile) {
	tex = &texFile;
	sprite.setTexture(tex->get());
}

TextureFile* AnimSprite::getTexFile() {
	return tex;
}

bool AnimSprite::isPlaying(const std::string& animName, bool includeChainTo) {
	Animation *playing = tex->getAnimation(animName);
	//TODO implement some iteration through animation chain
	/*
	std::set<Animation*> onChain;

	if (!playing || !activeAnim)
		return false;

	if (playing == activeAnim) {
		return true;
	}
	else if (includeChainTo) {
		Animation* st = activeAnim;
		onChain.insert(st);
		while (st->chainTo && onChain.find(st->chainTo) == onChain.end()) {
			st = st->chainTo;
			if (st == activeAnim) {
				return true;
			}
			onChain.insert(st);
		}
		return false;
	}
	*/
	return activeAnim == playing || (includeChainTo && activeAnim != nullptr && activeAnim->chainTo == playing);
}
const Animation* AnimSprite::getAnimation() {
	return activeAnim;
}

void AnimSprite::setAnimation(const std::string& animName, float timeScale) {
	activeAnim = tex->getAnimation(animName);
	currentFrame = 0;
	completedCurrentAnim = false;
	loopCount = 0;
	acc = sf::Time::Zero;
	animTimeScale = timeScale;
	//updateFrame();
}

void AnimSprite::setAnimation(Animation* anim, float timeScale) {
	activeAnim = anim;
	currentFrame = 0;
	completedCurrentAnim = false;
	loopCount = 0;
	acc = sf::Time::Zero;
	animTimeScale = timeScale;
	//updateFrame();
};


void AnimSprite::setTimeScale(float tScale) {
	animTimeScale = tScale;
}

void AnimSprite::swapAnimation(const std::string& animName) {
	Animation* a = tex->getAnimation(animName);

	if (activeAnim == nullptr) {
		setAnimation(a, animTimeScale);
		return;
	}

	if (activeAnim->numOfFrames > a->numOfFrames) {
		currentFrame = (currentFrame % a->numOfFrames);
	}

	activeAnim = a;
	updateSpritePos(position);
}

void AnimSprite::swapAnimation(Animation* anim) {
	if (activeAnim == nullptr) {
		setAnimation(anim, animTimeScale);
		return;
	}

	if (activeAnim->numOfFrames > anim->numOfFrames) {
		currentFrame = (currentFrame % anim->numOfFrames);
	}

	activeAnim = anim;
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
					swapAnimation(activeAnim->chainTo);

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
const sf::Sprite& AnimSprite::getSprite() {
	return sprite;
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
	sf::Vector2f ori = activeAnim != nullptr ? activeAnim->origin : sf::Vector2f();

	float flippedOrigin = getHFlip() ? ori.x : -ori.x;

	sprite.setPosition(snapToPixel(sf::Vector2f(pos.x + flippedOrigin, pos.y - ori.y)));

	updateFrame();
	position = pos;
}