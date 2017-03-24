#ifndef EFFECT_H
#define EFFECT_H

#include "Entity.hpp"

class Effect : public Entity {
	//display an animation until the animation completes, then die
public:

	enum EffectType : int {
		DEATH_GENERIC,
		HIT_GENERIC,
		COIN_PICKUP,
		SPRINT_FX,
		EXPLOSION
	};

	enum EffectLayer : int {
		OVER,
		UNDER,
	};

	int layer = OVER;

	// Generic constuctor of predefined effect types
	Effect(int type, bool flip, ResourceLoader *rloader);

	// Constuctor, with animation to play and texture to use
	Effect(std::string texFile, Animation ani, int onLayer, bool flip, ResourceLoader *rloader);

	void update(sf::Time deltaTime);

	void setParent(Entity *p, sf::Vector2f off = sf::Vector2f());
	Entity* getParent();

	bool sfxWhite = false;

protected:
	Entity *parent = nullptr;
	sf::Vector2f parentOffset;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		//draw sprite
		target.draw(sprite, ( !sfxWhite ? states : rMan->getShader("whiteout.frag") ));

	};

	Animation anim;
};

#endif