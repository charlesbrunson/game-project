#ifndef ANIMSPRITE_H
#define ANIMSPRITE_H

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <list>

#include "ResourceLoader.hpp"

#include "Animation.hpp"

#include "PixelSnap.hpp"

//class that handles sprite animation
class AnimSprite : public sf::Drawable {
public:

	AnimSprite();

	bool isPlaying(Animation &a, bool includeChainTo = false);
	
	void setAnimation(Animation &anim);

	void swapAnimation(Animation &anim);

	void update(sf::Time t);

	int getFrame();
	void setFrame(int i);

	bool atLastFrame();

	bool isSwitchingFramesNextStep(sf::Time deltaTime);

	bool completedCurrentAnimation();

	void updateSpritePos(sf::Vector2f pos) {
		sf::Vector2f ori = getActiveAnimation() != nullptr ? getActiveAnimation()->origin : sf::Vector2f();

		float flippedOrigin = getHFlip() ? ori.x : -ori.x;

		getSprite()->setPosition(snapToPixel(sf::Vector2f(pos.x + flippedOrigin, pos.y - ori.y)));

		updateFrame();
		position = pos;
	}

	//sprite ref
	sf::Sprite *getSprite();
	
	//horizontal flip
	void setHFlip(bool i);
	bool getHFlip();
	
	Animation *getActiveAnimation();

	const int getLoopCount();

	const float getAnimProgress();

	const sf::Time getAnimationLength(int frames = -1);

	//call this in update of entity
	void updateFrame();

protected:		
	Animation *activeAnim = nullptr;

	//starts at one
	int currentFrame = 0;

	//the sprite that gets drawn
	sf::Sprite sprite;

	//for knowing when to switch frames
	sf::Time acc = sf::Time::Zero;

	bool hFlipped = false;

	//how many times has this animation looped
	int loopCount = 0;
	
private:

	bool completedCurrentAnim = false;

	sf::Vector2f position;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		if (activeAnim != nullptr)
			target.draw(sprite, states);
	};
};
#endif