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

	void setPosition(sf::Vector2f pos);

	void setTexFile(TextureFile& texFile);
	TextureFile* getTexFile();

	void update(sf::Time t);
	void updateFrame();
	void updateSpritePos(sf::Vector2f pos);

	bool isPlaying(const std::string& animName, bool includeChainTo = false);

	void setAnimation(const std::string& animName, float timeScale = 1.f);
	void setAnimation(Animation* anim, float timeScale = 1.f);
	void setTimeScale(float tScale);

	const Animation* getAnimation();

	void swapAnimation(const std::string& animName);
	void swapAnimation(Animation* anim);
	
	int getFrame();
	void setFrame(int i);
	bool onLastFrame();
	bool isNextFrameWithin(sf::Time deltaTime);
	
	const sf::Sprite& getSprite();

	//horizontal flip
	void setHFlip(bool i);
	bool getHFlip();
	
	int getLoopCount();
	float getAnimProgress();
	bool isComplete();
	
private:
	
	TextureFile* tex;

	Animation* activeAnim = nullptr;

	// speed up or slow down animation
	// carries through animation chains
	float animTimeScale = 1.f;

	//starts at one
	int currentFrame = 0;

	//the sprite that gets drawn
	sf::Sprite sprite;

	//for knowing when to switch frames
	sf::Time acc = sf::Time::Zero;

	bool hFlipped = false;

	//how many times has this animation looped
	int loopCount = 0;

	bool completedCurrentAnim = false;

	sf::Vector2f position;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		if (activeAnim != nullptr)
			target.draw(sprite, states);
	};
};
#endif
